#include <vine/ge/Vector4f.h>

#include <stdexcept>

#include <vine/ge/Ge.h>
#include <vine/ge/Vector3f.h>

#include <cmath>

VI_GE_NS_BEGIN
Vector4f::Vector4f()
  : Vector4f(0., 0., 0., 0.) {
}
Vector4f::Vector4f(const Vector3f& v, float ww)
  : x(v.x)
  , y(v.y)
  , z(v.z)
  , w(ww) {
}
Vector4f::Vector4f(float xx, float yy, float zz, float ww)
  : x(xx)
  , y(yy)
  , z(zz)
  , w(ww) {
}

float Vector4f::dotProduct(const Vector4f& vec) const noexcept {
    return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
}

float Vector4f::normalize() noexcept {
    float len = length();
    if (len > EPSF) {
        *this /= len;
    }
    return len;
}

bool Vector4f::setLength(float len) noexcept {
    auto cur_len = length();
    if (isZero(cur_len)) return false;

    *this *= (len / cur_len);
    return true;
}

void Vector4f::set(float xx, float yy, float zz, float ww) noexcept {
    x = xx;
    y = yy;
    z = zz;
    w = ww;
}

void Vector4f::get(float& xx, float& yy, float& zz, float& ww) const noexcept {
    xx = x;
    yy = y;
    zz = z;
    ww = w;
}

float Vector4f::angleTo(const Vector4f& vec) const noexcept {
    if (isZero() || vec.isZero()) return 0.;
    auto v = *this * vec / length() * vec.length();
    if (v > 1.)
        v = 1.;
    else if (v < -1.)
        v = -1.;
    return acos(v);
}

float Vector4f::length() const noexcept {
    return sqrt(x * x + y * y + z * z + w * w);
}

float Vector4f::lengthSqrd() const noexcept {
    return x * x + y * y + z * z + w * w;
}

bool Vector4f::isZero(float eps) const noexcept {
    return ge::isZero(x, eps) && ge::isZero(y, eps) && ge::isZero(z, eps) && ge::isZero(w, eps);
}

bool Vector4f::isParalleTo(const Vector4f& vec, float eps) const noexcept {
    if (isZero(eps) || vec.isZero(eps)) return true;
#pragma message("[not impl] Vector4f::isParalleTo")
    return false;
}

bool Vector4f::isPerpendicularTo(const Vector4f& vec, float eps) const noexcept {
    if (isZero(eps) || vec.isZero(eps)) return true;
    return ge::isZero(dotProduct(vec), eps);
}

bool Vector4f::isNormalized(float eps) const noexcept {
    return ge::isEqual(length(), 1.0f, eps);
}

bool Vector4f::equals(const Vector4f& other, float eps) const {
    return ge::isEqual(x, other.x, eps) && ge::isEqual(y, other.y, eps) && ge::isEqual(z, other.z, eps);
}

const Vector3f& Vector4f::asVector3f() const noexcept {
    return reinterpret_cast<const Vector3f&>(*this);
}

bool Vector4f::operator==(const Vector4f& right) const noexcept {
    return x == right.x && y == right.y && z == right.z;
}

bool Vector4f::operator!=(const Vector4f& right) const noexcept {
    return !(*this == right);
}

Vector4f Vector4f::operator-(const Vector4f& right) const noexcept {
    return Vector4f(x - right.x, y - right.y, z - right.z, w - right.w);
}

Vector4f Vector4f::operator+(const Vector4f& right) const noexcept {
    return Vector4f(x + right.x, y + right.y, z + right.z, w + right.w);
}

Vector4f& Vector4f::operator+=(const Vector4f& right) noexcept {
    x += right.x;
    y += right.y;
    z += right.z;
    w += right.w;
    return *this;
}

Vector4f& Vector4f::operator-=(const Vector4f& right) noexcept {
    x -= right.x;
    y -= right.y;
    z -= right.z;
    z -= right.w;
    return *this;
}

Vector4f Vector4f::operator*(float scale) const noexcept {
    Vector4f v(*this);
    v.x *= scale;
    v.y *= scale;
    v.z *= scale;
    v.w *= scale;
    return v;
}

Vector4f& Vector4f::operator*=(float scale) noexcept {
    x *= scale;
    y *= scale;
    z *= scale;
    w *= scale;
    return *this;
}

Vector4f Vector4f::operator/(float scale) const {
    Vector4f v(*this);
    v.x /= scale;
    v.y /= scale;
    v.z /= scale;
    v.w /= scale;
    return v;
}

Vector4f& Vector4f::operator/=(float scale) {
    x /= scale;
    y /= scale;
    z /= scale;
    w /= scale;
    return *this;
}

float Vector4f::operator*(const Vector4f& vec) const noexcept {
    return dotProduct(vec);
}

float& Vector4f::operator[](size_t index) {
    if (index > 3) throw std::out_of_range("index out of range");
    return *(&x + index);
}
VI_GE_NS_END