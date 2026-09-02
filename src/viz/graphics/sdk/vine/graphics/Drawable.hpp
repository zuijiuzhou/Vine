#pragma once
#include "graphics_global.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/String.hpp>

#include <vine/math/Rect3.hpp>

V_GRAPHICS_NS_BEGIN

using vine::math::Aabbd;

class Material;
using MaterialPtr = intrusive_ptr<Material>;

/**
 * @brief Pure renderable object (geometry + material).
 *
 * Drawable represents what can be drawn: a material and geometric content.
 * It has no scene-graph transform or hierarchy; those are provided by the
 * owning Node. This mirrors osg::Drawable / OgreNext's Item.
 */
class V_GRAPHICS_API Drawable : public Object, public RefCounted<Drawable> {
    V_OBJECT_META_DECL;

  public:
    Drawable();
    ~Drawable();

  public:
    /** @brief Gets the drawable name. */
    String name() const;

    /** @brief Sets the drawable name. */
    void setName(const String& name);

    /** @brief Gets the local-space bounding box. */
    Aabbd boundingBox() const;

    /** @brief Gets the bound material. */
    Material* material() const;

    /** @brief Sets the bound material.
     *
     * @param m Material, or nullptr to clear.
     */
    void setMaterial(Material* m);

    /** @brief Returns whether the drawable is rendered. */
    bool isVisible() const;

    /** @brief Sets whether the drawable is rendered.
     *
     * Hidden drawables are skipped by command collection; the render backend
     * may keep their resources cached so that toggling visibility back on is
     * cheap.
     *
     * @param visible true to render, false to hide.
     */
    void setVisible(bool visible);

    /** @brief Gets the drawable opacity multiplier in [0, 1].
     *
     * Multiplied with the scene/node opacities and the bound material's
     * opacity to produce the effective per-drawable transparency.
     */
    float opacity() const;

    /** @brief Sets the drawable opacity multiplier in [0, 1]. */
    void setOpacity(float opacity);

  protected:
    /** @brief Computes the local-space bounding box.
     *
     * Subclasses must implement this.
     *
     * @return Local-space bounding box.
     */
    virtual Aabbd computeBoundingBox() const = 0;

  private:
    String name_;
    bool visible_ = true;
    float opacity_ = 1.0f;
    intrusive_ptr<Material> material_;
};

using DrawablePtr = intrusive_ptr<Drawable>;

V_GRAPHICS_NS_END
