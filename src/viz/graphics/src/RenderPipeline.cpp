#include <vine/graphics/RenderPipeline.hpp>

#include <vine/graphics/AxisGizmo.hpp>
#include <vine/graphics/RenderPass.hpp>
#include <vine/graphics/RenderTarget.hpp>

V_GRAPHICS_NS_BEGIN

Pipeline::Pipeline() = default;

Pipeline::~Pipeline() = default;

raw_ptr<RenderPass> Pipeline::windowPass() const
{
    return window_pass_.get();
}

raw_ptr<RenderTarget> Pipeline::offscreenTarget() const
{
    return offscreen_target_.get();
}

raw_ptr<AxisGizmo> Pipeline::gizmo() const
{
    return gizmo_.get();
}

void Pipeline::resize(int width, int height)
{
    if (width <= 0 || height <= 0) {
        return;
    }
    if (offscreen_target_ != nullptr) {
        offscreen_target_->setSize(width, height);
    }
    if (gizmo_ != nullptr) {
        gizmo_->onSurfaceResized(width, height);
    }
}

void Pipeline::retainPass(intrusive_ptr<RenderPass> pass)
{
    passes_.push_back(std::move(pass));
}

void Pipeline::setWindowPass(intrusive_ptr<RenderPass> pass)
{
    window_pass_ = std::move(pass);
}

void Pipeline::setOffscreenTarget(intrusive_ptr<RenderTarget> target)
{
    offscreen_target_ = std::move(target);
}

void Pipeline::setGizmo(intrusive_ptr<AxisGizmo> gizmo)
{
    gizmo_ = std::move(gizmo);
}

V_GRAPHICS_NS_END
