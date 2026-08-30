#include <vine/geometry/TriangleMesh.hpp>

V_GEOMETRY_NS_BEGIN

V_OBJECT_META_IMPL(TriangleMesh, Mesh)

TriangleMesh::TriangleMesh()
{
    shape_type_ = ShapeType::TriangleMesh;
}

const Vec3fArray& TriangleMesh::positions() const
{
    return positions_;
}

const Vec3fArray& TriangleMesh::normals() const
{
    return normals_;
}

const Vec2fArray& TriangleMesh::texcoords() const
{
    return texcoords_;
}

void TriangleMesh::setPositions(Vec3fArray positions)
{
    positions_ = std::move(positions);
}

void TriangleMesh::setNormals(Vec3fArray normals)
{
    normals_ = std::move(normals);
}

void TriangleMesh::setTexcoords(Vec2fArray texcoords)
{
    texcoords_ = std::move(texcoords);
}

void TriangleMesh::addTriangle(const vine::math::Vec3f& a, const vine::math::Vec3f& b, const vine::math::Vec3f& c)
{
    positions_.push_back(a);
    positions_.push_back(b);
    positions_.push_back(c);
}

void TriangleMesh::clear()
{
    positions_.clear();
    normals_.clear();
    texcoords_.clear();
}

std::size_t TriangleMesh::vertexCount() const
{
    return positions_.size();
}

std::size_t TriangleMesh::triangleCount() const
{
    return positions_.size() / 3;
}

bool TriangleMesh::isValid() const
{
    return !positions_.empty() && positions_.size() % 3 == 0;
}

V_GEOMETRY_NS_END
