#pragma once
#include "graphics_global.hpp"

#include <map>
#include <string>
#include <vector>

#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/intrusive_ptr.hpp>
#include <vine/raw_ptr.hpp>
#include <vine/window/window_global.hpp>

#include "FrameContext.hpp"
#include "ShaderPreset.hpp"

V_WINDOW_NS_BEGIN
struct KeyEvent;
struct MouseEvent;
struct ResizeEvent;
struct ScrollEvent;
V_WINDOW_NS_END

V_GRAPHICS_NS_BEGIN

class Camera;
class Light;
class Scene;
class RenderPass;
class RenderTarget;
class RenderBackend;
class CameraManipulator;
class Overlay;

/**
 * @brief High-level render engine managing the frame loop and render state.
 *
 * RenderEngine does not impose a fixed pipeline: nothing is rendered unless
 * the caller registers scene passes with addPass(). Each frame, frame() runs
 * the registered passes in ascending order, then the overlays in ascending
 * zOrder, then ends and swaps buffers. The engine carries two optional,
 * caller-set pieces of view state: a scene (scene()) used as the default
 * content for passes registered without an explicit one, and a master camera
 * (masterCamera()) that a bound camera manipulator drives (when unset the
 * manipulator has nothing to control and is inert). A pass that presents the
 * primary view to the window is simply a registered pass whose camera is the
 * master camera and whose render target is null. The engine is
 * platform-independent and delegates actual drawing to a RenderBackend
 * supplied by the caller via setBackend().
 *
 * The engine may be given a host native window (setWindowHandle) so the
 * backend can attach its render surface to it; input and resize events are
 * pushed by the host via pushEvent(). The engine stays platform-independent
 * and owns none of the window objects.
 */
class V_GRAPHICS_API RenderEngine : public Object, public RefCounted<RenderEngine> {
    V_OBJECT_META_DECL;

  public:
    /** @brief Constructs an empty engine with no backend attached yet.
     *
     * The engine starts with no scene, no master camera and no registered
     * pass; the caller configures the pipeline explicitly (see addPass()) or
     * through a RenderPipelineBuilder. Call setBackend() before initialize().
     */
    RenderEngine();
    ~RenderEngine();

  public:
    /** @brief Gets the bound render backend, or nullptr when unset. */
    raw_ptr<RenderBackend> backend() const;

    /** @brief Sets the render backend used for drawing.
     *
     * The engine keeps a reference to the backend for as long as it is set,
     * so the backend stays alive at least until the engine is destroyed or
     * a different backend (or nullptr) is set. Call before initialize().
     * Setting the same backend instance again is a no-op.
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

    /** @brief Renders one frame (begin, ordered passes, overlays, end, swap).
     *
     * Nothing is drawn when no scene pass is registered. Passes registered
     * with a null content binding render scene() when it is set.
     *
     * @param dt Seconds elapsed since the previous frame (drives animated
     *           overlays); the default 0 is fine for static overlays.
     */
    void frame(double dt = 0.0);

    /** @brief Gets this frame's shared context (elapsed time, surface size).
     *
     * Updated by frame() and the resize events pushed via pushEvent(); it is
     * the per-frame data shared across the pass pipeline (to be extended
     * later with previous-frame view-projection matrices and active lights).
     *
     * @return The current frame context.
     */
    const FrameContext& frameContext() const;

    /** @brief Registers an overlay drawn on top of every scene pass.
     *
     * The engine keeps a reference and draws overlays in ascending zOrder
     * every frame, after all registered scene passes. Registering the same
     * overlay instance twice is ignored.
     *
     * @param overlay Overlay to add.
     */
    void addOverlay(intrusive_ptr<Overlay> overlay);

    /** @brief Removes a previously added overlay.
     *
     * An overlay draws through its own pass, and the pass's camera identifies
     * the overlay's backend view slot (the backend keys non-main views by
     * pass->camera()). If that pass is no longer used by the engine after the
     * removal (it is not registered via addPass() and not shared with another
     * overlay), the backend is asked to release the overlay's view and the
     * pass's off-screen render target; otherwise the resources are kept for
     * the surviving user.
     *
     * @param overlay Overlay to remove (by pointer).
     */
    void removeOverlay(raw_ptr<Overlay> overlay);

    /** @brief Removes all registered overlays.
     *
     * Releases the backend view / render-target resources of every overlay
     * pass that the engine itself (registered scene passes) does not keep
     * drawing; passes shared between overlays are released once.
     */
    void clearOverlays();

    /** @brief Sets the default content scene.
     *
     * Passes registered without an explicit content (addPass(pass, order))
     * render this scene. It is optional: when unset, such passes have no
     * content and draw nothing. Setting the same scene again is a no-op.
     *
     * @param scene Default content scene, or null to clear.
     */
    void setScene(intrusive_ptr<Scene> scene);

    /** @brief Gets the default content scene, or nullptr when unset. */
    raw_ptr<Scene> scene() const;

