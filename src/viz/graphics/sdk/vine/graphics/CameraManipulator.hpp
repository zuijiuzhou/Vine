#pragma once
#include "graphics_global.hpp"

#include <vine/RefCounted.hpp>
#include <vine/math/Vector3.hpp>
#include <vine/raw_ptr.hpp>
#include <vine/window/InputEvent.hpp>
#include <vine/window/KeyCode.hpp>
#include <vine/window/MouseButton.hpp>

V_GRAPHICS_NS_BEGIN

using vine::math::Vec3d;

class Camera;

/**
 * @brief Abstract camera manipulator handling user interaction to update a camera.
 *
 * CameraManipulator defines the interface shared by all camera manipulators:
 * interaction mode, window input callbacks, and apply(). Concrete subclasses
 * (e.g. OrbitCameraManipulator) implement a specific camera control scheme and
 * are driven either directly through their motion methods or through the
 * window input callbacks (onMousePress/onMouseMove/onScroll/onKeyDown...).
 * A RenderEngine typically forwards vine::window events to these callbacks.
 *
 * CameraManipulator is reference counted (RefCounted<CameraManipulator>): a
 * RenderEngine that drives it keeps an intrusive_ptr so the manipulator stays
 * alive as long as it is attached. The camera must outlive the manipulator.
 */
class V_GRAPHICS_API CameraManipulator : public RefCounted<CameraManipulator> {
  public:
    enum class Mode {
        Orbit,        ///< Orbit around a target point.
        Pan,          ///< Pan (translate) the view.
        Zoom,         ///< Zoom in/out.
        FirstPerson,  ///< First-person navigation.
    };

    /** @brief Mouse button that drives the current drag interaction. */
    enum class DragAction {
        None,    ///< No drag in progress.
        Rotate,  ///< Left button: orbit (rotate).
        Pan,     ///< Middle/right button: pan (translate).
    };

  public:
    /** @brief Destroys the manipulator. */
    virtual ~CameraManipulator();

    /** @brief Gets the camera being manipulated. */
    raw_ptr<Camera> camera() const;

    /** @brief Gets the current interaction mode. */
    Mode mode() const;

    /** @brief Sets the current interaction mode. */
    void setMode(Mode m);

    /** @brief Commits pending changes to the bound camera. */
    virtual void apply() = 0;

  public:
    // ---- Window input callbacks (typically driven by a RenderEngine) ----

    /** @brief Handles a mouse button press.
     *
     * Starts a drag interaction (rotate for the left button, pan for the
     * middle/right button).
     *
     * @param event Mouse event describing the pressed button and position.
     */
    virtual void onMousePress(const vine::window::MouseEvent& event) = 0;

    /** @brief Handles mouse motion, applying the active drag interaction.
     *
     * @param event Mouse event describing the new position.
     */
    virtual void onMouseMove(const vine::window::MouseEvent& event) = 0;

    /** @brief Handles a mouse button release, ending any drag interaction.
     *
     * @param event Mouse event describing the released button.
     */
    virtual void onMouseRelease(const vine::window::MouseEvent& event) = 0;

    /** @brief Handles mouse wheel scroll.
     *
     * @param event Scroll event; positive deltaY zooms in.
     */
    virtual void onScroll(const vine::window::ScrollEvent& event) = 0;

    /** @brief Handles a key press.
     *
     * @param event Key event describing the pressed key.
     */
    virtual void onKeyDown(const vine::window::KeyEvent& event) = 0;

    /** @brief Handles a key release.
     *
     * @param event Key event describing the released key.
     */
    virtual void onKeyUp(const vine::window::KeyEvent& event) = 0;

    /** @brief Handles a viewport resize.
     *
     * The manipulator should refresh anything derived from the viewport size,
     * typically the camera projection aspect ratio.
     *
     * @param event Resize event carrying the new surface size in pixels.
     */
    virtual void onResize(const vine::window::ResizeEvent& event) = 0;

  protected:
    /** @brief Constructs a manipulator bound to a camera.
     *
     * @param camera Camera to manipulate. Must outlive the manipulator.
     */
    explicit CameraManipulator(raw_ptr<Camera> camera);

    // ---- State shared with derived classes ----

    raw_ptr<Camera> camera_ = nullptr;
    Mode mode_ = Mode::Orbit;
};

V_GRAPHICS_NS_END
