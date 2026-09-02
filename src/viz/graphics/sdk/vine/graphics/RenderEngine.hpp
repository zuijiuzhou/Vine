#pragma once
#include "graphics_global.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/window/window_global.hpp>

V_WINDOW_NS_BEGIN
struct KeyEvent;
struct MouseEvent;
struct ResizeEvent;
struct ScrollEvent;
V_WINDOW_NS_END

V_GRAPHICS_NS_BEGIN

class Camera;
class Scene;
class RenderPass;
class RenderBackend;
class CameraManipulator;

/**
 * @brief High-level render engine managing the frame loop and render state.
 *
 * RenderEngine owns the default scene, camera, and main render pass, and
 * drives one frame per call to frame(): begin, execute the main pass, end,
 * and swap buffers. It is platform-independent and delegates actual drawing
 * to a RenderBackend supplied by the caller via setBackend().
 *
 * The engine may be given a host native window (setWindowHandle) so the
 * backend can attach its render surface to it; input and resize events are
 * pushed by the host via pushEvent(). The engine stays platform-independent
 * and owns none of the window objects.
 */
class V_GRAPHICS_API RenderEngine : public Object, public RefCounted<RenderEngine> {
    V_OBJECT_META_DECL;

  public:
    /** @brief Constructs an engine with no backend attached yet.
     *
     * A default scene, camera and main render pass are created automatically.
     * Call setBackend() before initialize().
     */
    RenderEngine();
    ~RenderEngine();

  public:
    /** @brief Gets the bound render backend, or nullptr when unset. */
    RenderBackend* backend() const;

    /** @brief Sets the render backend used for drawing.
     *
     * The engine keeps a reference to the backend for as long as it is set,
     * so the backend stays alive at least until the engine is destroyed or
     * a different backend (or nullptr) is set. Call before initialize().
     *
     * @param backend Backend used for drawing, or null to clear.
     */
    void setBackend(intrusive_ptr<RenderBackend> backend);

    /** @brief Initializes the backend.
     *
     * @return true when the backend initialized successfully.
     */
    bool initialize();

    /** @brief Releases backend resources. */
    void shutdown();

    /** @brief Renders one frame (begin, main pass, end, swap). */
    void frame();

    /** @brief Sets the scene to render. */
    void setScene(Scene* scene);

    /** @brief Gets the scene to render. */
    Scene* scene() const;

    /** @brief Sets the camera used by the main pass. */
    void setCamera(Camera* camera);

    /** @brief Gets the camera used by the main pass. */
    Camera* camera() const;

    /** @brief Sets the main render pass. */
    void setMainPass(RenderPass* pass);

    /** @brief Gets the main render pass. */
    RenderPass* mainPass() const;

    /** @brief Sets the camera manipulator driving the engine camera.
     *
     * The manipulator must be bound to the engine's camera (or the camera it
     * will use). Window input events (from the bound WindowContext) are
     * forwarded to it.
     *
     * @param manipulator Manipulator, or nullptr to clear.
     */
    void setCameraManipulator(CameraManipulator* manipulator);

    /** @brief Gets the camera manipulator, or nullptr when unset. */
    CameraManipulator* cameraManipulator() const;

    /** @brief Pushes a mouse event to the camera manipulator.
     *
     * A move event carries button == MouseButton::None; a press/release sets
     * the pressed flag and the button. The host forwards translated toolkit
     * events here directly instead of binding a window context.
     *
     * @param event Mouse event to forward.
     */
    void pushEvent(const vine::window::MouseEvent& event);

    /** @brief Pushes a scroll (wheel) event to the camera manipulator.
     *
     * @param event Scroll event to forward.
     */
    void pushEvent(const vine::window::ScrollEvent& event);

    /** @brief Pushes a keyboard event to the camera manipulator.
     *
     * @param event Key event to forward.
     */
    void pushEvent(const vine::window::KeyEvent& event);

    /** @brief Pushes a surface resize.
     *
     * Refreshes the camera manipulator (camera aspect ratio) and rebuilds the
     * backend swapchain. Only the swapchain rebuild needs an initialized
     * backend; the manipulator update is always applied.
     *
     * @param event Resize event carrying the new surface size in pixels.
     */
    void pushEvent(const vine::window::ResizeEvent& event);

    /** @brief Supplies the native window the backend attaches to.
     *
     * Stored until initialize(); the backend reads the handle from it to
     * attach its render surface (e.g. a Qt QWindow). The handle value is
     * captured at call time, so the host must call it again before a
     * re-initialize whenever the native window was recreated. Pass nullptr
     * to clear.
     *
     * @param native_handle Native window handle (HWND on Windows), or nullptr.
     */
    void setWindowHandle(void* native_handle);

  private:
    intrusive_ptr<RenderBackend> backend_;
    intrusive_ptr<Scene> scene_;
    intrusive_ptr<Camera> camera_;
    intrusive_ptr<RenderPass> main_pass_;
    CameraManipulator* camera_manipulator_ = nullptr;
    void* native_handle_ = nullptr;
    bool initialized_ = false;
};

V_GRAPHICS_NS_END
