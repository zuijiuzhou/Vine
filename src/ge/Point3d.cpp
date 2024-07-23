#include <ge/Line.h>
#include <ge/Point3d.h>
#include <ge/Vector3d.h>

VI_GE_NS_BEGIN
Point3d::Point3d()
  : Point3d(0., 0., 0.) {
}
Point3d::Point3d(double xx, double yy, double zz)
  : x(xx)
  , y(yy)
  , z(zz) {
}

double Point3d::distanceTo(const Point3d& pt) const {
    return (*this - pt).length();
}
double Point3d::distanceTo(const Line3d& pt) const {
    return 0.0;
}
double Point3d::distanceTo(const Plane& pt) const {
    return 0.0;
}

bool Point3d::equals(const Point3d& other, double epsl) const noexcept {
    return ge::isEqual(x, other.x, epsl) && ge::isEqual(y, other.y, epsl) && ge::isEqual(z, other.z, epsl);
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