#include <vine/geometry/IndexedTriangleMesh.hpp>

V_GEOMETRY_NS_BEGIN

V_OBJECT_META_IMPL(IndexedTriangleMesh, Mesh)

IndexedTriangleMesh::IndexedTriangleMesh()
{
    shape_type_ = ShapeType::IndexedTriangleMesh;
}

const Vec3fArray& IndexedTriangleMesh::positions() const
{
    return positions_;
}

const Vec3fArray& IndexedTriangleMesh::normals() const
{
    return normals_;
}

const Vec2fArray& IndexedTriangleMesh::texcoords() const
{
    return texcoords_;
}

const UInt32Array& IndexedTriangleMesh::indices() const
{
    return indices_;
}

void IndexedTriangleMesh::setPositions(Vec3fArray positions)
{
    positions_ = std::move(positions);
}

void IndexedTriangleMesh::setNormals(Vec3fArray normals)
{
    normals_ = std::move(normals);
}

void IndexedTriangleMesh::setTexcoords(Vec2fArray texcoords)
{
    texcoords_ = std::move(texcoords);
}

void IndexedTriangleMesh::setIndices(UInt32Array indices)
{
    indices_ = std::move(indices);
}

std::uint32_t IndexedTriangleMesh::addVertex(const vine::math::Vec3f& position)
{
    positions_.push_back(position);
    return static_cast<std::uint32_t>(positions_.size() - 1);
}

void IndexedTriangleMesh::addTriangle(std::uint32_t i0, std::uint32_t i1, std::uint32_t i2)
{
    indices_.push_back(i0);
    indices_.push_back(i1);
    indices_.push_back(i2);
}

void IndexedTriangleMesh::clear()
{
    positions_.clear();
    normals_.clear();
    texcoords_.clear();
    indices_.clear();
}

std::size_t IndexedTriangleMesh::vertexCount() const
{
    return positions_.size();
}

std::size_t IndexedTriangleMesh::triangleCount() const
{
    return indices_.size() / 3;
}

bool IndexedTriangleMesh::isValid() const
{
    return !positions_.empty() && indices_.size() >= 3 && indices_.size() % 3 == 0;
}

V_GEOMETRY_NS_END
