#include <ge/Line2d.h>

VI_GE_NS_BEGIN
Line2d::Line2d(const Point2d& origin, const Vector2d& direction)
    : m_origin(origin), m_direction(direction)
{
}

Point2d Line2d::origin() const
{
    return m_origin;
}

Vector2d Line2d::direction() const
{
    return m_direction;
}

bool Line2d::intersectWith(const Line2d &line, Point2d &intersectionPt) const
{
    return false;
}
VI_GE_NS_END