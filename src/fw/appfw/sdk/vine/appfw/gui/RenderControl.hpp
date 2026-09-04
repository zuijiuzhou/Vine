#pragma once

#include "Control.hpp"

#include <vine/intrusive_ptr.hpp>

namespace vine::graphics
{
class RenderEngine;
}

V_APPFWGUI_NS_BEGIN

/**
 * @brief Render view control: hosts a native QWindow render surface inside a
 * QWidget container and drives a RenderEngine.
 *
 * The actual render surface is a QWindow (the layer a render backend binds
 * to). It is nested inside a QWidget via QWidget::createWindowContainer so it
 * can be hosted by the QWidget-based Control — this keeps Control unchanged
 * (it holds a QWidget) while the render layer gets the native QWindow it
 * needs.
 *
 * RenderControl owns the surface and the wiring: it creates the RenderEngine
 * internally and defaults to the first registered render backend on init()
 * (an explicit backend can be attached via engine()->setBackend() first);
 * Qt events on the surface and its host widget are translated and pushed to
 * the engine (engine()->pushEvent()) for the camera manipulator.
 */
class V_APPFW_API RenderControl : public Control {
    V_OBJECT_META_DECL;

  public:
    RenderControl();
    ~RenderControl() override;

  public:
    /** @brief Gets the render engine created by this control.
     *
     * The caller attaches a render backend to it (engine()->setBackend(...))
     * before calling init().
     *
     * @return The engine, or nullptr when creation failed.
     */
    vine::graphics::RenderEngine* engine() const;

    /** @brief Wires the native surface into the engine and initializes it.
     *
     * When no backend was attached via engine()->setBackend(), the first
     * registered render backend (RenderBackendRegistry) is used by default.
     * The backend attaches to the native surface, which only has a usable
     * size once the window is shown: the host calls init() at that point.
     * Idempotent and re-entrant: when Qt later destroys and recreates the
     * native surface, the backend is released via the surface-destroy event
     * and RenderControl re-attaches automatically once the new surface is
     * created and laid out (host init() calls remain safe). Resize and
     * surface-created handling is deferred until after Qt's layout pass so
     * the native window is at its final size when the swapchain is rebuilt.
     *
     * @return true once the engine initialized successfully, false when the
     *         surface was not ready yet (the host should retry later).
     */
    bool init();

    /** @brief Renders one frame through the engine. */
    void renderFrame();

    /** @brief Fits the whole scene into the view (falls back to the home
     * view when the scene is empty) and renders a frame. */
    void fitToScreen();

    /** @brief Gets the render surface device pixel ratio.
     *
     * Qt reports widget sizes in logical pixels; a render backend draws into
     * the native surface in device pixels, so HUD / sub-viewport positioning
     * (e.g. the axis gizmo) on high-DPI displays must scale by this factor.
     *
     * @return Device pixel ratio (1.0 when the surface is not available).
     */
    double devicePixelRatio() const;

  private:
    /** @brief One-time wiring of the default backend and the surface/host
     * event filter. Idempotent. */
    void wireEvents();

    /** @brief Handles native-surface destruction (Qt recreated the HWND):
     * releases the backend and clears the attach state. */
    void onSurfaceDestroyed();

    /** @brief Handles a surface resize/creation notice by scheduling a
     * deferred update. */
    void onSurfaceResized();

    /** @brief Coalesces and defers a surface update until after Qt's layout
     * pass. */
    void scheduleSurfaceUpdate();

    /** @brief Rebuilds/resizes the backend for the final surface size and
     * requests settle frames. */
    void handleSurfaceUpdate();

    /** @brief Requests a few extra display-synced frames after a resize. */
    void requestSettleFrames();

    /** @brief Renders one display-synced settle frame (UpdateRequest). */
    void onSurfaceUpdate();

    /** @brief Initializes the engine once the native surface is exposed. */
    void initializeBackend();

    /** @brief Pops up the view context menu at the cursor position. */
    void showContextMenu();

    /** @brief Gets the native handle of the render surface (HWND on Windows). */
    void* nativeHandle() const;

    /** @brief Gets the render surface width in pixels. */
    int surfaceWidth() const;

    /** @brief Gets the render surface height in pixels. */
    int surfaceHeight() const;

    /** @brief Gets whether the render surface is visible. */
    bool surfaceVisible() const;

    struct Impl;
    Impl* const d;
};

V_APPFWGUI_NS_END
