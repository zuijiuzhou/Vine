#include <vine/graphics/Camera.hpp>

#include <vine/math/Transform3.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(Camera, vine::Object);

struct Camera::Data {
    String name;
    Mat4d view{ Mat4d() };
    Mat4d projection{ Mat4d() };
    ProjectionType projection_type = ProjectionType::Perspective;
    // Copy of the last look-at parameters, used for ray generation.
    Vec3d eye{ 0.0, 0.0, 5.0 };
    Vec3d center{ 0.0, 0.0, 0.0 };
    Vec3d up{ 0.0, 1.0, 0.0 };
    // Copy of the last projection parameters, used for queries.
    double near_plane = 0.1;
    double far_plane = 1000.0;
    double fov = 60.0;
    double aspect_ratio = 1.0;
    double ortho_height = 10.0;
};

Camera::Camera()
  : d(new Data())
{
    setViewMatrixAsLookAt(d->eye, d->center, d->up);
    setProjectionMatrixAsPerspective(d->fov, d->aspect_ratio, d->near_plane, d->far_plane);
}

Camera::~Camera()
{
    delete d;
}

String Camera::name() const
{
    return d->name;
}

void Camera::setName(const String& name)
{
    d->name = name;
}

Camera::ProjectionType Camera::projectionType() const
{
    return d->projection_type;
}

void Camera::setViewMatrixAsLookAt(const Vec3d& eye, const Vec3d& center, const Vec3d& up)
{
    d->eye = eye;
    d->center = center;
    d->up = up.normalized();
    d->view = vine::math::lookAt(vine::math::Point3d(eye.x, eye.y, eye.z),
                                 vine::math::Point3d(center.x, center.y, center.z),
                                 d->up);
}

void Camera::setProjectionMatrixAsPerspective(double fovy, double aspect, double zNear, double zFar)
{
    d->projection_type = ProjectionType::Perspective;
    d->fov = fovy;
    d->aspect_ratio = aspect;
    d->near_plane = zNear;
    d->far_plane = zFar;
    const double fov_rad = fovy * vine::math::DEG_TO_RAD;
    d->projection = vine::math::perspective<double>(fov_rad, aspect, zNear, zFar);
}

void Camera::setProjectionMatrixAsOrtho(double left, double right, double bottom, double top,
                                        double zNear, double zFar)
{
    d->projection_type = ProjectionType::Orthographic;
    d->ortho_height = top - bottom;
    d->near_plane = zNear;
    d->far_plane = zFar;
    d->projection = vine::math::ortho<double>(left, right, bottom, top, zNear, zFar);
}

Vec3d Camera::eye() const
{
    return d->eye;
}

Vec3d Camera::target() const
{
    return d->center;
}

Vec3d Camera::up() const
{
    return d->up;
}

double Camera::nearPlane() const
{
    return d->near_plane;
}

double Camera::farPlane() const
{
    return d->far_plane;
}

double Camera::fieldOfView() const
{
    return d->fov;
}

double Camera::aspectRatio() const
{
    return d->aspect_ratio;
}

double Camera::orthographicHeight() const
{
    return d->ortho_height;
}

Mat4d Camera::viewMatrix() const
{
    return d->view;
}

Mat4d Camera::projectionMatrix() const
{
    return d->projection;
}

Ray Camera::screenToWorldRay(const Vec2d& screenPos) const
{
    const double ndc_x = (2.0 * screenPos.x) - 1.0;
    const double ndc_y = 1.0 - (2.0 * screenPos.y);

    const Vec3d forward = (d->center - d->eye).normalized();
    const Vec3d right = forward.cross(d->up).normalized();
    const Vec3d up = right.cross(forward).normalized();

    if (d->projection_type == ProjectionType::Orthographic) {
        // Orthographic: direction is camera forward, origin is offset in the
        // near plane.
        const double half_h = d->ortho_height * 0.5;
        const double half_w = half_h * d->aspect_ratio;
        const Vec3d origin = d->eye + right * (ndc_x * half_w) + up * (ndc_y * half_h);
        return Ray(origin, forward);
    }
    // Perspective: unproject through the near plane.
    const double fov_rad = d->fov * vine::math::DEG_TO_RAD;
    const double tan_half = std::tan(fov_rad * 0.5);
    const double x = ndc_x * tan_half * d->aspect_ratio;
    const double y = ndc_y * tan_half;
    const Vec3d dir = (forward + right * x + up * y).normalized();
    return Ray(d->eye, dir);
}

V_GRAPHICS_NS_END
