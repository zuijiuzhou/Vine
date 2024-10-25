#include <vine/ge/Point2i.h>
#include <vine/ge/Vector2i.h>
#include <cmath>

VI_GE_NS_BEGIN
Point2i::Point2i() : Point2i(0, 0)
{
}
Point2i::Point2i(int xx, int yy)
    : x(xx), y(yy)
{
}
const Vector2i &Point2i::asVector() const
{
    return reinterpret_cast<const Vector2i &>(*this);
}
Vector2i Point2i::toVector() const
{
    return Vector2i(x, y);
}
double Point2i::distanceTo(const Point2i &pt) const
{
    return sqrt(x * pt.x + y * pt.y);
}
bool Point2i::operator==(const Point2i &right) const
{
    return x == right.x && y == right.y;
}
bool Point2i::operator!=(const Point2i &right) const
{
    return !(*this == right);
}
Vector2i Point2i::operator-(const Point2i &right) const
{
    return Vector2i(x - right.x, y - right.y);
}
Point2i Point2i::operator+(const Vector2i &right) const
{
    return Point2i(x + right.x, y + right.y);
}
Point2i &Point2i::operator+=(const Vector2i &right)
{
    x += right.x;
    y += right.y;
    return *this;
}
Point2i &Point2i::operator-=(const Vector2i &right)
{
    x -= right.x;
    y -= right.y;
    return *this;
}
VI_GE_NS_END