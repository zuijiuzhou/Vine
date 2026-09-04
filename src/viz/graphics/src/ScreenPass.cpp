#include <vine/graphics/ScreenPass.hpp>

#include <vine/graphics/Light.hpp>
#include <vine/graphics/RenderBackend.hpp>
#include <vine/graphics/RenderTarget.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/graphics/ShaderProgram.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(ScreenPass, vine::graphics::RenderPass);

ScreenPass::ScreenPass()
{
    // A screen-space pass composites over previously rendered content:
    // clearing would wipe the main scene before the textured triangle draws.
    setClearEnabled(false);
    setShouldClearDepth(false);
}

ScreenPass::~ScreenPass() = default;

raw_ptr<RenderTarget> ScreenPass::sourceTarget() const
{
    return source_;
}

int ScreenPass::sourceAttachment() const
{
    return source_attachment_;
}

void ScreenPass::setSourceAttachment(int attachment)
{
    source_attachment_ = attachment < 0 ? 0 : attachment;
}

raw_ptr<ShaderProgram> ScreenPass::program() const
{
    return program_.get();
}

void ScreenPass::setProgram(intrusive_ptr<ShaderProgram> program)
{
    program_ = std::move(program);
}

void ScreenPass::resolveInputTextures(const std::vector<raw_ptr<RenderTarget>>& inputs)
{
    source_ = nullptr;
    for (const raw_ptr<RenderTarget> input : inputs) {
        if (input != nullptr) {
            source_ = input;
            break;
        }
    }
}

void ScreenPass::execute(raw_ptr<Scene> scene, raw_ptr<RenderBackend> backend)
{
    if (backend == nullptr || source_ == nullptr) {
        return;
    }
    backend->setRenderTarget(renderTarget());
    if (hasViewport()) {
        int x = 0, y = 0, w = 0, h = 0;
        getViewport(x, y, w, h);
        backend->setViewport(x, y, w, h);
    }
    if (clearEnabled()) {
        backend->clear(clearColor(), shouldClearDepth());
    }
    if (program_ == nullptr) {
        // Plain screen-space copy of one colour attachment.
        backend->drawScreenTexture(source_, source_attachment_);
        return;
    }
    if (camera() == nullptr) {
        return; // the fullscreen program path needs a camera for its view
    }
    // Forward the content scene's lights so the backend can push them to the
    // fullscreen fragment program (mirrors how scene passes feed their lights).
    if (scene != nullptr) {
        std::vector<raw_ptr<const Light>> light_ptrs;
        light_ptrs.reserve(scene->lights().size());
        for (const auto& light : scene->lights()) {
            light_ptrs.push_back(light.get());
        }
        backend->setLights(light_ptrs);
    }
    backend->drawScreenProgram(source_, program_.get(), camera());
}

V_GRAPHICS_NS_END
