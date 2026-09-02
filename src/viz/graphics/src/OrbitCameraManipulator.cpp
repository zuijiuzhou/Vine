#include <vine/graphics/OrbitCameraManipulator.hpp>

#include <vine/graphics/Camera.hpp>
#include <vine/graphics/Geometry.hpp>
#include <vine/graphics/Ray.hpp>
#include <vine/graphics/RayIntersection.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/math/Point3.hpp>
#include <vine/math/Math.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

V_GRAPHICS_NS_BEGIN

namespace
{

constexpr double kDegToRad = vine::math::DEG_TO_RAD;
constexpr double kTiny = 1e-6;

/** @brief Rotates a vector around a unit axis (Rodrigues' formula). */
Vec3d rotateAround(const Vec3d& v, const Vec3d& axis, double angle)
{
    const double c = std::cos(angle);
    const double s = std::sin(angle);
    return v * c + axis.cross(v) * s + axis * (axis.dot(v) * (1.0 - c));
}

}  // namespace

OrbitCameraManipulator::OrbitCameraManipulator(Camera* camera, Scene* scene)
  : CameraManipulator(camera)
  , scene_(scene)
{
    if (camera != nullptr) {
        reference_up_ = camera->up();
    }
    syncFromCamera();
    home_center_ = center_;
    home_distance_ = distance_;
    home_yaw_ = yaw_;
    home_pitch_ = pitch_;
    home_head_ = head_;
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
    center_ = center;
    apply();
}

Vec3d OrbitCameraManipulator::orbitCenter() const
{
    return center_;
}

double OrbitCameraManipulator::orbitRadius() const
{
    return distance_;
}

void OrbitCameraManipulator::setOrbitRadius(double radius)
{
    distance_ = std::max(radius, kTiny);
    apply();
}

void OrbitCameraManipulator::pan(double screenDx, double screenDy)
{
    Camera* cam = camera_;
    if (cam == nullptr) {
        return;
    }
    const double vh = viewport_h_ > 0 ? static_cast<double>(viewport_h_) : 600.0;
    const Vec3d eye = cam->eye();
    const Vec3d forward = (center_ - eye).normalized();
    if (forward.length() < kTiny) {
        return;
    }
    const Vec3d right = forward.cross(cam->up()).normalized();
    const Vec3d up = right.cross(forward).normalized();

    const double tan_half = std::tan(cam->fieldOfView() * kDegToRad * 0.5);
    double world_per_pixel = 0.0;
    if (cam->projectionType() == Camera::ProjectionType::Orthographic) {
        world_per_pixel = 2.0 * distance_ * tan_half / vh;
    } else {
        const double depth = anchor_on_ray_
            ? std::max((anchor_ - eye).dot(forward), kTiny)
            : std::max(distance_, kTiny);
        world_per_pixel = 2.0 * depth * tan_half / vh;
    }

    const Vec3d delta = (-right * screenDx + up * screenDy) * world_per_pixel;
    center_ += delta;
    apply();
}

void OrbitCameraManipulator::zoom(double factor)
{
    if (camera_ == nullptr || factor <= 0.0) {
        return;
    }
    const double d0 = distance_;
    const double d1 = clampDistance(d0 * factor);
    if (std::abs(d1 - d0) < 1e-9) {
        return;
    }

    // Keep the anchor (a picked world point under the cursor) fixed on screen
    // while zooming: slide the orbit centre laterally so the anchor's screen
    // offset is unchanged after the distance change.
    if (zoom_to_cursor_ && anchor_on_ray_) {
        const Vec3d eye = camera_->eye();
        const Vec3d forward = (center_ - eye).normalized();
        if (forward.length() >= kTiny) {
            double pz = 0.0;
            double pz1 = 0.0;
            if (camera_->projectionType() == Camera::ProjectionType::Orthographic) {
                // In orthographic, the screen offset of a point scales with 1 / distance.
                pz = d0;
                pz1 = d1;
            } else {
                pz = std::max((anchor_ - eye).dot(forward), kTiny);
                pz1 = pz + (d1 - d0);
            }
            if (pz > kTiny) {
                const Vec3d base = eye + forward * pz;
                const Vec3d lateral = anchor_ - base;
                center_ += lateral * (1.0 - pz1 / pz);
            }
        }
    }
    distance_ = d1;
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
                               cam->target() + forward * distance, cam->up());
    syncFromCamera();
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
                               cam->target() + right * distance, cam->up());
    syncFromCamera();
}

