#pragma once

#include "geometry_global.hpp"

#include <cstddef>

#include <vine/math/Rect3.hpp>

#include "Array.hpp"
#include "Shape.hpp"

V_GEOMETRY_NS_BEGIN

using vine::math::Aabbf;

/**
 * @brief Base class for polygonal mesh shapes.
 *
 * Vertex storage (positions, normals, texture coordinates) is shared by all
 * meshes here; derived classes add their own topology (e.g. an index array).
 */
class V_GEOMETRY_API Mesh : public Shape {
    V_OBJECT_META_DECL;

  protected:
    /// Protected so Mesh cannot be instantiated directly.
    Mesh();

  public:
    /**
     * @brief Returns the vertex positions.
     *
     * @return Position array.
     */
    [[nodiscard]]
    const Vec3fArray& positions() const;

    /**
     * @brief Returns the per-vertex normals.
     *
     * @return Normal array, may be empty.
     */
    [[nodiscard]]
    const Vec3fArray& normals() const;

    /**
     * @brief Returns the per-vertex texture coordinates.
     *
     * @return Texcoord array, may be empty.
     */
    [[nodiscard]]
    const Vec2fArray& texcoords() const;

    /**
     * @brief Replaces the vertex positions.
     *
     * @param positions New position array.
     */
    void setPositions(Vec3fArray positions);

    /**
     * @brief Replaces the per-vertex normals.
     *
     * @param normals New normal array.
     */
    void setNormals(Vec3fArray normals);

    /**
     * @brief Replaces the per-vertex texture coordinates.
     *
     * @param texcoords New texcoord array.
     */
    void setTexcoords(Vec2fArray texcoords);

    /**
     * @brief Returns the number of stored vertices.
     *
     * @return Vertex count.
     */
    [[nodiscard]]
    std::size_t vertexCount() const;

    /**
     * @brief Returns the cached axis-aligned bounding box.
     *
     * Holds whatever was last stored via setAabb() or computed by computeAabb();
     * geometry edits do not update it automatically.
     *
     * @return Cached AABB (empty when none was set or computed yet).
     */
    [[nodiscard]]
    const Aabbf& aabb() const;

    /**
     * @brief Stores an axis-aligned bounding box into the cache.
     *
     * @param aabb Axis-aligned bounding box to cache.
     */
    void setAabb(const Aabbf& aabb);

    /**
     * @brief Computes the AABB enclosing all vertex positions and caches it.
     *
     * An empty position array yields a zero box at the origin.
     *
     * @return The computed AABB (also stored by the cache).
     */
    Aabbf computeAabb();

  protected:
    /**
     * @brief Removes all vertex attribute arrays and resets the AABB cache.
     *
     * Called by derived clear() implementations.
     */
    void clearAttributes();

    /// Vertex positions.
    Vec3fArray positions_;
    /// Optional per-vertex normals (same size as positions when set).
    Vec3fArray normals_;
    /// Optional per-vertex texture coordinates (same size as positions when set).
    Vec2fArray texcoords_;

  private:
    /// Cached axis-aligned bounding box (empty until set or computed).
    Aabbf aabb_{ Aabbf::empty() };
};

V_GEOMETRY_NS_END
