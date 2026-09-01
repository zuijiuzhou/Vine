#pragma once
#include "Drawable.hpp"
#include "graphics_global.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/geometry/Shape.hpp>
#include <cstddef>

V_GRAPHICS_NS_BEGIN

/**
 * @brief Geometry drawable wrapping a vine::geometry::Shape.
 *
 * Represents renderable geometry (mesh, BRep, primitive) by referencing a
 * Shape. Provides mesh-level queries such as vertex/triangle counts.
 */
class V_GRAPHICS_API Geometry : public Drawable {
    V_OBJECT_META_DECL;

  public:
    Geometry();
    ~Geometry();

  public:
    /** @brief Gets the associated shape (may be null). */
    const vine::geometry::Shape* shape() const;

    /** @brief Sets the associated shape.
     *
     * @param shape Shape to render, or nullptr to clear.
     */
    void setShape(vine::geometry::Shape* shape);

    /** @brief Gets the triangle count (0 when not a mesh shape). */
    std::size_t triangleCount() const;

    /** @brief Gets the vertex count (0 when not a mesh shape). */
    std::size_t vertexCount() const;

  protected:
    BoundingBox computeBoundingBox() const override;

  private:
    struct Data;
    Data* const d;
};

using GeometryPtr = intrusive_ptr<Geometry>;

V_GRAPHICS_NS_END