void OrbitCameraManipulator::moveUp(double distance)
{
    Camera* cam = camera_;
    if (cam == nullptr) {
        return;
    }
    cam->setViewMatrixAsLookAt(cam->eye() + cam->up() * distance,
                               cam->target() + cam->up() * distance, cam->up());
    syncFromCamera();
}

void OrbitCameraManipulator::rotate(double deltaYaw, double deltaPitch)
{
    orbit(deltaYaw, deltaPitch);
}

void OrbitCameraManipulator::rotateHead(double deltaHead)
{
    head_ += deltaHead;
    apply();
}

bool OrbitCameraManipulator::fitToScreen()
{
    Camera* cam = camera_;
    if (cam == nullptr || scene_ == nullptr) {
        return false;
    }
    const Aabbd bounds = scene_->boundingBox();
    if (!bounds.isValid()) {
        return false;
    }
    const vine::math::Point3d centre_p = bounds.center();
    const Vec3d centre(centre_p.x, centre_p.y, centre_p.z);
    const Vec3d size = bounds.size();
    // Bounding-sphere radius around the box centre.
    const double radius = (size * 0.5).length();

    center_ = centre;
    const double fov_rad = cam->fieldOfView() * kDegToRad;
    const double aspect = std::max(cam->aspectRatio(), 1e-6);
    const double margin = 1.15;

    if (cam->projectionType() == Camera::ProjectionType::Orthographic) {
        // Frustum half-height needed to frame the sphere in both dimensions.
        const double half_w = radius * margin * aspect;
        const double needed_half_h = std::max(radius * margin, half_w / aspect);
        const double tan_half = std::tan(fov_rad * 0.5);
        distance_ = std::max(needed_half_h / tan_half, radius * 1.05);
    } else {
        const double tan_half = std::tan(fov_rad * 0.5);
        const double vertical_half = fov_rad * 0.5;
        const double horizontal_half = std::atan(tan_half * aspect);
        const double half_angle = std::min(vertical_half, horizontal_half);
        const double sin_half = std::sin(half_angle);
        distance_ = std::max(radius * margin / std::max(sin_half, 1e-6), radius * 1.05);
    }
    distance_ = clampDistance(distance_);
    apply();
    return true;
}

void OrbitCameraManipulator::home()
{
    center_ = home_center_;
    distance_ = home_distance_;
    yaw_ = home_yaw_;
    pitch_ = home_pitch_;
    head_ = home_head_;
    apply();
}

void OrbitCameraManipulator::setFixedHead(bool fixed)
{
    fixed_head_ = fixed;
    if (fixed) {
        head_ = 0.0;
    }
    apply();
}

bool OrbitCameraManipulator::isFixedHead() const
{
    return fixed_head_;
}

void OrbitCameraManipulator::setZoomToCursor(bool enabled)
{
    zoom_to_cursor_ = enabled;
}

bool OrbitCameraManipulator::zoomToCursor() const
{
    return zoom_to_cursor_;
}

void OrbitCameraManipulator::setMinDistance(double distance)
{
    min_distance_ = distance;
}

double OrbitCameraManipulator::minDistance() const
{
    return min_distance_;
}

void OrbitCameraManipulator::setScene(Scene* scene)
{
    scene_ = scene;
}

Scene* OrbitCameraManipulator::scene() const
{
    return scene_;
}

bool OrbitCameraManipulator::pickAt(double screenX, double screenY, Vec3d& outPoint) const
{
    if (scene_ == nullptr || camera_ == nullptr) {
        return false;
    }
    Vec3d origin;
    Vec3d direction;
    makeScreenRay(screenX, screenY, origin, direction);
    RayIntersectionResult result =
        RayIntersection::intersectScene(Ray(origin, direction), scene_);
    if (!result.hit) {
        return false;
    }
    outPoint = result.point;
    return true;
}

bool OrbitCameraManipulator::setCenterFromScreen(double screenX, double screenY)
{
    bool on_ray = false;
    const Vec3d anchor = resolveAnchor(screenX, screenY, on_ray);
    aimCenterAt(anchor);
    return on_ray;
}

