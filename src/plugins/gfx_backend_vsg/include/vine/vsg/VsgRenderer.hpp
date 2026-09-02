#pragma once
#include "vsg_global.hpp"

#include <vine/graphics/RenderBackend.hpp>

#include <vsg/app/Viewer.h>
#include <vsg/core/ref_ptr.h>

namespace vine::graphics
{
class Scene;
class Camera;
class RenderPass;
struct RenderCommand;
}

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
    VsgRenderer(vine::graphics::Scene* scene, vine::graphics::Camera* camera);
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
    void executePass(const vine::graphics::RenderPass* pass,
                     const std::vector<vine::graphics::RenderCommand>& commands) override;

    /** @brief Sets the render target.
     *
     * Off-screen targets are not yet supported; only the default framebuffer
     * (nullptr) is valid.
     *
     * @param target Render target, or nullptr for the default framebuffer.
     */
    void setRenderTarget(vine::graphics::RenderTarget* target) override;

    /** @brief Renders the current frame from the render command stream.
     *
     * The retained vsg scene is reconciled against the commands (SceneBridge)
     * and (re)compiled when the set of drawables changed structurally.
     *
     * @param commands Render commands for this frame.
     * @param camera   Camera used for view/projection.
     */
    void render(const std::vector<vine::graphics::RenderCommand>& commands,
                const vine::graphics::Camera* camera) override;

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
    vine::graphics::MaterialManager* materialManager() override;

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

    /** @brief Gets the native handle the renderer attached its window to.
     *
     * Returns the host surface handle (HWND on Windows) used when the window
     * was created, or nullptr when not attached to a host surface.
     *
     * @return The attached native handle, or nullptr.
     */
    void* nativeHandle() const override;

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
    struct Data;
    Data* const d;
};

V_VSG_NS_END
