#include <ge/Point2d.h>
#include <ge/Vector2d.h>
#include <cmath>

ETD_GE_NS_BEGIN
Point2d::Point2d() : Point2d(0., 0.)
{
}
Point2d::Point2d(double xx, double yy)
    : x(xx), y(yy)
{
}
const Vector2d &Point2d::asVector() const
{
    return reinterpret_cast<const Vector2d &>(*this);
}
Vector2d Point2d::toVector() const
{
    return Vector2d(x, y);
}
double Point2d::distanceTo(const Point2d &pt) const
{
    return sqrt(x * pt.x + y * pt.y);
}
double Point2d::distanceTo(const Line2d &line) const
{
    return 0.;
}
bool Point2d::operator==(const Point2d &right) const
{
    return x == right.x && y == right.y;
}
bool Point2d::operator!=(const Point2d &right) const
{
    return !(*this == right);
}
Vector2d Point2d::operator-(const Point2d &right) const
{
    return Vector2d(x - right.x, y - right.y);
}
Point2d Point2d::operator+(const Vector2d &right) const
{
    return Point2d(x + right.x, y + right.y);
}
Point2d &Point2d::operator+=(const Vector2d &right)
{
    x += right.x;
    y += right.y;
    return *this;
}
Point2d &Point2d::operator-=(const Vector2d &right)
{
    x -= right.x;
    y -= right.y;
    return *this;
}
ETD_GE_NS_END