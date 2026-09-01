#pragma once
#include "graphics_global.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/String.hpp>

#include "BoundingBox.hpp"

V_GRAPHICS_NS_BEGIN

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
    BoundingBox boundingBox() const;

    /** @brief Gets the bound material. */
    Material* material() const;

    /** @brief Sets the bound material.
     *
     * @param m Material, or nullptr to clear.
     */
    void setMaterial(Material* m);

  protected:
    /** @brief Computes the local-space bounding box.
     *
     * Subclasses must implement this.
     *
     * @return Local-space bounding box.
     */
    virtual BoundingBox computeBoundingBox() const = 0;

  private:
    struct Data;
    Data* const d;
};

using DrawablePtr = intrusive_ptr<Drawable>;

V_GRAPHICS_NS_END
