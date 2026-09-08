#pragma once
#include "vsg_global.hpp"

#include <vine/graphics/RenderBackend.hpp>
#include <vine/raw_ptr.hpp>

#include <vsg/app/Viewer.h>
#include <vsg/core/ref_ptr.h>

namespace vine::graphics
{

class Camera;
class Light;
class RenderPass;
class RenderTarget;
class ShaderProgram;
struct RenderCommand;

} // namespace vine::graphics

V_VSG_NS_BEGIN

/**
 * @brief VSG render backend implementing vine::graphics::RenderBackend.
 *
 * VsgRenderer is the VulkanSceneGraph implementation of the graphics
 * abstraction layer. It drives a vsg::Viewer running on a vsg::Window and
 * reconciles a retained vsg scene against the per-frame render command
 * stream (SceneBridge), so runtime scene edits are reflected without
 * re-initializing the backend.
 */
class V_VSG_API VsgRenderer : public vine::graphics::RenderBackend {
  public:
    /** @brief Constructs a renderer.
     *
     * The engine owns the pipeline and drives content per pass, so the
     * renderer binds neither a Vine scene nor a camera: content slots are
     * created lazily from the per-pass render() calls the engine drives.
     */
    VsgRenderer();
    ~VsgRenderer() override;

    // ---- RenderBackend interface ----

    /** @brief Initializes the window, viewer, camera and pipeline.
     *
     * @return true when initialization succeeded.
     */
    bool initialize() override;

    /** @brief Closes the window and releases the viewer. */
    void shutdown() override;

    /** @brief Begins a frame (advance + handle events). */
    void beginFrame() override;

    /** @brief Ends a frame (viewer update). */
    void endFrame() override;

    /** @brief Sets the render target.
     *
     * Off-screen targets are not yet supported; only the default framebuffer
     * (nullptr) is valid.
     *
     * @param target Render target, or nullptr for the default framebuffer.
     */
    void setRenderTarget(vine::raw_ptr<vine::graphics::RenderTarget> target) override;

    /** @brief Returns whether off-screen render targets are supported.
     *
     * @return true.
     */
    bool supportsRenderTargets() override;

    /** @brief Draws a full-screen textured pass sampling a target's colour
     * attachment.
     *
     * Samples colour attachment @p attachment of @p source (an off-screen
     * target this backend rendered earlier in the same frame) through a
     * full-screen textured triangle drawn into the CURRENT target (the one
     * set by setRenderTarget(), nullptr = the window) within the sub-viewport
     * set by setViewport() (picture-in-picture). A multi-attachment target
     * (MRT / G-buffer) exposes each colour attachment as an independent
     * sampleable texture. EXPERIMENTAL.
     *
     * @param source     Off-screen target whose colour texture to sample.
     * @param attachment Colour attachment index to sample (0 = first).
     */
    void drawScreenTexture(vine::graphics::RenderTarget* source, int attachment) override;

    /** @brief Draws a full-screen pass through a user fragment program,
     * sampling every colour attachment of an MRT source (deferred lighting).
     *
     * See RenderBackend::drawScreenProgram for the contract. The backend
     * compiles the program's fragment stage, binds each source colour
     * attachment as a sampled texture (binding 0..N-1) and pushes view-space
     * light parameters each frame.
     *
     * @param source  MRT target whose colour attachments are sampled.
     * @param program User program supplying the fragment stage.
     * @param camera  Camera whose view transforms the pushed lights.
     */
    void drawScreenProgram(vine::graphics::RenderTarget*                       source,
                           vine::raw_ptr<const vine::graphics::ShaderProgram> program,
                           vine::raw_ptr<const vine::graphics::Camera>        camera) override;

    /** @brief Stops drawing and frees GPU state for a removed pass' window
     * content slot.
     *
     * @param camera The removed pass's camera (the content-slot key), or null.
     * @param order  The removed pass's explicit pipeline order (the
     *               content-slot key within that camera).
     */
    void releaseWindowLayer(raw_ptr<const vine::graphics::Camera> camera, int order) override;

