#pragma once

#include "CameraManipulator.hpp"

V_GRAPHICS_NS_BEGIN

/**
 * @brief Orbit-style camera manipulator.
 *
 * Implements the classic orbit/pan/zoom camera controls around a target
 * point: left-drag orbits (yaw/pitch), middle/right-drag pans, and the wheel
 * zooms. In FirstPerson mode it also supports WASD movement and arrow-key
 * rotation.
 */
class V_GRAPHICS_API OrbitCameraManipulator : public CameraManipulator {
  public:
    /** @brief Constructs an orbit manipulator bound to a camera.
     *
     * The orbit center and radius are initialized from the camera's current
     * view: center = camera target, radius = distance from eye to target.
     *
     * @param camera Camera to manipulate. Must outlive the manipulator.
     */
    explicit OrbitCameraManipulator(Camera* camera);
    ~OrbitCameraManipulator() override;

  public:
    /** @brief Orbits the camera around the target (yaw/pitch rotation).
     *
     * @param deltaYaw   Yaw angle change in radians.
     * @param deltaPitch Pitch angle change in radians.
     */
    void orbit(double deltaYaw, double deltaPitch);

    /** @brief Sets the orbit center (target point). */
    void setOrbitCenter(const Vec3d& center);

    /** @brief Gets the orbit radius. */
    double orbitRadius() const;

    /** @brief Sets the orbit radius.
     *
     * @param radius Distance from eye to orbit center.
     */
    void setOrbitRadius(double radius);

    /** @brief Pans the camera based on screen-space deltas.
     *
     * @param screenDx Horizontal screen delta in pixels.
     * @param screenDy Vertical screen delta in pixels.
     */
    void pan(double screenDx, double screenDy);

    /** @brief Zooms by a multiplicative factor (e.g. wheel delta).
     *
     * @param factor Zoom factor; >1 zooms in, <1 zooms out.
     */
    void zoom(double factor);

    /** @brief Moves the camera forward/backward in first-person mode.
     *
     * @param distance Positive moves forward.
     */
    void moveForward(double distance);

    /** @brief Moves the camera right/left in first-person mode.
     *
     * @param distance Positive moves right.
     */
    void moveRight(double distance);

    /** @brief Moves the camera up/down in first-person mode.
     *
     * @param distance Positive moves up.
     */
    void moveUp(double distance);

    /** @brief Rotates the camera yaw/pitch in first-person mode.
     *
     * @param deltaYaw   Yaw change in radians.
     * @param deltaPitch Pitch change in radians.
     */
    void rotate(double deltaYaw, double deltaPitch);

    /** @brief Commits pending changes to the bound camera. */
    void apply() override;

  public:
    // ---- Window input callbacks ----

    /** @brief Handles a mouse button press (see CameraManipulator). */
    void onMousePress(const vine::window::MouseEvent& event) override;

    /** @brief Handles mouse motion, applying the active drag interaction. */
    void onMouseMove(const vine::window::MouseEvent& event) override;

    /** @brief Handles a mouse button release, ending any drag interaction. */
    void onMouseRelease(const vine::window::MouseEvent& event) override;

    /** @brief Handles mouse wheel scroll, zooming the camera. */
    void onScroll(const vine::window::ScrollEvent& event) override;

    /** @brief Handles a key press (first-person movement keys). */
    void onKeyDown(const vine::window::KeyEvent& event) override;

    /** @brief Handles a key release. */
    void onKeyUp(const vine::window::KeyEvent& event) override;

    /** @brief Updates the camera projection aspect on a viewport resize. */
    void onResize(const vine::window::ResizeEvent& event) override;

  private:
    // ---- Orbit / drag state ----

    Vec3d orbit_center_{ 0.0, 0.0, 0.0 };
    double orbit_radius_ = 10.0;
    double yaw_ = 0.0;
    double pitch_ = 0.0;

    CameraManipulator::DragAction drag_ = CameraManipulator::DragAction::None;
    double last_x_ = 0.0;
    double last_y_ = 0.0;

    // Sensitivity of drag-based interactions (radians / pixels).
    double drag_sensitivity_ = 0.01;
    // First-person movement step per key event (world units).
    double move_step_ = 0.5;
    // First-person yaw/pitch rotation per key event (radians).
    double rotate_step_ = 0.05;
};

V_GRAPHICS_NS_END
