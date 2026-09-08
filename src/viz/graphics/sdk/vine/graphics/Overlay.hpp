#pragma once
#include "graphics_global.hpp"

#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/intrusive_ptr.hpp>
#include <vine/raw_ptr.hpp>

#include "RenderPass.hpp"

V_GRAPHICS_NS_BEGIN

class Camera;
class Scene;

/**
 * @brief Base class for content drawn on top of the main scene (HUD).
 *
 * An overlay pairs a content scene with a render pass (camera, optional
 * sub-viewport and clear policy). The RenderEngine draws every visible
 * overlay after the main pass in ascending zOrder. update() runs once per
 * frame before drawing; the default implementation mirrors the configured
 * source camera onto the overlay camera (see setMirrorMode()).
 *
 * Typical overlays: an axis gizmo in a corner, a minimap, a crosshair, or
 * screen-space markers. Overlays reuse the ordinary Scene / RenderPass /
 * backend pipeline, so a new render backend needs no overlay-specific code.
 */
class V_GRAPHICS_API Overlay : public Object, public RefCounted<Overlay> {
    V_OBJECT_META_DECL;

  public:
    /** @brief How the overlay camera tracks a source camera. */
    enum class MirrorMode {
        /// Camera is fully independent (e.g. a minimap or a 2D screen HUD).
        None = 0,
        /// Copy only the source orientation; keep the overlay's own framing.
        Orientation,
        /// Adopt the source view (eye / target / up) completely.
        FullView,
    };

    /** @brief Constructs an overlay with an empty pass and content scene.
     *
     * The pass has clearing disabled so the overlay draws over the previous
     * frame instead of erasing it.
     */
    Overlay();

    /** @brief Destroys the overlay. */
    ~Overlay() override;

    /** @brief Per-frame update hook.
     *
     * The default implementation applies the configured mirror mode. Derived
     * overlays override this to refresh animated or data-driven content.
     *
     * @param dt Seconds elapsed since the previous frame.
     */
    virtual void update(double dt);

    /** @brief Gets the pass that draws this overlay. */
    raw_ptr<RenderPass> pass() const;

    /** @brief Sets the pass that draws this overlay.
     *
     * The overlay draws through this pass (content scene, clear policy and
     * optional sub-viewport); the pass's camera identifies the overlay to the
     * render backend, which retains the overlay's view keyed by that camera.
     * The overlay keeps a reference. Whether the backend releases the pass's
     * GPU resources when the overlay is removed depends on whether the same
     * pass is still used by the engine (main / extra passes) or another
     * overlay.
     *
     * @param pass Render pass, or null to clear.
     */
    void setPass(intrusive_ptr<RenderPass> pass);

    /** @brief Gets the content scene drawn by this overlay. */
    raw_ptr<Scene> content() const;

    /** @brief Sets the content scene drawn by this overlay.
     *
     * The overlay keeps a reference.
     *
     * @param content Content scene, or null to clear.
     */
    void setContent(intrusive_ptr<Scene> content);

    /** @brief Gets the draw order relative to other overlays (lower first). */
    int zOrder() const;

    /** @brief Sets the draw order relative to other overlays (lower first). */
    void setZOrder(int order);

    /** @brief Returns whether the overlay is drawn this frame. */
    bool visible() const;

    /** @brief Sets whether the overlay is drawn. */
    void setVisible(bool visible);

    /** @brief Sets how the overlay camera tracks a source camera.
     *
     * @param mode Mirror mode.
     */
    void setMirrorMode(MirrorMode mode);

    /** @brief Gets the configured mirror mode. */
    MirrorMode mirrorMode() const;

    /** @brief Sets the camera the overlay mirrors (non-owning).
     *
     * Usually the engine's main camera.
     *
     * @param camera Source camera, or null to disable mirroring.
     */
    void setSourceCamera(raw_ptr<Camera> camera);

    /** @brief Called by the engine when the rendering surface is resized.
     *
     * @param width  New surface width in pixels.
     * @param height New surface height in pixels.
     */
    virtual void onSurfaceResized(int width, int height) {}

  protected:
    /** @brief Applies the configured mirror mode onto the overlay camera. */
    void applyMirror();

  private:
    RenderPassPtr pass_;
    intrusive_ptr<Scene> content_;
    int z_order_ = 0;
    bool visible_ = true;
    MirrorMode mirror_mode_ = MirrorMode::None;
    raw_ptr<Camera> source_camera_ = nullptr;  ///< Non-owning source camera.
};

using OverlayPtr = intrusive_ptr<Overlay>;

V_GRAPHICS_NS_END
