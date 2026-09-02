#pragma once
#include "graphics_global.hpp"

#include "Overlay.hpp"

V_GRAPHICS_NS_BEGIN

/**
 * @brief A small world-orientation indicator drawn as an overlay.
 *
 * AxisGizmo builds a content scene with three coloured sticks along +X (red),
 * +Y (green) and +Z (blue), rendered into a square sub-viewport in the
 * bottom-left corner of the surface. The gizmo camera mirrors the source
 * camera's orientation (Overlay::MirrorMode::Orientation) while keeping its
 * own framing distance, so the sticks always show the current world axes as
 * the main camera rotates.
 *
 * Example:
 * \code
 * auto gizmo = intrusive_ptr<AxisGizmo>(new AxisGizmo());
 * gizmo->setSourceCamera(engine->camera());
 * engine->addOverlay(gizmo);
 * \endcode
 */
class V_GRAPHICS_API AxisGizmo : public Overlay {
    V_OBJECT_META_DECL;

  public:
    /** @brief Constructs a gizmo with three unit axis sticks.
     *
     * The pass viewport is (re)positioned to the bottom-left corner by
     * onSurfaceResized(); until the first resize it defaults to a 96px box
     * at the origin corner.
     */
    AxisGizmo();

    /** @brief Destroys the gizmo. */
    ~AxisGizmo() override;

    /** @brief Sets the stick length in world units.
     *
     * @param length Positive stick length (default 1).
     */
    void setAxisLength(double length);

    /** @brief Sets the stick cross-section half extent in world units.
     *
     * @param thickness Positive half thickness (default 0.09).
     */
    void setThickness(double thickness);

    /** @brief Sets the on-screen box size in device pixels.
     *
     * @param size Side length of the square gizmo viewport (default 96).
     */
    void setBoxSize(int size);

    /** @brief Sets the ratio between logical surface size and device pixels.
     *
     * Overlays are positioned in device pixels because the render backend
     * draws into a native surface sized in device pixels. Qt reports logical
     * sizes, so hosts on high-DPI displays must supply their devicePixelRatio
     * (default 1).
     *
     * @param ratio Device pixel ratio (> 0).
     */
    void setPixelRatio(double ratio);

    /** @brief Positions the gizmo viewport in the bottom-left corner. */
    void onSurfaceResized(int width, int height) override;

  private:
    /** @brief Rebuilds the axis content from the current geometry settings. */
    void rebuild();

    /// Owned framing camera (kept alive for the pass, which stores a raw ref).
    intrusive_ptr<Camera> camera_;
    double axis_length_ = 1.0;
    double thickness_ = 0.09;
    double pixel_ratio_ = 1.0;
    int margin_px_ = 16;
    int size_px_ = 96;
    int surface_w_ = 0;
    int surface_h_ = 0;
};

using AxisGizmoPtr = intrusive_ptr<AxisGizmo>;

V_GRAPHICS_NS_END
