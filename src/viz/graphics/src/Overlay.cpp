#include <vine/graphics/Overlay.hpp>

#include <vine/graphics/Camera.hpp>
#include <vine/graphics/RenderPass.hpp>
#include <vine/graphics/Scene.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(Overlay, vine::Object);

Overlay::Overlay()
  : pass_(new RenderPass())
{
    // Overlays draw over the previous frame: do not clear the whole surface.
    pass_->setClearEnabled(false);
}

Overlay::~Overlay() = default;

RenderPass* Overlay::pass() const
{
    return pass_.get();
}

void Overlay::setPass(RenderPass* pass)
{
    pass_ = pass;
}

Scene* Overlay::content() const
{
    return content_.get();
}

void Overlay::setContent(Scene* content)
{
    content_ = content;
}

int Overlay::zOrder() const
{
    return z_order_;
}

void Overlay::setZOrder(int order)
{
    z_order_ = order;
}

bool Overlay::visible() const
{
    return visible_;
}

void Overlay::setVisible(bool visible)
{
    visible_ = visible;
}

void Overlay::setMirrorMode(MirrorMode mode)
{
    mirror_mode_ = mode;
    applyMirror();
}

Overlay::MirrorMode Overlay::mirrorMode() const
{
    return mirror_mode_;
}

void Overlay::setSourceCamera(Camera* camera)
{
    source_camera_ = camera;
    applyMirror();
}

void Overlay::update(double dt)
{
    (void)dt;
    applyMirror();
}

void Overlay::applyMirror()
{
    if (mirror_mode_ == MirrorMode::None) {
        return;
    }
    Camera* src = source_camera_;
    RenderPass* p = pass_.get();
    Camera* dst = (p != nullptr) ? p->camera() : nullptr;
    if (src == nullptr || dst == nullptr) {
        return;
    }

    if (mirror_mode_ == MirrorMode::FullView) {
        dst->setViewMatrixAsLookAt(src->eye(), src->target(), src->up());
        return;
    }

    // Orientation: face the same way as the source while keeping the overlay
    // camera's own framing distance. The overlay content is expected to be
    // centred on its local origin.
    const Vec3d offset = src->target() - src->eye();
    const double len = offset.length();
    if (len < 1e-9) {
        return;
    }
    const Vec3d fwd = offset / len;

    const Vec3d dvec = dst->target() - dst->eye();
    const double dist = dvec.length() > 1e-9 ? dvec.length() : 1.0;

    // Rebuild an un-rolled up basis perpendicular to the view direction.
    Vec3d right = fwd.cross(src->up());
    const double rlen = right.length();
    Vec3d up = dst->up();
    if (rlen >= 1e-9) {
        right /= rlen;
        up = right.cross(fwd).normalized();
    }
    const Vec3d eye = fwd * (-dist);
    dst->setViewMatrixAsLookAt(eye, Vec3d(0.0, 0.0, 0.0), up);
}

V_GRAPHICS_NS_END
