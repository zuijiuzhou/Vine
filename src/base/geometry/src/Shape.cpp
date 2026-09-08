#include <vine/geometry/Shape.hpp>

V_GEOMETRY_NS_BEGIN

V_OBJECT_META_IMPL(Shape, vine::Object)

Shape::Shape()
{}

bool Shape::isValid() const
{
    // A shape whose type was never assigned is not usable.
    return shape_type_ != ShapeType::Unknown;
}

bool Shape::hasVolume(double eps) const
{
    (void)eps;  // reserved for mesh volume significance checks.
    switch (shape_type_)
    {
    case ShapeType::Box:
    case ShapeType::Cylinder:
    case ShapeType::Cone:
    case ShapeType::Sphere:
    case ShapeType::Ellipsoid:
        return true;
    default:
        return false;
    }
}

ShapeKind Shape::shapeKind() const
{
    switch (shape_type_)
    {
    case ShapeType::Box:
    case ShapeType::Cylinder:
    case ShapeType::Cone:
    case ShapeType::Sphere:
    case ShapeType::Ellipsoid:
        return ShapeKind::Primitive;
    case ShapeType::TriangleMesh:
    case ShapeType::IndexedTriangleMesh:
        return ShapeKind::Mesh;
    case ShapeType::Brep:
        return ShapeKind::Brep;
    default:
        return ShapeKind::Unknown;
    }
}

V_GEOMETRY_NS_END