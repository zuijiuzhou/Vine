#include <vine/ge/Line.h>

VI_GE_NS_BEGIN
Line::Line(const Point3d &origin, const Vector3d &direction)
    : origin(origin), direction(direction)
{
}

bool Line::intersectWith(const Line &line, Point3d &intersectionPt, double tol) const
{
    if (line.origin == origin)
    {
        intersectionPt = origin;
        return true;
    }

    return false;
}
VI_GE_NS_END