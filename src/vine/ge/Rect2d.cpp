#include <vine/ge/Rect2d.h>

VI_GE_NS_BEGIN

Rect2d::Rect2d() : Rect2d(0, 0, 0, 0)
{
}

Rect2d::Rect2d(const Point2d &top_left, const Vector2d &size)
    : x(top_left.x), y(top_left.y), w(size.x), h(size.y)
{
}

Rect2d::Rect2d(double x, double y, double w, double h)
    : x(x), y(y), w(w), h(h)
{
}

Point2d Rect2d::topLeft() const
{
    return Point2d(x, y);
}
Point2d Rect2d::topRight() const
{
    return Point2d(x + w, y);
}
Point2d Rect2d::bottomLeft() const
{
    return Point2d(x, y + h);
}
Point2d Rect2d::bottomRight() const
{
    return Point2d(x + w, y + h);
}

Vector2d Rect2d::size() const
{

    return Vector2d(w, h);
}

VI_GE_NS_END