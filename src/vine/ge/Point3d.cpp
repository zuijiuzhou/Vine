#include <vine/ge/Point3d.h>

#include <vine/ge/Line.h>
#include <vine/ge/Point2d.h>
#include <vine/ge/Vector3d.h>

VI_GE_NS_BEGIN

Point3d::Point3d()
  : x(0.)
  , y(0.)
  , z(0.) {
}
Point3d::Point3d(const Point2d& pt, double zz)
  : x(pt.x)
  , y(pt.x)
  , z(zz) {
}
Point3d::Point3d(double xx, double yy, double zz)
  : x(xx)
  , y(yy)
  , z(zz) {
}

double Point3d::distanceTo(const Point3d& pt) const {
    return (*this - pt).length();
}

bool Point3d::equals(const Point3d& other, double eps) const noexcept {
    return ge::isEqual(x, other.x, eps) && ge::isEqual(y, other.y, eps) && ge::isEqual(z, other.z, eps);
}
Vector3d Point3d::toVector() const {
    return Vector3d(x, y, z);
}
const Point2d& Point3d::asPoint2d() const {
    return reinterpret_cast<const Point2d&>(*this);
}
const Vector3d& Point3d::asVector() const {
    return reinterpret_cast<const Vector3d&>(*this);
}

bool Point3d::operator==(const Point3d& right) const {
    return x == right.x && y == right.y && z == right.z;
}
bool Point3d::operator!=(const Point3d& right) const {
    return !(*this == right);
}
Vector3d Point3d::operator-(const Point3d& right) const {
    return Vector3d(x - right.x, y - right.y, z - right.z);
}
Point3d Point3d::operator+(const Vector3d& right) const {
    return Point3d(x + right.x, y + right.y, z + right.z);
}
Point3d& Point3d::operator+=(const Vector3d& right) {
    x += right.x;
    y += right.y;
    z += right.z;
    return *this;
}
Point3d& Point3d::operator-=(const Vector3d& right) {
    x -= right.x;
    y -= right.y;
    z -= right.z;
    return *this;
}
VI_GE_NS_END