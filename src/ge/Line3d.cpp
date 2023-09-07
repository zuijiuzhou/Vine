#include <ge/Line3d.h>

VI_GE_NS_BEGIN
Line3d::Line3d(const Point3d &origin, const Vector3d &direction)
    : origin(origin), direction(direction)
{
}

bool Line3d::intersectWith(const Line3d &line, Point3d &intersectionPt, double tol) const
{
    if (line.origin == origin)
    {
        intersectionPt = origin;
        return true;
    }

    return false;
}
VI_GE_NS_END