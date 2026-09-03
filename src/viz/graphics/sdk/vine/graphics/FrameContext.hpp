#pragma once
#include "graphics_global.hpp"

V_GRAPHICS_NS_BEGIN

/**
 * @brief Per-frame shared context (skeleton).
 *
 * Carries values that many passes / overlays need during one frame, updated
 * by RenderEngine each frame: elapsed time and the current surface size.
 * Later extensions: previous-frame view-projection matrices (for temporal
 * effects such as motion vectors / TAA) and the active light list.
 */
struct V_GRAPHICS_API FrameContext {
    double dt = 0.0;  ///< Seconds elapsed since the previous frame.
    int surface_width = 0;  ///< Surface width in device pixels (0 until first resize).
    int surface_height = 0; ///< Surface height in device pixels.
};

V_GRAPHICS_NS_END