    /** @brief Notifies the renderer of the order of the pass about to render.
     *
     * The engine announces each pass's explicit pipeline order (addPass())
     * right before it executes; the value is consumed by the following
     * render() and used both as the KEY of this target's retained content slot
     * (with the pass camera) and as its stacking order (ascending), so
     * stacking always follows the user-set order even when slots are first
     * created out of order (pre-frame warm-up).
     *
     * @param order The current pass's explicit pipeline order.
     */
    void setPassOrder(int order) override;

    /** @brief Frees GPU state (offscreen graph + PiP slot) for a removed target.
     *
     * @param target Render target being removed, or null.
     */
    void releaseRenderTarget(vine::graphics::RenderTarget* target) override;

    /** @brief Renders the current frame from the render command stream.
     *
     * The retained vsg scene is reconciled against the commands (SceneBridge)
     * and (re)compiled when the set of drawables changed structurally.
     *
     * @param commands Render commands for this frame.
     * @param camera   Camera used for view/projection.
     */
    void render(const std::vector<vine::graphics::RenderCommand>& commands, vine::raw_ptr<const vine::graphics::Camera> camera) override;

    /** @brief Sets the light sources for the upcoming render() pass.
     *
     * Called by RenderPass::execute() from the pass's content scene before
     * render(). The lights replace the view's default light (the headlight on
     * the main view / the ambient fill on off-screen views); an empty list
     * keeps the view's default. Lights are borrowed for the duration of the
     * call and translated to vsg light nodes immediately.
     *
     * @param lights Lights of the content scene, or empty for the default.
     */
    void setLights(const std::vector<vine::raw_ptr<const vine::graphics::Light>>& lights) override;

    /** @brief Sets the clear color and depth-clear state. */
    void clear(const vine::Color& backgroundColor, bool clearDepth) override;

    /** @brief Sets how the next render()'s content handles depth (see
     * RenderBackend::setDepthMode).
     *
     * @param mode Depth handling for the next render() content.
     */
    void setDepthMode(vine::graphics::DepthMode mode) override;

    /** @brief Presents the rendered frame. */
    void swapBuffers() override;

    /** @brief Gets the backend's material manager.
     *
     * The manager is created with the renderer and stays valid for the
     * renderer's lifetime.
     *
     * @return The VSG material manager.
     */
    vine::raw_ptr<vine::graphics::MaterialManager> materialManager() override;

    /** @brief Binds a host native window; when present, the renderer renders
     * into that window's native surface instead of creating its own window.
     *
     * @param native_handle Native window handle (HWND on Windows), or nullptr.
     */
    void setWindowHandle(void* native_handle) override;

    /** @brief Rebuilds the swapchain for the new surface size.
     *
     * @param width  New surface width in pixels.
     * @param height New surface height in pixels.
     */
    void resize(int width, int height) override;

    /** @brief Restricts the next render() to a sub-viewport of the surface.
     *
     * Called by RenderPass::execute() before a pass that owns a sub-viewport
     * (e.g. an axis gizmo in a screen corner). The rectangle is consumed by
     * the following render() call, which draws into its own RenderGraph. A
     * pass without a sub-viewport renders the full surface.
     *
     * @param x      Viewport origin x in device pixels.
     * @param y      Viewport origin y in device pixels (top-left origin).
     * @param width  Viewport width in device pixels.
     * @param height Viewport height in device pixels.
     */
    void setViewport(int x, int y, int width, int height) override;

    /** @brief Gets the native handle the renderer attached its window to.
     *
     * Returns the host surface handle (HWND on Windows) used when the window
     * was created, or nullptr when not attached to a host surface.
     *
     * @return The attached native handle, or nullptr.
     */
    void* nativeHandle() const override;

