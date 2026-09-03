#pragma once
#include "vsg_global.hpp"

#include <vine/graphics/RenderBackend.hpp>
#include <vine/raw_ptr.hpp>

#include <vsg/app/Viewer.h>
#include <vsg/core/ref_ptr.h>

namespace vine::graphics
{

class Scene;
class Camera;
class Light;
class RenderPass;
class RenderTarget;
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
    /** @brief Constructs a renderer bound to a Vine scene and camera.
     *
     * @param scene  Vine scene to render (must outlive the renderer).
     * @param camera Vine camera used for the view.
     */
    VsgRenderer(vine::raw_ptr<vine::graphics::Scene> scene, vine::raw_ptr<vine::graphics::Camera> camera);
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

    /** @brief Executes a render pass.
     *
     * @param pass     The pass to execute (clear state applied).
     * @param commands Render commands collected for the pass.
     */
    void executePass(vine::raw_ptr<const vine::graphics::RenderPass> pass, const std::vector<vine::graphics::RenderCommand>& commands) override;

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

    /** @brief Draws a full-screen textured pass sampling a target's colour.
     *
     * Samples the colour attachment of @p source (an off-screen target this
     * backend rendered earlier in the same frame) through a full-screen
     * textured triangle drawn into the CURRENT target (the one set by
     * setRenderTarget(), nullptr = the window) within the sub-viewport set
     * by setViewport() (picture-in-picture). EXPERIMENTAL.
     *
     * @param source Off-screen target whose colour texture to sample.
     */
    void drawScreenTexture(vine::graphics::RenderTarget* source) override;

    /** @brief Stops drawing and frees GPU state for a removed overlay.
     *
     * @param overlay_camera Overlay camera retained by this backend, or null.
     */
    void releaseOverlay(raw_ptr<const vine::graphics::Camera> overlay_camera) override;

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
     * Called by RenderPass::execute() before an overlay pass that owns a
     * sub-viewport (e.g. an axis gizmo). The rectangle is consumed by the
     * following render() call, which draws into its own RenderGraph. A pass
     * without a sub-viewport renders the full surface.
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

    /** @brief Gets the translated vsg camera. */
    ::vsg::ref_ptr<::vsg::Camera> vsgCamera() const;

    /** @brief Gets the translated vsg scene graph. */
    ::vsg::ref_ptr<::vsg::Node> vsgScene() const;

  private:
    /** @brief Renders an overlay pass into its own sub-viewport RenderGraph. */
    void
    renderOverlayPass(const std::vector<vine::graphics::RenderCommand>& commands, vine::raw_ptr<const vine::graphics::Camera> camera, int vp_x, int vp_y, int vp_w, int vp_h);

    /** @brief Renders the command stream into an off-screen RenderTarget.
     *
     * Creates (on first use) an off-screen render graph for the target and
     * records the scene into it. EXPERIMENTAL: needs on-device validation.
     *
     * @param target   Off-screen target to render into.
     * @param commands Render commands for this pass.
     * @param camera   Camera used for view/projection.
     * @param lights   Content scene lights (empty keeps the view default).
     */
    void renderOffscreenTarget(vine::graphics::RenderTarget* target,
                               const std::vector<vine::graphics::RenderCommand>& commands,
                               vine::raw_ptr<const vine::graphics::Camera> camera,
                               const std::vector<const vine::graphics::Light*>& lights);

    /** @brief Records and presents the frame (once, when swapBuffers is called). */
    void submitFrame();

    struct Data;
    Data* const d;
};

V_VSG_NS_END
