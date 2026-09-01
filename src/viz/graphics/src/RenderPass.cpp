#include <vine/graphics/RenderPass.hpp>

#include <vine/graphics/Camera.hpp>
#include <vine/graphics/RenderBackend.hpp>
#include <vine/graphics/RenderCommand.hpp>
#include <vine/graphics/RenderTarget.hpp>
#include <vine/graphics/Scene.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(RenderPass, vine::Object);

struct RenderPass::Data {
    String name;
    RenderTarget* render_target = nullptr;
    Camera* camera = nullptr;
    Color clear_color{ 51, 51, 51, 255 };
    bool clear_depth = true;
};

RenderPass::RenderPass()
  : d(new Data())
{}

String RenderPass::name() const
{
    return d->name;
}

void RenderPass::setName(const String& name)
{
    d->name = name;
}

RenderTarget* RenderPass::renderTarget() const
{
    return d->render_target;
}

void RenderPass::setRenderTarget(RenderTarget* target)
{
    d->render_target = target;
}

Camera* RenderPass::camera() const
{
    return d->camera;
}

void RenderPass::setCamera(Camera* camera)
{
    d->camera = camera;
}

Color RenderPass::clearColor() const
{
    return d->clear_color;
}

void RenderPass::setClearColor(const Color& color)
{
    d->clear_color = color;
}

bool RenderPass::shouldClearDepth() const
{
    return d->clear_depth;
}

void RenderPass::setShouldClearDepth(bool clear)
{
    d->clear_depth = clear;
}

void RenderPass::execute(Scene* scene, RenderBackend* backend)
{
    if (backend == nullptr || scene == nullptr) {
        return;
    }
    backend->setRenderTarget(d->render_target);
    backend->clear(d->clear_color, d->clear_depth);
    const auto commands = scene->collectRenderCommands(d->camera);
    if (d->camera != nullptr) {
        backend->render(commands, d->camera);
    }
}

V_GRAPHICS_NS_END
