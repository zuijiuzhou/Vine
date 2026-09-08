#include <vine/graphics/Camera.hpp>

#include <vine/math/Transform3.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(Camera, vine::Object);

Camera::Camera()
{
    setViewMatrixAsLookAt(eye_, center_, up_);
    setProjectionMatrixAsPerspective(fov_, aspect_ratio_, near_plane_, far_plane_);
}

Camera::~Camera() = default;

String Camera::name() const
{
    return name_;
}

void Camera::setName(const String& name)
{
    name_ = name;
}

Camera::ProjectionType Camera::projectionType() const
{
    return projection_type_;
}

void Camera::setViewMatrixAsLookAt(const Vec3d& eye, const Vec3d& center, const Vec3d& up)
{
    eye_ = eye;
    center_ = center;
    up_ = up.normalized();
    view_ = vine::math::lookAt(vine::math::Point3d(eye.x, eye.y, eye.z),
                               vine::math::Point3d(center.x, center.y, center.z),
                               up_);
}

void Camera::setProjectionMatrixAsPerspective(double fovy, double aspect, double zNear, double zFar)
{
    projection_type_ = ProjectionType::Perspective;
    fov_ = fovy;
    aspect_ratio_ = aspect;
    near_plane_ = zNear;
    far_plane_ = zFar;
    const double fov_rad = fovy * vine::math::DEG_TO_RAD;
    projection_ = vine::math::perspective<double>(fov_rad, aspect, zNear, zFar);
}

void Camera::setProjectionMatrixAsOrtho(double left, double right, double bottom, double top,
                                        double zNear, double zFar)
{
    projection_type_ = ProjectionType::Orthographic;
    ortho_height_ = top - bottom;
    near_plane_ = zNear;
    far_plane_ = zFar;
    projection_ = vine::math::ortho<double>(left, right, bottom, top, zNear, zFar);
}

Vec3d Camera::eye() const
{
    return eye_;
}

Vec3d Camera::target() const
{
    return center_;
}

Vec3d Camera::up() const
{
    return up_;
}

double Camera::nearPlane() const
{
    return near_plane_;
}

double Camera::farPlane() const
{
    return far_plane_;
}

double Camera::fieldOfView() const
{
    return fov_;
}

double Camera::aspectRatio() const
{
    return aspect_ratio_;
}

double Camera::orthographicHeight() const
{
    return ortho_height_;
}

Mat4d Camera::viewMatrix() const
{
    return view_;
}

Mat4d Camera::projectionMatrix() const
{
    return projection_;
}

Ray Camera::screenToWorldRay(const Vec2d& screenPos) const
{
    const double ndc_x = (2.0 * screenPos.x) - 1.0;
    const double ndc_y = 1.0 - (2.0 * screenPos.y);

    const Vec3d forward = (center_ - eye_).normalized();
    const Vec3d right = forward.cross(up_).normalized();
    const Vec3d up = right.cross(forward).normalized();

    if (projection_type_ == ProjectionType::Orthographic) {
        // Orthographic: direction is camera forward, origin is offset in the
        // near plane.
        const double half_h = ortho_height_ * 0.5;
        const double half_w = half_h * aspect_ratio_;
        const Vec3d origin = eye_ + right * (ndc_x * half_w) + up * (ndc_y * half_h);
        return Ray(origin, forward);
    }
    // Perspective: unproject through the near plane.
    const double fov_rad = fov_ * vine::math::DEG_TO_RAD;
    const double tan_half = std::tan(fov_rad * 0.5);
    const double x = ndc_x * tan_half * aspect_ratio_;
    const double y = ndc_y * tan_half;
    const Vec3d dir = (forward + right * x + up * y).normalized();
    return Ray(eye_, dir);
}

V_GRAPHICS_NS_END
