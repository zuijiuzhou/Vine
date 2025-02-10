#include <vine/ge/Vector3d.h>

#include <stdexcept>

#include <vine/ge/Ge.h>
#include <vine/ge/Point3d.h>
#include <vine/ge/Vector2d.h>

#include <cmath>

VI_GE_NS_BEGIN
Vector3d::Vector3d()
  : Vector3d(0., 0., 0.) {
}
Vector3d::Vector3d(const Vector2d& v, double zz)
  : x(v.x)
  , y(v.y)
  , z(zz) {
}
Vector3d::Vector3d(double xx, double yy, double zz)
  : x(xx)
  , y(yy)
  , z(zz) {
}

double Vector3d::dotProduct(const Vector3d& vec) const noexcept {
    return x * vec.x + y * vec.y + z * vec.z;
}

Vector3d Vector3d::crossProduct(const Vector3d& vec) const noexcept {
    Vector3d v;
    v.x = y * vec.z - z * vec.y;
    v.y = z * vec.x - x * vec.z;
    v.z = x * vec.y - y * vec.x;
    return v;
}

double Vector3d::normalize() noexcept {
    double len = length();
    if (len > EPSD) {
        *this /= len;
    }
    return len;
}

Vector3d Vector3d::normalized() const noexcept {
    auto v = *this;
    v.normalize();
    return v;
}

Vector3d Vector3d::perpVector() const noexcept {
    if (ge::isZero(x, EPSD) || ge::isZero(y, EPSD))
        return Vector3d(z, 0., -x);
    else
        return Vector3d(y, -x, 0.);
}

bool Vector3d::setLength(double len) noexcept {
    auto cur_len = length();
    if (isZero(cur_len)) return false;

    *this *= (len / cur_len);
    return true;
}

void Vector3d::set(double xx, double yy, double zz) noexcept {
    x = xx;
    y = yy;
    z = zz;
}

void Vector3d::get(double& xx, double& yy, double& zz) const noexcept {
    xx = x;
    yy = y;
    zz = z;
}

double Vector3d::angleTo(const Vector3d& vec) const noexcept {
    if (isZero() || vec.isZero()) return 0.;
    auto v = *this * vec / length() * vec.length();
    if (v > 1.)
        v = 1.;
    else if (v < -1.)
        v = -1.;
    return acos(v);
}

double Vector3d::angleTo(const Vector3d& vec, const Vector3d& refVec) const noexcept {
    auto rad = angleTo(vec);
    if ((*this ^ vec) * refVec > 0.) {
        return rad;
    }
    return PIPI - rad;
}

double Vector3d::length() const noexcept {
    return sqrt(x * x + y * y + z * z);
}

double Vector3d::lengthSqrd() const noexcept {
    return x * x + y * y + z * z;
}

bool Vector3d::isZero(double eps) const noexcept {
    return ge::isZero(x, eps) && ge::isZero(y, eps) && ge::isZero(z, eps);
}

bool Vector3d::isParalleTo(const Vector3d& vec, double eps) const noexcept {
    if (isZero(eps) || vec.isZero(eps)) return true;
    return crossProduct(vec).isZero(eps);
}

bool Vector3d::isPerpendicularTo(const Vector3d& vec, double eps) const noexcept {
    if (isZero(eps) || vec.isZero(eps)) return true;
    return ge::isEqual(angleTo(vec), ge::PI_2, eps);
}

bool Vector3d::isNormalized(double eps) const noexcept {
    return ge::isEqual(length(), 1.0, eps);
}

bool Vector3d::equals(const Vector3d& other, double eps) const {
    return ge::isEqual(x, other.x, eps) && ge::isEqual(y, other.y, eps) && ge::isEqual(z, other.z, eps);
}

Point3d Vector3d::toPoint() const noexcept {
    return Point3d(x, y, z);
}

const Point3d& Vector3d::asPoint() const noexcept {
    return reinterpret_cast<const Point3d&>(*this);
}

const Vector2d& Vector3d::asVector2d() const noexcept {
    return reinterpret_cast<const Vector2d&>(*this);
}

bool Vector3d::operator==(const Vector3d& right) const noexcept {
    return x == right.x && y == right.y && z == right.z;
}

bool Vector3d::operator!=(const Vector3d& right) const noexcept {
    return !(*this == right);
}

Vector3d Vector3d::operator-(const Vector3d& right) const noexcept {
    return Vector3d(x - right.x, y - right.y, z - right.z);
}

Vector3d Vector3d::operator+(const Vector3d& right) const noexcept {
    return Vector3d(x + right.x, y + right.y, z + right.z);
}

Vector3d& Vector3d::operator+=(const Vector3d& right) noexcept {
    x += right.x;
    y += right.y;
    z += right.z;
    return *this;
}

Vector3d& Vector3d::operator-=(const Vector3d& right) noexcept {
    x -= right.x;
    y -= right.y;
    z -= right.z;
    return *this;
}

Vector3d Vector3d::operator*(double scale) const noexcept {
    Vector3d v(*this);
    v.x *= scale;
    v.y *= scale;
    v.z *= scale;
    return v;
}

Vector3d& Vector3d::operator*=(double scale) noexcept {
    x *= scale;
    y *= scale;
    z *= scale;
    return *this;
}

Vector3d Vector3d::operator/(double scale) const {
    Vector3d v(*this);
    v.x /= scale;
    v.y /= scale;
    v.z /= scale;
    return v;
}

Vector3d& Vector3d::operator/=(double scale) {
    x /= scale;
    y /= scale;
    z /= scale;
    return *this;
}

Vector3d Vector3d::operator^(const Vector3d& vec) const noexcept {
    return crossProduct(vec);
}

double Vector3d::operator*(const Vector3d& vec) const noexcept {
    return dotProduct(vec);
}

double& Vector3d::operator[](size_t index) {
    if (index > 2) throw std::out_of_range("index out of range");
    return *(&x + index);
}
VI_GE_NS_END