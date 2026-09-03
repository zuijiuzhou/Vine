#pragma once
#include "graphics_global.hpp"

V_GRAPHICS_NS_BEGIN

/**
 * @brief Shading-model presets for rendering scene geometry.
 *
 * A ShaderPreset selects how lit (or unlit) content is shaded. It is a
 * semantic, backend-agnostic knob held by the render configuration (the
 * engine forwards it to the backend before initialize()); each backend maps
 * a preset to its own shader/material pipeline. Presets marked "reserved"
 * have no backend implementation yet and fall back to StandardPhong until
 * their prerequisite slice lands.
 */
enum class ShaderPreset {
    StandardPhong,  ///< Lit Phong-style shading with per-material diffuse/specular.
    FlatShaded,     ///< Unlit flat (vertex-lit constant) shading; shares the
                    ///< Phong material value, so it is a drop-in for the
                    ///< current material pipeline.
    Pbr,            ///< Reserved: physics-based shading (needs its own
                    ///< PBR material value) - deferred to the custom-shader
                    ///< slice.
    ShadowedPhong,  ///< Reserved: Phong + shadow mapping - deferred until the
                    ///< shadow slice (last in the roadmap).
};

V_GRAPHICS_NS_END
