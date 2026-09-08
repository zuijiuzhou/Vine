#include <vine/geometry/BrepShape.hpp>

V_GEOMETRY_NS_BEGIN

V_OBJECT_META_IMPL(BrepShape, Shape)

BrepShape::BrepShape()
{
    shape_type_ = ShapeType::Brep;
}

BrepShape::~BrepShape() = default;

const TopoDS_Shape* BrepShape::shape() const
{
    return shape_;
}

void BrepShape::setShape(TopoDS_Shape* shape)
{
    shape_ = shape;
}

bool BrepShape::isValid() const
{
    return shape_ != nullptr;
}

V_GEOMETRY_NS_END