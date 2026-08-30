#pragma once

#include "geometry_global.hpp"

#include <cstddef>
#include <utility>
#include <vector>

#include "Array.hpp"
#include "Mesh.hpp"

V_GEOMETRY_NS_BEGIN

/**
 * @brief A non-indexed triangle mesh (triangle soup).
 *
 * Positions are stored as three consecutive vertices per triangle. Normals
 * and texture coordinates, when present, match the position array 1:1.
 */
class V_GEOMETRY_API TriangleMesh : public Mesh {
    V_OBJECT_META_DECL;

  public:
    TriangleMesh();

  public:
    /**
     * @brief Returns the vertex positions.
     *
     * @return Position array (3 entries per triangle).
     */
    const Vec3fArray& positions() const;

    /**
     * @brief Returns the per-vertex normals.
     *
     * @return Normal array, may be empty.
     */
    const Vec3fArray& normals() const;

    /**
     * @brief Returns the per-vertex texture coordinates.
     *
     * @return Texcoord array, may be empty.
     */
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
     * @brief Appends one triangle.
     *
     * @param a First vertex.
     * @param b Second vertex.
     * @param c Third vertex.
     */
    void addTriangle(const vine::math::Vec3f& a, const vine::math::Vec3f& b, const vine::math::Vec3f& c);

    /**
     * @brief Removes all vertices and attributes.
     */
    void clear();

    /**
     * @brief Returns the number of stored vertices.
     *
     * @return Vertex count.
     */
    [[nodiscard]]
    std::size_t vertexCount() const;

    /**
     * @brief Returns the number of triangles.
     *
     * @return Triangle count.
     */
    [[nodiscard]]
    std::size_t triangleCount() const;

    [[nodiscard]]
    bool isValid() const override;

  private:
    /// Vertices, three per triangle.
    Vec3fArray positions_;
    /// Optional per-vertex normals (same size as positions when set).
    Vec3fArray normals_;
    /// Optional per-vertex texture coordinates (same size as positions when set).
    Vec2fArray texcoords_;
};

V_GEOMETRY_NS_END
