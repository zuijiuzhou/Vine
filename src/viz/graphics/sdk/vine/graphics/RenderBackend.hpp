#pragma once
#include "graphics_global.hpp"

#include <vector>

#include <vine/Color.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/intrusive_ptr.hpp>

V_GRAPHICS_NS_BEGIN

class Camera;
class MaterialManager;
class RenderTarget;
class RenderPass;
struct RenderCommand;

/**
 * @brief Abstract render backend interface.
 *
 * Defines the contract that concrete graphics backends (OpenGL, Vulkan, etc.)
 * must implement. Supports both high-level pass execution and low-level
 * command rendering.
 *
 * RenderBackend is reference-counted: factories return an intrusive_ptr and
 * RenderEngine keeps its own reference, so ownership and lifetime are
 * explicit.
 */
class V_GRAPHICS_API RenderBackend : public Object, public RefCounted<RenderBackend> {
    V_OBJECT_META_DECL;

  public:
    virtual ~RenderBackend() = default;

    /** @brief Initializes the backend. */
    virtual bool initialize() = 0;

    /** @brief Releases backend resources. */
    virtual void shutdown() = 0;

    /** @brief Begins a frame. */
    virtual void beginFrame() = 0;

    /** @brief Ends a frame. */
    virtual void endFrame() = 0;

    /** @brief Executes a single render pass.
     *
     * @param pass     The render pass to execute.
     * @param commands Render commands collected for the pass.
     */
    virtual void executePass(const RenderPass* pass, const std::vector<RenderCommand>& commands) = 0;

    /** @brief Sets the active render target.
     *
     * @param target Render target, or nullptr for the default framebuffer.
     */
    virtual void setRenderTarget(RenderTarget* target) = 0;

    /** @brief Renders a list of commands.
     *
     * @param commands Render commands to draw.
     * @param camera   Camera used for view/projection.
     */
    virtual void render(const std::vector<RenderCommand>& commands, const Camera* camera) = 0;

    /** @brief Clears buffers.
     *
     * @param backgroundColor Clear color.
     * @param clearDepth      Whether to also clear the depth buffer.
     */
    virtual void clear(const Color& backgroundColor, bool clearDepth = true) = 0;

    /** @brief Swaps buffers (double buffering). */
    virtual void swapBuffers() = 0;

    /** @brief Gets the backend's material manager.
     *
     * Returns nullptr when the backend has no material manager, e.g. before
     * initialize() or when the backend does not support materials. The
     * returned manager stays valid for the backend's lifetime.
     *
     * @return The backend material manager, or nullptr.
     */
    virtual MaterialManager* materialManager()
    {
        return nullptr;
    }

    /** @brief Binds a host native window the backend may render into.
     *
     * Called by RenderEngine before initialize() when the host provides an
     * existing native window (e.g. a Qt QWindow handle). The backend attaches
     * to it instead of creating its own window. Default no-op.
     *
     * @param native_handle Native window handle (HWND on Windows), or nullptr.
     */
    virtual void setWindowHandle(void* native_handle)
    {
        (void)native_handle;
    }

    /** @brief Handles a change of the rendering surface size.
     *
     * @param width  New surface width in pixels.
     * @param height New surface height in pixels.
     */
    virtual void resize(int width, int height)
    {
        (void)width;
        (void)height;
    }

    /** @brief Restricts subsequent drawing to a sub-rectangle of the surface.
     *
     * Called by RenderPass::execute() before drawing a pass that owns a
     * sub-viewport (e.g. an axis gizmo in a screen corner). Backends should
     * combine the viewport and scissor to that rectangle. The default no-op
     * keeps drawing to the full surface, which is correct for backends that
     * do not support sub-viewports yet.
     *
     * @param x      Viewport origin x in device pixels.
     * @param y      Viewport origin y in device pixels (top-left origin).
     * @param width  Viewport width in device pixels.
     * @param height Viewport height in device pixels.
     */
    virtual void setViewport(int x, int y, int width, int height)
    {
        (void)x;
        (void)y;
        (void)width;
        (void)height;
    }

    /** @brief Gets the native handle the backend is currently attached to.
     *
     * Returns the native window handle (HWND on Windows) the backend bound
     * its render surface to during initialize(), or nullptr when the backend
     * is not attached to a host surface (standalone window or not yet
     * initialized). Host code can compare this against the current window
     * context handle to detect when the windowing system recreated the native
     * surface underneath the backend.
     *
     * @return The attached native handle, or nullptr.
     */
    virtual void* nativeHandle() const
    {
        return nullptr;
    }

  protected:
    RenderBackend() = default;
};

V_GRAPHICS_NS_END
