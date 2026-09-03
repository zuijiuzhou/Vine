#include <vine/graphics/ScreenPass.hpp>

#include <vine/graphics/RenderBackend.hpp>
#include <vine/graphics/RenderTarget.hpp>
#include <vine/graphics/Scene.hpp>

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

void ScreenPass::execute(raw_ptr<Scene> /*scene*/, raw_ptr<RenderBackend> backend)
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
    backend->drawScreenTexture(source_);
}

V_GRAPHICS_NS_END
