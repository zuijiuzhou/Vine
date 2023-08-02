#include <ge/Line3d.h>

VINE_GE_NS_BEGIN
Line3d::Line3d(Point3d origin, Vector3d direction)
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
VINE_GE_NS_END