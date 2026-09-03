#pragma once
#include "graphics_global.hpp"
#include "ShaderPreset.hpp"

#include <vector>

#include <vine/Color.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/intrusive_ptr.hpp>
#include <vine/raw_ptr.hpp>

V_GRAPHICS_NS_BEGIN

class Camera;
class Light;
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
     * Pass nullptr to render into the default (window) framebuffer. When the
     * backend reports supportsRenderTargets() and a valid off-screen target
     * is passed, the backend creates and manages the target's GPU
     * attachments (created lazily on first bind, rebuilt when the target is
     * resized) and renders into it until another target is set. The backend
     * owns the attachments; the RenderTarget stays a logical description.
     *
     * @param target Render target, or nullptr for the default framebuffer.
     */
    virtual void setRenderTarget(RenderTarget* target) = 0;

    /** @brief Returns whether off-screen render targets are supported.
     *
     * Backends that support them honour non-null RenderTargets in
     * setRenderTarget() by creating and binding GPU attachments. Backends
     * that do not support them only accept nullptr (the default framebuffer)
     * and should ignore off-screen targets.
     *
     * @return true when setRenderTarget() accepts off-screen targets.
     */
    virtual bool supportsRenderTargets()
    {
        return false;
    }

    /** @brief Draws a full-screen textured pass sampling a target's colour.
     *
     * Samples the colour attachment of @p source (a target written earlier in
     * the same frame, e.g. an off-screen render-to-texture pass) through a
     * full-screen textured triangle drawn into the CURRENT target — the one
     * set by the most recent setRenderTarget() (nullptr = the default
     * framebuffer) — respecting any sub-viewport configured via setViewport().
     * This is the low-level primitive behind a screen/composite pass; the
     * target to sample stays a logical RenderTarget and the backend resolves
     * it to its own GPU texture. The default no-op lets backends without
     * texture-input support ignore the call.
     *
     * @param source Target whose colour texture to sample, or nullptr.
     */
    virtual void drawScreenTexture(vine::graphics::RenderTarget* source)
    {
        (void)source;
    }

    /** @brief Releases backend resources for a removed overlay.
     *
     * Called by the engine just before an overlay is dropped so the backend
     * can stop drawing it and free its GPU objects (view / pipelines / view
     * slots). An overlay draws through its pass, so the overlay's camera here
     * is the pass camera (pass->camera()); the backend retains the overlay's
     * sub-view keyed by that camera, and this call removes that key. Overlays
     * carry no Vine-side GPU state, so the backend is the only owner of these
     * resources. The engine only calls this after confirming the pass is no
     * longer used elsewhere (main pass / extra pass / another overlay). The
     * default no-op lets backends that keep no per-overlay GPU state ignore
     * the call.
     *
     * @param overlay_camera The overlay's pass camera (the key the backend
     *                       used to retain the overlay's view), or nullptr.
     */
    virtual void releaseOverlay(raw_ptr<const Camera> overlay_camera)
    {
        (void)overlay_camera;
    }

    /** @brief Releases backend GPU resources for a removed render target.
     *
     * Called by the engine before a target's owning pass/slot is destroyed.
     * The backend owns the target's GPU attachments (images / views / render
     * passes / framebuffers / a per-target scene bridge) plus any sampling
     * (PiP) state, so it must free them here to keep a closed resource loop.
     * The target object itself stays a logical description owned by the
     * caller. The default no-op lets backends without off-screen targets
     * ignore the call.
     *
     * @param target The render target being removed, or nullptr.
     */
    virtual void releaseRenderTarget(vine::graphics::RenderTarget* target)
    {
        (void)target;
    }

    /** @brief Renders a list of commands.
     *
     * @param commands Render commands to draw.
     * @param camera   Camera used for view/projection.
     */
    virtual void render(const std::vector<RenderCommand>& commands, const Camera* camera) = 0;

    /** @brief Sets the light sources for the upcoming render() pass.
     *
     * Called by RenderPass::execute() from the pass's content scene before
     * render(), so every pass lights whatever scene it renders. Backends that
     * support scene lights replace any view-level default light (e.g. a
     * headlight) with the given lights; an empty list restores the backend
     * default. Lights are borrowed for the duration of the call.
     *
     * @param lights Lights of the content scene, or empty for the backend
     *               default.
     */
    virtual void setLights(const std::vector<raw_ptr<const Light>>& lights)
    {
        (void)lights;
    }

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

    /** @brief Selects the shading-model preset for scene geometry.
     *
     * Must be called before initialize(); the backend maps the preset onto its
     * shader/material pipeline (vsg: Phong vs flat ShaderSet). Presets without
     * a backend implementation yet (Pbr / ShadowedPhong) fall back to
     * StandardPhong. Default no-op.
     *
     * @param preset Shading-model preset.
     */
    virtual void setShaderPreset(ShaderPreset preset)
    {
        (void)preset;
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
