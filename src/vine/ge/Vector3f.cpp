#include <vine/ge/Vector3f.h>

#include <stdexcept>

#include <vine/ge/Ge.h>
#include <vine/ge/Matrixd4x4.h>
#include <vine/ge/Point3f.h>
#include <vine/ge/Vector2f.h>

#include <cmath>

VI_GE_NS_BEGIN
Vector3f::Vector3f()
  : Vector3f(0., 0., 0.) {
}
Vector3f::Vector3f(const Vector2f& v, float zz)
  : x(v.x)
  , y(v.y)
  , z(zz) {
}
Vector3f::Vector3f(float xx, float yy, float zz)
  : x(xx)
  , y(yy)
  , z(zz) {
}

float Vector3f::dotProduct(const Vector3f& vec) const noexcept {
    return x * vec.x + y * vec.y + z * vec.z;
}

Vector3f Vector3f::crossProduct(const Vector3f& vec) const noexcept {
    Vector3f v;
    v.x = y * vec.z - z * vec.y;
    v.y = z * vec.x - x * vec.z;
    v.z = x * vec.y - y * vec.x;
    return v;
}

float Vector3f::normalize() noexcept {
    float len = length();
    if (len > EPSF) {
        *this /= len;
    }
    return len;
}

Vector3f Vector3f::normalized() const noexcept {
    auto v = *this;
    v.normalize();
    return v;
}

Vector3f Vector3f::perpVector() const noexcept {
    if (ge::isZero(x, EPSF) || ge::isZero(y, EPSF))
        return Vector3f(z, 0., -x);
    else
        return Vector3f(y, -x, 0.);
}

bool Vector3f::setLength(float len) noexcept {
    auto cur_len = length();
    if (isZero(cur_len)) return false;

    *this *= (len / cur_len);
    return true;
}

void Vector3f::set(float xx, float yy, float zz) noexcept {
    x = xx;
    y = yy;
    z = zz;
}

void Vector3f::get(float& xx, float& yy, float& zz) const noexcept {
    xx = x;
    yy = y;
    zz = z;
}

void Vector3f::rotate(const Matrixd4x4& mat) {
}

float Vector3f::angleTo(const Vector3f& vec) const noexcept {
    if (isZero() || vec.isZero()) return 0.;
    auto v = *this * vec / length() * vec.length();
    if (v > 1.)
        v = 1.;
    else if (v < -1.)
        v = -1.;
    return acos(v);
}

float Vector3f::angleTo(const Vector3f& vec, const Vector3f& refVec) const noexcept {
    auto rad = angleTo(vec);
    if ((*this ^ vec) * refVec > 0.) {
        return rad;
    }
    return PIPI - rad;
}

float Vector3f::length() const noexcept {
    return sqrt(x * x + y * y + z * z);
}

float Vector3f::lengthSqrd() const noexcept {
    return x * x + y * y + z * z;
}

bool Vector3f::isZero(float eps) const noexcept {
    return ge::isZero(x, eps) && ge::isZero(y, eps) && ge::isZero(z, eps);
}

bool Vector3f::isParalleTo(const Vector3f& vec, float eps) const noexcept {
    if (isZero(eps) || vec.isZero(eps)) return true;
    return crossProduct(vec).isZero(eps);
}

bool Vector3f::isPerpendicularTo(const Vector3f& vec, float eps) const noexcept {
    if (isZero(eps) || vec.isZero(eps)) return true;
    return ge::isEqual(angleTo(vec), static_cast<float>(ge::PI_2), eps);
}

bool Vector3f::isNormalized(float eps) const noexcept {
    return ge::isEqual(length(), 1.0f, eps);
}

bool Vector3f::equals(const Vector3f& other, float eps) const {
    return ge::isEqual(x, other.x, eps) && ge::isEqual(y, other.y, eps) && ge::isEqual(z, other.z, eps);
}

Point3f Vector3f::toPoint() const noexcept {
    return Point3f(x, y, z);
}

const Point3f& Vector3f::asPoint() const noexcept {
    return reinterpret_cast<const Point3f&>(*this);
}

const Vector2f& Vector3f::asVector2f() const noexcept {
    return reinterpret_cast<const Vector2f&>(*this);
}

bool Vector3f::operator==(const Vector3f& right) const noexcept {
    return x == right.x && y == right.y && z == right.z;
}

bool Vector3f::operator!=(const Vector3f& right) const noexcept {
    return !(*this == right);
}

Vector3f Vector3f::operator-(const Vector3f& right) const noexcept {
    return Vector3f(x - right.x, y - right.y, z - right.z);
}

Vector3f Vector3f::operator+(const Vector3f& right) const noexcept {
    return Vector3f(x + right.x, y + right.y, z + right.z);
}

Vector3f& Vector3f::operator+=(const Vector3f& right) noexcept {
    x += right.x;
    y += right.y;
    z += right.z;
    return *this;
}

Vector3f& Vector3f::operator-=(const Vector3f& right) noexcept {
    x -= right.x;
    y -= right.y;
    z -= right.z;
    return *this;
}

Vector3f Vector3f::operator*(float scale) const noexcept {
    Vector3f v(*this);
    v.x *= scale;
    v.y *= scale;
    v.z *= scale;
    return v;
}

Vector3f& Vector3f::operator*=(float scale) noexcept {
    x *= scale;
    y *= scale;
    z *= scale;
    return *this;
}

Vector3f Vector3f::operator/(float scale) const {
    Vector3f v(*this);
    v.x /= scale;
    v.y /= scale;
    v.z /= scale;
    return v;
}

Vector3f& Vector3f::operator/=(float scale) {
    x /= scale;
    y /= scale;
    z /= scale;
    return *this;
}

Vector3f Vector3f::operator^(const Vector3f& vec) const noexcept {
    return crossProduct(vec);
}

float Vector3f::operator*(const Vector3f& vec) const noexcept {
    return dotProduct(vec);
}

float& Vector3f::operator[](size_t index) {
    if (index > 2) throw std::out_of_range("index out of range");
    return *(&x + index);
}
VI_GE_NS_END