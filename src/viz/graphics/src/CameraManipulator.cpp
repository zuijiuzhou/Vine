#include <vine/graphics/CameraManipulator.hpp>

#include <vine/graphics/Camera.hpp>
#include <vine/math/Math.hpp>

V_GRAPHICS_NS_BEGIN

struct CameraManipulator::Data {
    Camera* camera = nullptr;
    Mode mode = Mode::Orbit;
    Vec3d orbit_center{ 0.0, 0.0, 0.0 };
    double orbit_radius = 10.0;
    double yaw = 0.0;
    double pitch = 0.0;
};

CameraManipulator::CameraManipulator(Camera* camera)
  : d(new Data())
{
    d->camera = camera;
    if (camera != nullptr) {
        d->orbit_center = camera->target();
        d->orbit_radius = (camera->eye() - camera->target()).length();
    }
}

CameraManipulator::Mode CameraManipulator::mode() const
{
    return d->mode;
}

void CameraManipulator::setMode(Mode m)
{
    d->mode = m;
}

void CameraManipulator::orbit(double deltaYaw, double deltaPitch)
{
    d->yaw += deltaYaw;
    d->pitch += deltaPitch;
    const double limit = vine::math::PI_HALF - 1e-3;
    d->pitch = std::clamp(d->pitch, -limit, limit);
    apply();
}

void CameraManipulator::setOrbitCenter(const Vec3d& center)
{
    d->orbit_center = center;
    apply();
}

double CameraManipulator::orbitRadius() const
{
    return d->orbit_radius;
}

void CameraManipulator::setOrbitRadius(double radius)
{
    d->orbit_radius = std::max(radius, 1e-3);
    apply();
}

void CameraManipulator::pan(double screenDx, double screenDy)
{
    if (d->camera == nullptr) {
        return;
    }
    const double world_per_pixel = 2.0 * d->orbit_radius * std::tan(d->camera->fieldOfView() * 0.5 * vine::math::DEG_TO_RAD) / 1000.0;
    const Vec3d forward = (d->camera->target() - d->camera->eye()).normalized();
    const Vec3d right = forward.cross(d->camera->up()).normalized();
    const Vec3d up = right.cross(forward).normalized();
    const Vec3d delta = (-right * screenDx + up * screenDy) * world_per_pixel;
    d->camera->setViewMatrixAsLookAt(d->camera->eye() + delta,
                                     d->camera->target() + delta,
                                     d->camera->up());
    d->orbit_center += delta;
}

void CameraManipulator::zoom(double factor)
{
    d->orbit_radius = std::max(d->orbit_radius * factor, 1e-3);
    apply();
}

void CameraManipulator::moveForward(double distance)
{
    if (d->camera == nullptr) {
        return;
    }
    const Vec3d forward = (d->camera->target() - d->camera->eye()).normalized();
    d->camera->setViewMatrixAsLookAt(d->camera->eye() + forward * distance,
                                     d->camera->target() + forward * distance,
                                     d->camera->up());
}

void CameraManipulator::moveRight(double distance)
{
    if (d->camera == nullptr) {
        return;
    }
    const Vec3d forward = (d->camera->target() - d->camera->eye()).normalized();
    const Vec3d right = forward.cross(d->camera->up()).normalized();
    d->camera->setViewMatrixAsLookAt(d->camera->eye() + right * distance,
                                     d->camera->target() + right * distance,
                                     d->camera->up());
}

void CameraManipulator::moveUp(double distance)
{
    if (d->camera == nullptr) {
        return;
    }
    d->camera->setViewMatrixAsLookAt(d->camera->eye() + d->camera->up() * distance,
                                     d->camera->target() + d->camera->up() * distance,
                                     d->camera->up());
}

void CameraManipulator::rotate(double deltaYaw, double deltaPitch)
{
    d->yaw += deltaYaw;
    d->pitch += deltaPitch;
    const double limit = vine::math::PI_HALF - 1e-3;
    d->pitch = std::clamp(d->pitch, -limit, limit);
    apply();
}

void CameraManipulator::apply()
{
    if (d->camera == nullptr) {
        return;
    }
    const double cp = std::cos(d->pitch);
    const double sp = std::sin(d->pitch);
    const double cy = std::cos(d->yaw);
    const double sy = std::sin(d->yaw);
    const Vec3d offset{ d->orbit_radius * cp * sy,
                        d->orbit_radius * sp,
                        d->orbit_radius * cp * cy };
    d->camera->setViewMatrixAsLookAt(d->orbit_center + offset,
                                     d->orbit_center,
                                     d->camera->up());
}

V_GRAPHICS_NS_END
