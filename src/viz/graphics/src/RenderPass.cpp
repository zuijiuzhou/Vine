#include <vine/graphics/RenderPass.hpp>

#include <vine/graphics/Camera.hpp>
#include <vine/graphics/RenderBackend.hpp>
#include <vine/graphics/RenderCommand.hpp>
#include <vine/graphics/RenderTarget.hpp>
#include <vine/graphics/Scene.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(RenderPass, vine::Object);

RenderPass::RenderPass() = default;

String RenderPass::name() const
{
    return name_;
}

void RenderPass::setName(const String& name)
{
    name_ = name;
}

RenderTarget* RenderPass::renderTarget() const
{
    return render_target_;
}

void RenderPass::setRenderTarget(RenderTarget* target)
{
    render_target_ = target;
}

Camera* RenderPass::camera() const
{
    return camera_;
}

void RenderPass::setCamera(Camera* camera)
{
    camera_ = camera;
}

Color RenderPass::clearColor() const
{
    return clear_color_;
}

void RenderPass::setClearColor(const Color& color)
{
    clear_color_ = color;
}

bool RenderPass::shouldClearDepth() const
{
    return clear_depth_;
}

void RenderPass::setShouldClearDepth(bool clear)
{
    clear_depth_ = clear;
}

void RenderPass::execute(Scene* scene, RenderBackend* backend)
{
    if (backend == nullptr || scene == nullptr) {
        return;
    }
    backend->setRenderTarget(render_target_);
    backend->clear(clear_color_, clear_depth_);
    const auto commands = scene->collectRenderCommands(camera_);
    if (camera_ != nullptr) {
        backend->render(commands, camera_);
    }
}

V_GRAPHICS_NS_END
