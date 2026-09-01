#pragma once
#include "graphics_global.hpp"

#include <vine/math/Vector3.hpp>

V_GRAPHICS_NS_BEGIN

using vine::math::Vec3d;

class Camera;

/**
 * @brief Camera manipulator handling user interaction to update camera parameters.
 *
 * Supports multiple interaction modes: orbit (rotate around target), pan,
 * zoom, and first-person navigation. The manipulator updates the associated
 * Camera object; call apply() to commit changes.
 */
class V_GRAPHICS_API CameraManipulator {
  public:
    enum class Mode {
        Orbit,        ///< Orbit around a target point.
        Pan,          ///< Pan (translate) the view.
        Zoom,         ///< Zoom in/out.
        FirstPerson,  ///< First-person navigation.
    };

  public:
    /** @brief Constructs a manipulator bound to a camera.
     *
     * @param camera Camera to manipulate. Must outlive the manipulator.
     */
    explicit CameraManipulator(Camera* camera);

    /** @brief Gets the current interaction mode. */
    Mode mode() const;

    /** @brief Sets the current interaction mode. */
    void setMode(Mode m);

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
    void apply();

  private:
    struct Data;
    Data* const d;
};

V_GRAPHICS_NS_END
