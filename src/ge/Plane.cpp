#include <ge/Plane.h>

ETD_GE_NS_BEGIN
Plane::Plane(Point3d origin, Vector3d normal)
    : origin(origin), normal(normal)
{
}

bool Plane::intersectWith(const Line3d &line, Point3d &intersectionPt, double tol) const
{
    return false;
}
ETD_GE_NS_END