    /** @brief Sets the master camera (the interactive primary view camera).
     *
     * The master camera is the view a bound camera manipulator drives (see
     * setCameraManipulator()); when it is not set the manipulator is inert.
     * It is optional: passes carry their own cameras, and the window-present
     * pass is simply a registered pass whose camera is the master camera with
     * a null render target. Setting the same camera again is a no-op.
     *
     * @param camera Master camera, or null to clear.
     */
    void setMasterCamera(intrusive_ptr<Camera> camera);

    /** @brief Gets the master camera, or nullptr when unset. */
    raw_ptr<Camera> masterCamera() const;

    /** @brief Sets the shading-model preset for scene geometry.
     *
     * Forwarded to the backend before initialize(). Presets without a backend
     * implementation (Pbr / ShadowedPhong) fall back to StandardPhong until
     * their slice lands. The default is StandardPhong.
     *
     * @param preset Shading-model preset.
     */
    void setShaderPreset(ShaderPreset preset);

    /** @brief Gets the shading-model preset. */
    ShaderPreset shaderPreset() const;

    /** @brief Registers a scene render pass executed every frame.
     *
     * Passes run in ascending @p order each frame (equal orders keep
     * insertion order, stable), followed by the overlays:
     *
     *   - negative orders run first (e.g. a shadow-map pass driven by a light
     *     camera, or a depth / g-buffer pre-pass);
     *   - the pass presenting the primary view to the window (master camera,
     *     null render target) conventionally sits at order 0;
     *   - positive orders run after it (e.g. post-processing / compositing).
     *
     * By default the pass draws the default content scene (setScene()) with
     * its own camera, render target and clear state (configured on the pass
     * before adding it); bind a different scene with the addPass(pass,
     * content, order) overload. Nothing is auto-registered: without at least
     * one pass the engine draws nothing but overlays. Registering the same
     * pass instance twice is ignored.
     *
     * @param pass  Pass to add (the engine keeps a reference).
     * @param order Execution order (ascending; any integer allowed).
     */
    void addPass(intrusive_ptr<RenderPass> pass, int order);

    /** @brief Registers a scene render pass bound to explicit content.
     *
     * Like addPass(pass, order), but the pass renders @p content instead of
     * the default content scene. The content association is stored by the
     * engine, not on the pass object, so a RenderPass stays a reusable stage.
     * Registering the same pass instance twice is ignored (use
     * bindPassContent() to change its content).
     *
     * @param pass    Pass to add (the engine keeps a reference).
     * @param content Scene the pass renders (the engine keeps a reference);
     *                null falls back to the default content scene.
     * @param order   Execution order (ascending; any integer allowed).
     */
    void addPass(intrusive_ptr<RenderPass> pass, intrusive_ptr<Scene> content, int order);

    /** @brief Removes a previously added scene pass.
     *
     * The pass is dropped from the ordered pass list. Its backend resources
     * (the view keyed by the pass's camera, plus any off-screen render target)
     * are released only when no registered overlay still draws the same pass
     * - a pass may be both an engine scene pass and an overlay pass, and
     * removing one use must not tear down the other. An overlay-owned pass is
     * released when that overlay is removed.
     *
     * @param pass Pass to remove (by pointer).
     */
    void removePass(raw_ptr<RenderPass> pass);

    /** @brief Removes all registered scene passes.
     *
     * Passes that a registered overlay still draws are left for the overlay
     * removal path; the others get their backend resources released.
     */
    void clearPasses();

    /** @brief Gets the number of registered scene passes.
     *
     * @return Number of passes added via addPass().
     */
    std::size_t passCount() const;

    /** @brief Rebinds which scene a registered pass renders.
     *
     * The binding is managed by the engine; a pass with no bound content
     * renders the default content scene (scene()), so setScene() updates it.
     *
     * @param pass    Pass registered via addPass() (by pointer).
     * @param content New content scene, or null to fall back to the default
     *                content scene.
     */
    void bindPassContent(raw_ptr<RenderPass> pass, intrusive_ptr<Scene> content);

    /** @brief Gets the effective content scene a registered pass renders.
     *
     * @param pass Pass registered via addPass() (by pointer).
     * @return The pass's bound content when set, otherwise the default content
     *         scene (possibly nullptr when none is set); null when @p pass is
     *         not registered.
     */
    raw_ptr<Scene> contentOf(raw_ptr<RenderPass> pass) const;

    /** @brief Publishes a render target under a named output slot.
     *
     * The engine keeps a reference so the target stays alive while published.
     * Passes that declare an output name (RenderPass::setOutputName) publish
     * automatically after they run; this lets application code publish extra
     * producers (e.g. CPU-generated textures) under the same mechanism.
     *
     * @param name   Slot name consumers resolve against.
     * @param target Target to publish (the engine keeps a reference).
     */
    void publish(const String& name, intrusive_ptr<RenderTarget> target);

