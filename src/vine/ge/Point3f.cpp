#include <vine/ge/Point3f.h>

#include <vine/ge/Point2f.h>
#include <vine/ge/Vector3f.h>

VI_GE_NS_BEGIN
Point3f::Point3f()
  : Point3f(0., 0., 0.) {
}
Point3f::Point3f(float xx, float yy, float zz)
  : x(xx)
  , y(yy)
  , z(zz) {
}
Point3f::Point3f(const Point2f& pt, float zz)
  : x(pt.x)
  , y(pt.x)
  , z(zz) {
}

float Point3f::distanceTo(const Point3f& pt) const {
    return (*this - pt).length();
}

bool Point3f::equals(const Point3f& other, float eps) const noexcept {
    return ge::isEqual(x, other.x, eps) && ge::isEqual(y, other.y, eps) && ge::isEqual(z, other.z, eps);
}

Vector3f Point3f::toVector() const {
    return Vector3f(x, y, z);
}
const Point2f& Point3f::asPoint2f() const {
    return reinterpret_cast<const Point2f&>(*this);
}

const Vector3f& Point3f::asVector() const {
    return reinterpret_cast<const Vector3f&>(*this);
}

bool Point3f::operator==(const Point3f& right) const {
    return x == right.x && y == right.y && z == right.z;
}
bool Point3f::operator!=(const Point3f& right) const {
    return !(*this == right);
}
Vector3f Point3f::operator-(const Point3f& right) const {
    return Vector3f(x - right.x, y - right.y, z - right.z);
}
Point3f Point3f::operator+(const Vector3f& right) const {
    return Point3f(x + right.x, y + right.y, z + right.z);
}
Point3f& Point3f::operator+=(const Vector3f& right) {
    x += right.x;
    y += right.y;
    z += right.z;
    return *this;
}
Point3f& Point3f::operator-=(const Vector3f& right) {
    x -= right.x;
    y -= right.y;
    z -= right.z;
    return *this;
}
VI_GE_NS_END