void OrbitCameraManipulator::apply()
{
    Camera* cam = camera_;
    if (cam == nullptr) {
        return;
    }
    pitch_ = std::clamp(pitch_, -(vine::math::PI_HALF - 1e-3), vine::math::PI_HALF - 1e-3);
    if (cam->projectionType() == Camera::ProjectionType::Orthographic) {
        clampEyeOutsideBounds();
    }
    distance_ = clampDistance(distance_);

    const double cp = std::cos(pitch_);
    const double sp = std::sin(pitch_);
    const double cy = std::cos(yaw_);
    const double sy = std::sin(yaw_);
    // Unit direction from the orbit centre towards the eye.
    const Vec3d dir{ cp * sy, sp, cp * cy };
    const Vec3d eye = center_ + dir * distance_;
    const Vec3d forward = (center_ - eye).normalized();

    Vec3d up = reference_up_;
    if (!fixed_head_ && std::abs(head_) > 1e-9 && forward.length() >= kTiny) {
        up = rotateAround(up, forward, head_);
    }
    cam->setViewMatrixAsLookAt(eye, center_, up);

    if (cam->projectionType() == Camera::ProjectionType::Orthographic) {
        updateOrthoProjection();
    }
}

void OrbitCameraManipulator::onMousePress(const vine::window::MouseEvent& event)
{
    last_x_ = event.x;
    last_y_ = event.y;
    pointer_known_ = true;

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

    // Resolve the interaction anchor: the picked model point under the cursor,
    // falling back to the scene bounding-box centre or the world origin.
    anchor_ = resolveAnchor(event.x, event.y, anchor_on_ray_);

    // The resolved anchor doubles as the rotate pivot: pressing on a model
    // pins that surface point under the cursor, pressing on empty space pins
    // the scene centre. It is not applied on press (no jump); onMouseMove
    // rotates the whole camera rig about it during the drag.
}

