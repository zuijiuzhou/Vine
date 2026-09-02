#include <vine/geometry/Mesh.hpp>

#include <utility>

V_GEOMETRY_NS_BEGIN

V_OBJECT_META_IMPL(Mesh, Shape)

Mesh::Mesh()
{}

const Vec3fArray& Mesh::positions() const
{
    return positions_;
}

const Vec3fArray& Mesh::normals() const
{
    return normals_;
}

const Vec2fArray& Mesh::texcoords() const
{
    return texcoords_;
}

void Mesh::setPositions(Vec3fArray positions)
{
    positions_ = std::move(positions);
}

void Mesh::setNormals(Vec3fArray normals)
{
    normals_ = std::move(normals);
}

void Mesh::setTexcoords(Vec2fArray texcoords)
{
    texcoords_ = std::move(texcoords);
}

std::size_t Mesh::vertexCount() const
{
    return positions_.size();
}

const Aabbf& Mesh::aabb() const
{
    return aabb_;
}

void Mesh::setAabb(const Aabbf& aabb)
{
    aabb_ = aabb;
}

Aabbf Mesh::computeAabb()
{
    aabb_ = Aabbf::compute(positions_);
    return aabb_;
}

void Mesh::clearAttributes()
{
    positions_.clear();
    normals_.clear();
    texcoords_.clear();
    aabb_ = Aabbf::empty();
}

V_GEOMETRY_NS_END