    /** @brief Selects the shading-model preset for scene geometry.
     *
     * Forwarded by the engine before initialize(); maps onto vsg's Phong or
     * flat ShaderSet. Reserved presets (Pbr / ShadowedPhong) fall back to
     * Phong until implemented.
     *
     * @param preset Shading-model preset.
     */
    void setShaderPreset(vine::graphics::ShaderPreset preset) override;

    // ---- VSG convenience interface ----

    /** @brief Convenience: runs one full frame loop (begin/render/end/swap).
     *
     * Equivalent to calling beginFrame(), render(), endFrame() and
     * swapBuffers() in sequence.
     */
    void frame();

    /** @brief Gets the underlying vsg viewer. */
    ::vsg::ref_ptr<::vsg::Viewer> viewer() const;

  private:
    /** @brief Builds (or rebuilds) an off-screen target's GPU attachments and
     * empty render graph, sized to the target.
     *
     * Called from render() when an off-screen target is first rendered or was
     * resized. The graph is created without Views; content-slot Views are
     * appended by setupContentSlot() as passes render into the target (C6.4:
     * the same multi-slot mechanism the window target uses, so one RT can
     * bake several content groups with different programs / depth policy). A
     * rebuild first releases the previous graph and every content slot
     * compiled against it. EXPERIMENTAL: needs on-device validation.
     *
     * @param target Off-screen target to (re)build.
     */
    void buildOffscreenTarget(vine::graphics::RenderTarget* target);

    /** @brief Keeps the command graph's off-screen render graphs in a
     * dependency-valid record order.
     *
     * A consumer's graph is ordered after every target it samples (its PiP
     * screen / fullscreen-program sources), so a same-frame producer chain
     * (A -> B -> window) samples the CURRENT frame; the window swapchain graph
     * stays the last child. Called whenever an off-screen graph is (re)built
     * or a sampling slot is newly attached — creation order alone cannot
     * guarantee the dependency order (a producer rebuilt after its consumers
     * existed, or a consumer wired to a producer built later would otherwise
     * record the consumer first and sample stale / undefined content).
     */
    void reconcileOffscreenOrder();

    /** @brief Builds (on first use) the retained vsg view for a content slot.
     *
     * A content slot is a View of a TARGET's render graph — the window target
     * (@p target == nullptr) and every off-screen target share this one
     * mechanism. Slots are keyed by (camera, explicit pass order): each
     * (camera, @p order) pair is its own retained View, and the views are
     * stacked in ascending @p order — the order the caller gave addPass() —
     * so several passes sharing one camera stack exactly as the user-ordered
     * pipeline runs, regardless of creation order. @p depth_mode is the
     * content's depth handling (explicit per pass, independent of clearing);
     * @p presenting marks the full-target pass that cleared the target (its
     * default light seeds the window headlight when there is no scene light).
     * Lights come from the content scene each frame.
     *
     * @param target     Output target key the slot lives under (nullptr = window).
     * @param camera     Vine camera identifying the slot.
     * @param order      The pass's explicit pipeline order (slot key + stacking).
     * @param depth_mode Depth handling for the slot's content.
     * @param presenting True when this slot is the full-target pass that cleared.
     */
    void setupContentSlot(vine::graphics::RenderTarget* target, vine::graphics::Camera* camera, int order, vine::graphics::DepthMode depth_mode, bool presenting);

