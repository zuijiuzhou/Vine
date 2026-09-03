#include <vine/graphics/RenderPipelineBuilder.hpp>

#include <vine/graphics/Camera.hpp>
#include <vine/graphics/RenderEngine.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/graphics/ScreenPass.hpp>

V_GRAPHICS_NS_BEGIN

RenderPipelineBuilder::RenderPipelineBuilder(raw_ptr<RenderEngine> engine)
  : engine_(engine)
{}

RenderPipelineBuilder& RenderPipelineBuilder::setContent(intrusive_ptr<Scene> content)
{
    content_ = std::move(content);
    return *this;
}

RenderPipelineBuilder& RenderPipelineBuilder::setCamera(raw_ptr<Camera> camera)
{
    camera_ = camera;
    return *this;
}

raw_ptr<ScreenPass> RenderPipelineBuilder::addOffscreenToScreen(const String& output_slot,
                                                                int rt_width,
                                                                int rt_height,
                                                                RenderTarget::ColorFormat color_format,
                                                                RenderTarget::DepthFormat depth_format,
                                                                int pip_x, int pip_y, int pip_w, int pip_h)
{
    if (engine_ == nullptr) {
        return nullptr;
    }
    raw_ptr<Camera> camera = (camera_ != nullptr) ? camera_ : engine_->masterCamera();
    if (camera == nullptr) {
        return nullptr;
    }

    // Off-screen target + an order < 0 scene pass that renders into it and
    // publishes the result under the slot name.
    auto target = intrusive_ptr<RenderTarget>(new RenderTarget());
    target->setSize(rt_width, rt_height);
    target->attachColor(color_format);
    target->attachDepth(depth_format);

    auto offscreen = intrusive_ptr<RenderPass>(new RenderPass());
    offscreen->setName(output_slot);
    offscreen->setCamera(camera);
    offscreen->setRenderTarget(target);
    offscreen->setOutputName(output_slot);
    if (content_ != nullptr) {
        engine_->addPass(offscreen, content_, -2);
    } else {
        engine_->addPass(offscreen, -2);
    }
    passes_.push_back(offscreen);

    // An order > 0 ScreenPass sampling the slot into the PiP sub-viewport.
    auto screen = intrusive_ptr<ScreenPass>(new ScreenPass());
    screen->setName(output_slot);
    screen->addInputName(output_slot);
    screen->setViewport(pip_x, pip_y, pip_w, pip_h);
    engine_->addPass(screen, 100);
    passes_.push_back(screen);

    return screen.get();
}

void RenderPipelineBuilder::addPass(intrusive_ptr<RenderPass> pass, int order)
{
    if (engine_ == nullptr || pass == nullptr) {
        return;
    }
    engine_->addPass(std::move(pass), order);
}

V_GRAPHICS_NS_END
