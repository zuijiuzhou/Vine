#pragma once
#include "graphics_global.hpp"
#include "Drawable.hpp"
#include "Material.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/math/Matrix4x4.hpp>

V_GRAPHICS_NS_BEGIN

using vine::math::Mat4d;


/**
 * @brief A single drawable rendering instruction.
 *
 * Encapsulates everything needed to render one object: the drawable itself,
 * its material, and its world-space transform.
 */
struct V_GRAPHICS_API RenderCommand {
    /** Drawable to render. */
    DrawablePtr drawable;

    /** Material to use. */
    MaterialPtr material;

    /** World-space model matrix. */
    Mat4d modelMatrix;

    /** Whether the object is transparent (requires sorted rendering). */
    bool isTransparent = false;

    /** Effective opacity in [0, 1]: scene x node x drawable x material. */
    float opacity = 1.0f;

    /** @brief Default constructor. */
    RenderCommand() = default;

    /** @brief Constructs a render command.
     *
     * @param d     Drawable to render.
     * @param m     Material to use.
     * @param model World-space model matrix.
     */
    RenderCommand(Drawable* d, Material* m, const Mat4d& model);
};

V_GRAPHICS_NS_END