    /** @brief Renders one content slot (a View of a target's render graph).
     *
     * The slot is created on first use, keyed by (camera, explicit pass
     * order): each (camera, @p order) pair is its own retained View appended
     * to the target's render graph — the window target (nullptr key) or an
     * off-screen target — stacked in ascending @p order (the pass's explicit
     * pipeline order). @p depth_mode is the content's depth handling (explicit,
     * independent of clearing); @p presenting marks the full-target pass that
     * cleared the target (such content fills the whole target and seeds the
     * window headlight when there is no scene light). Lights come from the
     * content scene each frame.
     *
     * @param target     Output target key the slot lives under (nullptr = the
     *                   window).
     * @param commands   Render commands for this slot.
     * @param camera     Camera identifying the slot.
     * @param lights     Content lights for the slot (empty keeps the slot's
     *                   seeded default light: headlight for the window's
     *                   presenting slot, ambient otherwise).
     * @param depth_mode Depth handling for the slot's content.
     * @param presenting True when this slot is the full-target pass that cleared.
     * @param order      The pass's explicit pipeline order (slot key + stacking).
     * @param vp_x       Viewport origin x in device pixels.
     * @param vp_y       Viewport origin y in device pixels.
     * @param vp_w       Viewport width (0 = full target).
     * @param vp_h       Viewport height (0 = full target).
     */
    void renderContentSlot(vine::graphics::RenderTarget*                     target,
                           const std::vector<vine::graphics::RenderCommand>& commands,
                           vine::raw_ptr<const vine::graphics::Camera>       camera,
                           const std::vector<const vine::graphics::Light*>&  lights,
                           vine::graphics::DepthMode                         depth_mode,
                           bool                                              presenting,
                           int                                               order,
                           int                                               vp_x,
                           int                                               vp_y,
                           int                                               vp_w,
                           int                                               vp_h);

    /** @brief Records and presents the frame (once, when swapBuffers is called). */
    void submitFrame();

    /** @brief Consumes the sub-viewport queued by setViewport() for one pass.
     *
     * Every draw path (main scene, PiP screen, fullscreen program) reads the
     * same pending rectangle and clears it, so the consume is factored here.
     * A pass that never queued a viewport leaves the flags false and the
     * caller substitutes the full surface.
     *
     * @param x Receives the queued origin x (0 when none was queued).
     * @param y Receives the queued origin y (0 when none was queued).
     * @param w Receives the queued width (0 when none was queued).
     * @param h Receives the queued height (0 when none was queued).
     * @return true when a viewport was queued for this pass.
     */
    bool takePendingViewport(int& x, int& y, int& w, int& h);

    /** @brief Moves a slot View into its target graph's children at the
     * position matching its explicit stacking order.
     *
     * Every slot view under a target's render graph carries an explicit order
     * — content slots keyed by (camera, pass order), fullscreen-program views
     * and PiP / present screen views by the pass order announced via
     * setPassOrder(). The children stay sorted ascending by that order, so a
     * fullscreen lighting / present view can be stacked between two content
     * slots (e.g. an opaque depth pass below it and a forward transparent pass
     * above it) instead of always drawing first or last. Any child that maps
     * to no known slot sorts as INT_MAX (drawn last).
     *
     * @param target Output target whose graph receives the view (nullptr =
     *               the window).
     * @param view   The View to position (removed from any current position
     *               first).
     * @param order  The view's explicit stacking order.
     */
    void placeViewByOrder(vine::graphics::RenderTarget* target,
                          const ::vsg::ref_ptr<::vsg::View>& view,
                          int order);

    /** @brief Incrementally compiles only the content-slot views that gained
     * new/rebuild subtrees this frame (D22).
     *
     * vsg compiles Vulkan objects per viewID and can only create a graphics
     * pipeline when the compiling context carries the owning target's render
     * pass (window swapchain or off-screen framebuffer). vsg's own
     * CompileManager pool is built once from the views present at first
     * Viewer::compile() — in Vine that runs on an EMPTY window graph, so the
     * pool's contexts are empty and compileManager->compile(view) silently
     * compiles nothing (and record then hits unbuilt pipelines). This method
     * instead registers each queued view's (window/framebuffer render pass +
     * view) context into the pool on first sight via the public
     * CompileManager::add() API, then compiles that view through its own
     * context only — mirroring what Viewer::compile() does for the whole
     * graph, scoped to the views that actually changed.
     *
     * @return true when every queued view was compiled incrementally, false
     *         when any step failed and the caller should fall back to a full
     *         Viewer::compile().
     */
    bool incrementalCompileViews();

  private:
    struct Impl;
    struct Persistent;
    std::unique_ptr<Impl> impl;
    std::unique_ptr<Persistent> persistent;
};

V_VSG_NS_END