    /** @brief Looks up a published render target by slot name.
     *
     * @param name Slot name to look up.
     * @return The published target, or nullptr when nothing is published
     *         under @p name.
     */
    raw_ptr<RenderTarget> resolve(const String& name) const;

    /** @brief Removes a published output slot.
     *
     * @param name Slot name to remove (no-op when not published).
     */
    void unpublish(const String& name);

    /** @brief Sets the camera manipulator driving the master camera.
     *
     * The manipulator must be bound to the master camera (masterCamera(), or
     * the camera it will use). Window input events (from the bound
     * WindowContext) are forwarded to it. When no master camera is set the
     * manipulator has nothing to drive and input is not applied to any view.
     * The engine keeps the given reference for as long as the manipulator is
     * set, so it stays alive until a different manipulator (or nullptr) is
     * set or the engine is destroyed. Setting the same manipulator again is a
     * no-op.
     *
     * @param manipulator Manipulator handle, or null to clear.
     */
    void setCameraManipulator(intrusive_ptr<CameraManipulator> manipulator);

    /** @brief Gets the camera manipulator, or nullptr when unset. */
    raw_ptr<CameraManipulator> cameraManipulator() const;

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
    /** @brief Executes a scene pass against the given content scene. */
    void drawScenePass(raw_ptr<RenderPass> pass, raw_ptr<Scene> content);

    /** @brief Whether the engine's scene-pass pipeline draws @p pass.
     *
     * true when @p pass is registered via addPass(). Used by the clear-all
     * removal paths (clearOverlays / clearPasses) to tell engine-owned passes
     * apart from overlay-owned ones.
     *
     * @param pass Pass to test, or nullptr (returns false).
     * @return true when the engine pass pipeline draws @p pass.
     */
    bool isEnginePass(raw_ptr<RenderPass> pass) const;

    /** @brief Whether any registered overlay (other than @p except) draws @p pass.
     *
     * @param pass   Pass to test, or nullptr (returns false).
     * @param except Overlay whose own use is ignored (typically the overlay
     *               being removed), or nullptr to count every overlay.
     * @return true when an overlay other than @p except uses @p pass.
     */
    bool passUsedByOverlay(raw_ptr<RenderPass> pass, raw_ptr<Overlay> except) const;

    /** @brief Whether @p pass is still drawn after @p except is removed.
     *
     * @p pass counts as still drawn when the engine's scene-pass pipeline uses
     * it (isEnginePass) OR any overlay other than @p except uses it
     * (passUsedByOverlay). Callers removing @p except (an overlay) may release
     * @p pass's backend resources only when this returns false.
     *
     * @param pass   Pass to test, or nullptr (returns false).
     * @param except Overlay being removed, or nullptr.
     * @return true when something other than @p except still draws @p pass.
     */
    bool passStillUsedElsewhere(raw_ptr<RenderPass> pass, raw_ptr<Overlay> except) const;

    /** @brief Resolves a pass's declared inputs from the named-output registry.
     *
     * Called just before executing a pass: for every name in
     * pass->inputNames() the matching published target is handed to
     * pass->resolveInputTextures() (missing names resolve to nullptr).
     *
     * @param pass Pass whose inputs to resolve.
     */
    void resolvePassInputs(raw_ptr<RenderPass> pass);

    /** @brief Publishes a pass's output target under its output name.
     *
     * Called just after executing a pass. When the pass declares a non-empty
     * output name (RenderPass::setOutputName) and renders into a non-null
     * RenderTarget, that target is registered for later consumers.
     *
     * @param pass Pass whose output to publish.
     */
    void publishPassOutput(raw_ptr<RenderPass> pass);

    /** @brief A scene pass with its optional bound content and execution order.
     *
     * content == nullptr means the pass renders the default content scene
     * (scene(), possibly nullptr); the engine resolves the effective content
     * at draw time so setScene() stays a single point of change for passes
     * without an explicit binding.
     */
    struct PassSlot {
        intrusive_ptr<RenderPass> pass;
        intrusive_ptr<Scene>      content;
        int                       order = 0;
    };

    // ---- Fields ----

    intrusive_ptr<RenderBackend>        backend_;
    intrusive_ptr<Scene>                scene_;         // optional default content
    intrusive_ptr<Camera>               master_camera_; // optional interactive view camera
    ShaderPreset                        shader_preset_{ ShaderPreset::StandardPhong };
    std::vector<intrusive_ptr<Overlay>> overlays_;
    std::vector<PassSlot>               passes_;        // uniform ordered scene-pass registry
    intrusive_ptr<CameraManipulator>    camera_manipulator_;
    FrameContext                        frame_ctx_;
    void*                               native_handle_      = nullptr;
    bool                                initialized_        = false;

    /// Named-output registry: slot name -> published render target. Cleared at
    /// the start of every frame and rebuilt as the ordered passes publish.
    std::map<String, intrusive_ptr<RenderTarget>> outputs_;
};

V_GRAPHICS_NS_END