void OrbitCameraManipulator::onMouseMove(const vine::window::MouseEvent& event)
{
    const double dx = event.x - last_x_;
    const double dy = event.y - last_y_;
    last_x_ = event.x;
    last_y_ = event.y;
    pointer_known_ = true;

    switch (drag_) {
        case CameraManipulator::DragAction::Rotate:
            if (mode_ == Mode::FirstPerson) {
                rotate(dx * drag_sensitivity_, -dy * drag_sensitivity_);
            } else {
                // Orbit feels like grabbing the model: dragging right turns the
                // model's front to the right and dragging down tilts its top
                // towards the viewer (the camera orbits the opposite way).
                //
                // The whole rig rotates rigidly about the press pivot (the
                // grabbed model point, or the scene centre on empty space). A
                // rigid rotation keeps the pivot's screen position exactly
                // fixed, so the model turns about it with constant size - no
                // drift, no fly-out, even when the model sits near an edge.
                pivotRotate(dx, dy);
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
    Camera* cam = camera_;
    if (cam == nullptr || std::abs(event.deltaY) < 1e-6) {
        return;
    }
    // Anchor at the current pointer position (falls back to the view centre).
    const double sx = pointer_known_ ? last_x_ : viewport_w_ * 0.5;
    const double sy = pointer_known_ ? last_y_ : viewport_h_ * 0.5;
    anchor_ = resolveAnchor(sx, sy, anchor_on_ray_);

    // The wheel performs a dolly (translate the camera along a direction)
    // rather than shrinking a radius about a fixed point, so in perspective
    // the camera can move through the scene instead of stopping at a target.
    //
    // While the picked anchor lies in front of the camera the dolly dives
    // along the ray eye -> anchor, which keeps that model point pinned under
    // the cursor (anchored zoom). Once the eye has passed through the anchor
    // (it falls behind the near plane) the dive direction is released and the
    // camera keeps moving straight along the view axis, so it travels through
    // the model instead of stopping at its surface.
    const Vec3d eye = cam->eye();
    const Vec3d forward = (cam->target() - eye).normalized();
    if (forward.length() < kTiny) {
        return;
    }
    Vec3d dir = forward;
    if (anchor_on_ray_) {
        const Vec3d to_anchor = anchor_ - eye;
        const double d = to_anchor.length();
        // Keep the dive aimed at the anchor while it is clearly in front (it
        // then stays pinned under the cursor). Once the eye is essentially at
        // the surface (the anchor is within a few near planes), release the
        // anchor and keep flying straight so the camera passes through the
        // model instead of hovering in front of it.
        const double release_dist = cam->nearPlane() * 3.0;
        if (d > release_dist && to_anchor.dot(forward) > 0.0) {
            dir = to_anchor / d;
        }
    }

    // Move by a fraction of the current eye-to-centre distance per notch so
    // the dolly speed tracks the zoom level.
    const double ratio = 1.0 - 1.0 / wheel_factor_;
    const double step = (event.deltaY > 0.0 ? distance_ * ratio : -distance_ * ratio);
    center_ += dir * step;
    apply();
}

void OrbitCameraManipulator::onKeyDown(const vine::window::KeyEvent& event)
{
    if (event.code == vine::window::KeyCode::Home) {
        if (!fitToScreen()) {
            home();
        }
        return;
    }
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

void OrbitCameraManipulator::onResize(const vine::window::ResizeEvent& event)
{
    if (event.width <= 0 || event.height <= 0) {
        return;
    }
    viewport_w_ = event.width;
    viewport_h_ = event.height;
    Camera* cam = camera_;
    if (cam == nullptr) {
        return;
    }
    const double aspect = static_cast<double>(event.width) / static_cast<double>(event.height);
    if (cam->projectionType() == Camera::ProjectionType::Perspective) {
        cam->setProjectionMatrixAsPerspective(cam->fieldOfView(), aspect,
                                              cam->nearPlane(), cam->farPlane());
    } else {
        updateOrthoProjection();
    }
}

void OrbitCameraManipulator::makeScreenRay(double screenX, double screenY,
                                           Vec3d& origin, Vec3d& direction) const
{
    Camera* cam = camera_;
    origin = cam != nullptr ? cam->eye() : Vec3d();
    direction = Vec3d(0.0, 0.0, -1.0);
    if (cam == nullptr) {
        return;
    }
    const double w = viewport_w_ > 0 ? static_cast<double>(viewport_w_) : 1.0;
    const double h = viewport_h_ > 0 ? static_cast<double>(viewport_h_) : 1.0;
    const Ray ray = cam->screenToWorldRay(Vec2d(screenX / w, screenY / h));
    origin = ray.origin;
    direction = ray.direction;
}

void OrbitCameraManipulator::pivotRotate(double screenDx, double screenDy)
{
    Camera* cam = camera_;
    if (cam == nullptr) {
        return;
    }
    const double ayaw = -screenDx * drag_sensitivity_;
    const double apitch = -screenDy * drag_sensitivity_;
    if (std::abs(ayaw) < 1e-12 && std::abs(apitch) < 1e-12) {
        return;
    }

    const Vec3d eye = cam->eye();
    const Vec3d target = cam->target();
    const Vec3d pivot = anchor_;
    const Vec3d fwd0 = (target - eye).normalized();
    if (fwd0.length() < kTiny) {
        return;
    }
    const Vec3d right0 = fwd0.cross(cam->up()).normalized();
    const Vec3d off = eye - pivot;
    if (right0.length() < kTiny || off.length() < kTiny) {
        return;
    }
    const Vec3d world_up = reference_up_.normalized();

    // Rotate the whole camera rig about the pivot: yaw about the world-up
    // axis through the pivot, then pitch about the resulting screen-right
    // axis. A rigid rotation keeps the pivot's screen position exactly fixed,
    // so the model turns about it with constant apparent size (no drift / no
    // fly-out), regardless of where the pivot sits in the viewport.
    const Vec3d eye1 = pivot + rotateAround(off, world_up, ayaw);
    const Vec3d fwd1 = rotateAround(fwd0, world_up, ayaw);
    const Vec3d right1 = rotateAround(right0, world_up, ayaw);
    if (right1.length() < kTiny) {
        return;
    }
    const Vec3d eye2 = pivot + rotateAround(eye1 - pivot, right1, apitch);
    const Vec3d fwd2 = rotateAround(fwd1, right1, apitch).normalized();
    if (fwd2.length() < kTiny) {
        return;
    }

    // Rebuild an un-rolled up basis so orbiting never rolls the view.
    Vec3d up = cam->up();
    const Vec3d right = fwd2.cross(world_up).normalized();
    if (right.length() >= kTiny) {
        up = right.cross(fwd2).normalized();
    } else {
        // Looking straight up/down: keep the up propagated by the rig rotation.
        const Vec3d rolled_up = rotateAround(cam->up(), right1, apitch);
        up = right1.cross(rolled_up).normalized();
    }

    // Preserve the eye-to-target distance so the frame scale is unchanged.
    const double d_view = (target - eye).length();
    const Vec3d target2 = eye2 + fwd2 * d_view;
    cam->setViewMatrixAsLookAt(eye2, target2, up);
    syncFromCamera();
}

Vec3d OrbitCameraManipulator::resolveAnchor(double screenX, double screenY,
                                            bool& onRay) const
{
    onRay = false;
    Vec3d hit;
    if (pickAt(screenX, screenY, hit)) {
        onRay = true;
        return hit;
    }
    if (scene_ != nullptr) {
        const Aabbd bounds = scene_->boundingBox();
        if (bounds.isValid()) {
            const vine::math::Point3d centre_p = bounds.center();
            return Vec3d(centre_p.x, centre_p.y, centre_p.z);
        }
    }
    return Vec3d(0.0, 0.0, 0.0);
}

void OrbitCameraManipulator::aimCenterAt(const Vec3d& point)
{
    Camera* cam = camera_;
    if (cam == nullptr) {
        return;
    }
    const Vec3d eye = cam->eye();
    const Vec3d offset = eye - point;
    const double d = offset.length();
    if (d < 1e-9) {
        return;
    }
    center_ = point;
    distance_ = clampDistance(d);
    const Vec3d dir = offset / d;
    pitch_ = std::asin(std::clamp(dir.y, -1.0, 1.0));
    yaw_ = std::atan2(dir.x, dir.z);
    head_ = 0.0;
    apply();
}

void OrbitCameraManipulator::syncFromCamera()
{
    Camera* cam = camera_;
    if (cam == nullptr) {
        return;
    }
    const Vec3d eye = cam->eye();
    center_ = cam->target();
    const Vec3d offset = eye - center_;
    const double d = offset.length();
    if (d > 1e-9) {
        distance_ = d;
        const Vec3d dir = offset / d;
        pitch_ = std::asin(std::clamp(dir.y, -1.0, 1.0));
        yaw_ = std::atan2(dir.x, dir.z);
    }
}

void OrbitCameraManipulator::clampEyeOutsideBounds()
{
    Camera* cam = camera_;
    if (cam == nullptr || scene_ == nullptr) {
        return;
    }
    const Aabbd bounds = scene_->boundingBox();
    if (!bounds.isValid()) {
        return;
    }
    const Vec3d dir{ std::cos(pitch_) * std::sin(yaw_),
                     std::sin(pitch_),
                     std::cos(pitch_) * std::cos(yaw_) };

    // Slab ray from the orbit centre along the eye direction: the eye must
    // stay outside the box, so it may not sit between the entry and exit.
    double t_min = -std::numeric_limits<double>::max();
    double t_max = std::numeric_limits<double>::max();
    for (int axis = 0; axis < 3; ++axis) {
        const double d = axis == 0 ? dir.x : (axis == 1 ? dir.y : dir.z);
        const double o = axis == 0 ? center_.x : (axis == 1 ? center_.y : center_.z);
        const double lo = axis == 0 ? bounds.min().x
                          : (axis == 1 ? bounds.min().y : bounds.min().z);
        const double hi = axis == 0 ? bounds.max().x
                          : (axis == 1 ? bounds.max().y : bounds.max().z);
        if (std::abs(d) < 1e-12) {
            if (o < lo || o > hi) {
                return;  // Parallel to the slab and outside: the ray misses the box.
            }
        } else {
            double t1 = (lo - o) / d;
            double t2 = (hi - o) / d;
            if (t1 > t2) {
                std::swap(t1, t2);
            }
            t_min = std::max(t_min, t1);
            t_max = std::min(t_max, t2);
        }
    }
    // Clamp only when the current distance would place the eye inside the box.
    if (distance_ >= t_min && distance_ <= t_max
        && t_max < std::numeric_limits<double>::max()) {
        distance_ = t_max * 1.02 + 1e-6;
    }
}

double OrbitCameraManipulator::clampDistance(double distance) const
{
    double low = min_distance_;
    if (low <= 0.0) {
        low = camera_ != nullptr ? std::max(camera_->nearPlane() * 1.2, 1e-4) : 1e-3;
    }
    return std::max(distance, low);
}

void OrbitCameraManipulator::updateOrthoProjection()
{
    Camera* cam = camera_;
    if (cam == nullptr || cam->projectionType() != Camera::ProjectionType::Orthographic) {
        return;
    }
    const double tan_half = std::tan(cam->fieldOfView() * kDegToRad * 0.5);
    const double half_h = distance_ * tan_half;
    const double half_w = half_h * std::max(cam->aspectRatio(), 1e-6);
    cam->setProjectionMatrixAsOrtho(-half_w, half_w, -half_h, half_h,
                                    cam->nearPlane(), cam->farPlane());
}

Vec3d OrbitCameraManipulator::referenceUp() const
{
    return reference_up_;
}

V_GRAPHICS_NS_END
