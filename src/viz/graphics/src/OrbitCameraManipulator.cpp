#include <vine/graphics/OrbitCameraManipulator.hpp>

#include <vine/graphics/Camera.hpp>
#include <vine/math/Math.hpp>
#include <algorithm>
#include <cmath>

V_GRAPHICS_NS_BEGIN

OrbitCameraManipulator::OrbitCameraManipulator(Camera* camera)
  : CameraManipulator(camera)
{
    if (camera != nullptr) {
        orbit_center_ = camera->target();
        orbit_radius_ = (camera->eye() - camera->target()).length();
    }
}

OrbitCameraManipulator::~OrbitCameraManipulator() = default;

void OrbitCameraManipulator::orbit(double deltaYaw, double deltaPitch)
{
    yaw_ += deltaYaw;
    pitch_ += deltaPitch;
    const double limit = vine::math::PI_HALF - 1e-3;
    pitch_ = std::clamp(pitch_, -limit, limit);
    apply();
}

void OrbitCameraManipulator::setOrbitCenter(const Vec3d& center)
{
    orbit_center_ = center;
    apply();
}

double OrbitCameraManipulator::orbitRadius() const
{
    return orbit_radius_;
}

void OrbitCameraManipulator::setOrbitRadius(double radius)
{
    orbit_radius_ = std::max(radius, 1e-3);
    apply();
}

void OrbitCameraManipulator::pan(double screenDx, double screenDy)
{
    Camera* cam = camera_;
    if (cam == nullptr) {
        return;
    }
    const double world_per_pixel = 2.0 * orbit_radius_ * std::tan(cam->fieldOfView() * 0.5 * vine::math::DEG_TO_RAD) / 1000.0;
    const Vec3d forward = (cam->target() - cam->eye()).normalized();
    const Vec3d right = forward.cross(cam->up()).normalized();
    const Vec3d up = right.cross(forward).normalized();
    const Vec3d delta = (-right * screenDx + up * screenDy) * world_per_pixel;
    cam->setViewMatrixAsLookAt(cam->eye() + delta, cam->target() + delta, cam->up());
    orbit_center_ += delta;
}

void OrbitCameraManipulator::zoom(double factor)
{
    orbit_radius_ = std::max(orbit_radius_ * factor, 1e-3);
    apply();
}

void OrbitCameraManipulator::moveForward(double distance)
{
    Camera* cam = camera_;
    if (cam == nullptr) {
        return;
    }
    const Vec3d forward = (cam->target() - cam->eye()).normalized();
    cam->setViewMatrixAsLookAt(cam->eye() + forward * distance,
                               cam->target() + forward * distance,
                               cam->up());
}

void OrbitCameraManipulator::moveRight(double distance)
{
    Camera* cam = camera_;
    if (cam == nullptr) {
        return;
    }
    const Vec3d forward = (cam->target() - cam->eye()).normalized();
    const Vec3d right = forward.cross(cam->up()).normalized();
    cam->setViewMatrixAsLookAt(cam->eye() + right * distance,
                               cam->target() + right * distance,
                               cam->up());
}

void OrbitCameraManipulator::moveUp(double distance)
{
    Camera* cam = camera_;
    if (cam == nullptr) {
        return;
    }
    cam->setViewMatrixAsLookAt(cam->eye() + cam->up() * distance,
                               cam->target() + cam->up() * distance,
                               cam->up());
}

void OrbitCameraManipulator::rotate(double deltaYaw, double deltaPitch)
{
    yaw_ += deltaYaw;
    pitch_ += deltaPitch;
    const double limit = vine::math::PI_HALF - 1e-3;
    pitch_ = std::clamp(pitch_, -limit, limit);
    apply();
}

