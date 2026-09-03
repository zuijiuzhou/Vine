#include <vine/graphics/Geometry.hpp>

#include <vine/geometry/IndexedTriangleMesh.hpp>
#include <vine/geometry/TriangleMesh.hpp>
#include <vine/math/Point3.hpp>
#include <vine/math/Vector3.hpp>

V_GRAPHICS_NS_BEGIN

using vine::math::Vec3d;

V_OBJECT_META_IMPL(Geometry, Drawable);

Geometry::Geometry() = default;

Geometry::~Geometry() = default;

raw_ptr<const vine::geometry::Shape> Geometry::shape() const
{
    return shape_.get();
}

void Geometry::setShape(intrusive_ptr<vine::geometry::Shape> shape)
{
    shape_ = std::move(shape);
}

std::size_t Geometry::triangleCount() const
{
    const auto* shape = shape_.get();
    if (shape == nullptr) {
        return 0;
    }
    switch (shape->shapeType()) {
        case vine::geometry::ShapeType::TriangleMesh: {
            const auto* mesh = dynamic_cast<const vine::geometry::TriangleMesh*>(shape);
            return mesh ? mesh->triangleCount() : 0;
        }
        case vine::geometry::ShapeType::IndexedTriangleMesh: {
            const auto* mesh =
                dynamic_cast<const vine::geometry::IndexedTriangleMesh*>(shape);
            return mesh ? mesh->triangleCount() : 0;
        }
        default:
            return 0;
    }
}

std::size_t Geometry::vertexCount() const
{
    const auto* shape = shape_.get();
    if (shape == nullptr) {
        return 0;
    }
    switch (shape->shapeType()) {
        case vine::geometry::ShapeType::TriangleMesh: {
            const auto* mesh = dynamic_cast<const vine::geometry::TriangleMesh*>(shape);
            return mesh ? mesh->vertexCount() : 0;
        }
        case vine::geometry::ShapeType::IndexedTriangleMesh: {
            const auto* mesh =
                dynamic_cast<const vine::geometry::IndexedTriangleMesh*>(shape);
            return mesh ? mesh->vertexCount() : 0;
        }
        default:
            return 0;
    }
}

Aabbd Geometry::computeBoundingBox() const
{
    const auto* shape = shape_.get();
    if (shape == nullptr) {
        return Aabbd::empty();
    }
    switch (shape->shapeType()) {
        case vine::geometry::ShapeType::TriangleMesh:
        case vine::geometry::ShapeType::IndexedTriangleMesh: {
            const auto* mesh = dynamic_cast<const vine::geometry::Mesh*>(shape);
            if (mesh == nullptr) {
                return Aabbd::empty();
            }
            // Prefer the mesh's cached AABB (set via computeAabb()/setAabb()).
            const auto& aabb = mesh->aabb();
            if (aabb.isValid()) {
                const auto mn = aabb.min();
                const auto mx = aabb.max();
                return Aabbd(mn.x, mn.y, mn.z, mx.x, mx.y, mx.z);
            }
            // No cache yet: scan the stored positions once.
            const auto& positions = mesh->positions();
            if (positions.empty()) {
                return Aabbd::empty();
            }
            Aabbd box = Aabbd::empty();
            for (const auto& v : positions) {
                box.expandBy(Vec3d(v.x, v.y, v.z));
            }
            return box;
        }
        default:
            return Aabbd::empty();
    }
}

V_GRAPHICS_NS_END
