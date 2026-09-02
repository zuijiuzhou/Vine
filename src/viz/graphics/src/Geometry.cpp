#include <vine/graphics/Geometry.hpp>

#include <vine/geometry/IndexedTriangleMesh.hpp>
#include <vine/geometry/TriangleMesh.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(Geometry, Drawable);

Geometry::Geometry() = default;

Geometry::~Geometry() = default;

const vine::geometry::Shape* Geometry::shape() const
{
    return shape_.get();
}

void Geometry::setShape(vine::geometry::Shape* shape)
{
    shape_ = shape;
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

BoundingBox Geometry::computeBoundingBox() const
{
    const auto* shape = shape_.get();
    if (shape == nullptr) {
        return BoundingBox();
    }
    switch (shape->shapeType()) {
        case vine::geometry::ShapeType::TriangleMesh: {
            const auto* mesh = dynamic_cast<const vine::geometry::TriangleMesh*>(shape);
            if (mesh == nullptr) {
                return BoundingBox();
            }
            BoundingBox box;
            for (const auto& v : mesh->positions()) {
                box.expand(Vec3d(v.x, v.y, v.z));
            }
            return box;
        }
        case vine::geometry::ShapeType::IndexedTriangleMesh: {
            const auto* mesh =
                dynamic_cast<const vine::geometry::IndexedTriangleMesh*>(shape);
            if (mesh == nullptr) {
                return BoundingBox();
            }
            BoundingBox box;
            for (const auto& v : mesh->positions()) {
                box.expand(Vec3d(v.x, v.y, v.z));
            }
            return box;
        }
        default:
            return BoundingBox();
    }
}

V_GRAPHICS_NS_END