void OrbitCameraManipulator::onResize(const vine::window::ResizeEvent& event)
{
    Camera* cam = camera_;
    if (cam == nullptr || event.width <= 0 || event.height <= 0) {
        return;
    }
    if (cam->projectionType() == Camera::ProjectionType::Perspective) {
        const double aspect = static_cast<double>(event.width) / static_cast<double>(event.height);
        cam->setProjectionMatrixAsPerspective(cam->fieldOfView(), aspect,
                                              cam->nearPlane(), cam->farPlane());
    }
}

void OrbitCameraManipulator::apply()
{
    Camera* cam = camera_;
    if (cam == nullptr) {
        return;
    }
    const double cp = std::cos(pitch_);
    const double sp = std::sin(pitch_);
    const double cy = std::cos(yaw_);
    const double sy = std::sin(yaw_);
    const Vec3d offset{ orbit_radius_ * cp * sy,
                        orbit_radius_ * sp,
                        orbit_radius_ * cp * cy };
    cam->setViewMatrixAsLookAt(orbit_center_ + offset, orbit_center_, cam->up());
}

void OrbitCameraManipulator::onMousePress(const vine::window::MouseEvent& event)
{
    last_x_ = event.x;
    last_y_ = event.y;
    switch (event.button) {
        case vine::window::MouseButton::Left:
            drag_ = CameraManipulator::DragAction::Rotate;
            break;
        case vine::window::MouseButton::Middle:
        case vine::window::MouseButton::Right:
            drag_ = CameraManipulator::DragAction::Pan;
            break;
        default:
            drag_ = CameraManipulator::DragAction::None;
            break;
    }
}

void OrbitCameraManipulator::onMouseMove(const vine::window::MouseEvent& event)
{
    const double dx = event.x - last_x_;
    const double dy = event.y - last_y_;
    last_x_ = event.x;
    last_y_ = event.y;

    switch (drag_) {
        case CameraManipulator::DragAction::Rotate:
            if (mode_ == Mode::FirstPerson) {
                rotate(dx * drag_sensitivity_, -dy * drag_sensitivity_);
            } else {
                orbit(dx * drag_sensitivity_, -dy * drag_sensitivity_);
            }
            break;
        case CameraManipulator::DragAction::Pan:
            pan(dx, dy);
            break;
        default:
            break;
    }
}

void OrbitCameraManipulator::onMouseRelease(const vine::window::MouseEvent& event)
{
    (void)event;
    drag_ = CameraManipulator::DragAction::None;
}

void OrbitCameraManipulator::onScroll(const vine::window::ScrollEvent& event)
{
    if (std::abs(event.deltaY) < 1e-6) {
        return;
    }
    const double factor = std::pow(1.1, event.deltaY);
    zoom(factor);
}

void OrbitCameraManipulator::onKeyDown(const vine::window::KeyEvent& event)
{
    if (mode_ != Mode::FirstPerson) {
        return;
    }
    switch (event.code) {
        case vine::window::KeyCode::W:
            moveForward(move_step_);
            break;
        case vine::window::KeyCode::S:
            moveForward(-move_step_);
            break;
        case vine::window::KeyCode::A:
            moveRight(-move_step_);
            break;
        case vine::window::KeyCode::D:
            moveRight(move_step_);
            break;
        case vine::window::KeyCode::E:
            moveUp(move_step_);
            break;
        case vine::window::KeyCode::Q:
            moveUp(-move_step_);
            break;
        case vine::window::KeyCode::Left:
            rotate(-rotate_step_, 0.0);
            break;
        case vine::window::KeyCode::Right:
            rotate(rotate_step_, 0.0);
            break;
        case vine::window::KeyCode::Up:
            rotate(0.0, -rotate_step_);
            break;
        case vine::window::KeyCode::Down:
            rotate(0.0, rotate_step_);
            break;
        default:
            break;
    }
}

void OrbitCameraManipulator::onKeyUp(const vine::window::KeyEvent& event)
{
    (void)event;
    // Movement is event-driven per key press; nothing to track on release.
}

V_GRAPHICS_NS_END
