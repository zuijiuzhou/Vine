
#include <vine/ge/Point2f.h>

#include <cmath>

#include <vine/ge/Vector2f.h>

VI_GE_NS_BEGIN
Point2f::Point2f()
  : Point2f(0., 0.) {
}
Point2f::Point2f(float xx, float yy)
  : x(xx)
  , y(yy) {
}
const Vector2f& Point2f::asVector() const {
    return reinterpret_cast<const Vector2f&>(*this);
}
Vector2f Point2f::toVector() const {
    return Vector2f(x, y);
}
float Point2f::distanceTo(const Point2f& pt) const {
    return sqrt(x * pt.x + y * pt.y);
}
bool Point2f::equals(const Point2f& other, float eps) const noexcept {
    return ge::isEqual(x, other.x, eps) && ge::isEqual(y, other.y, eps);
}
bool Point2f::operator==(const Point2f& right) const {
    return x == right.x && y == right.y;
}
bool Point2f::operator!=(const Point2f& right) const {
    return !(*this == right);
}
Vector2f Point2f::operator-(const Point2f& right) const {
    return Vector2f(x - right.x, y - right.y);
}
Point2f Point2f::operator+(const Vector2f& right) const {
    return Point2f(x + right.x, y + right.y);
}
Point2f& Point2f::operator+=(const Vector2f& right) {
    x += right.x;
    y += right.y;
    return *this;
}
Point2f& Point2f::operator-=(const Vector2f& right) {
    x -= right.x;
    y -= right.y;
    return *this;
}
VI_GE_NS_END