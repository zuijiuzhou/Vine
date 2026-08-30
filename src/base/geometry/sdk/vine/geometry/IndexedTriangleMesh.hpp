#pragma once

#include "geometry_global.hpp"

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "Array.hpp"
#include "Mesh.hpp"

V_GEOMETRY_NS_BEGIN

/**
 * @brief A triangle mesh with shared vertices referenced by indices.
 *
 * Vertices live in a single position array; each triangle references three
 * vertex indices. Normals and texture coordinates, when present, match the
 * position array 1:1.
 */
class V_GEOMETRY_API IndexedTriangleMesh : public Mesh {
    V_OBJECT_META_DECL;

  public:
    IndexedTriangleMesh();

  public:
    /**
     * @brief Returns the shared vertex positions.
     *
     * @return Position array.
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
     * @brief Returns the triangle indices.
     *
     * @return Index array (3 entries per triangle).
     */
    const UInt32Array& indices() const;

    /**
     * @brief Replaces the shared vertex positions.
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
     * @brief Replaces the triangle indices.
     *
     * @param indices New index array.
     */
    void setIndices(UInt32Array indices);

    /**
     * @brief Appends a vertex and returns its index.
     *
     * @param position Vertex position.
     * @return Index of the appended vertex.
     */
    std::uint32_t addVertex(const vine::math::Vec3f& position);

    /**
     * @brief Appends one triangle from vertex indices.
     *
     * @param i0 First vertex index.
     * @param i1 Second vertex index.
     * @param i2 Third vertex index.
     */
    void addTriangle(std::uint32_t i0, std::uint32_t i1, std::uint32_t i2);

    /**
     * @brief Removes all vertices and indices.
     */
    void clear();

    /**
     * @brief Returns the number of vertices.
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
    /// Shared vertex positions.
    Vec3fArray positions_;
    /// Optional per-vertex normals (same size as positions when set).
    Vec3fArray normals_;
    /// Optional per-vertex texture coordinates (same size as positions when set).
    Vec2fArray texcoords_;
    /// Vertex indices, three per triangle.
    UInt32Array indices_;
};

V_GEOMETRY_NS_END
