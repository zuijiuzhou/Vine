#include <vine/ge/Vector4d.h>

#include <stdexcept>

#include <vine/ge/Ge.h>
#include <vine/ge/Vector3d.h>

#include <cmath>

VI_GE_NS_BEGIN
Vector4d::Vector4d()
  : Vector4d(0., 0., 0., 0.) {
}
Vector4d::Vector4d(const Vector3d& v, double ww)
  : x(v.x)
  , y(v.y)
  , z(v.z)
  , w(ww) {
}
Vector4d::Vector4d(double xx, double yy, double zz, double ww)
  : x(xx)
  , y(yy)
  , z(zz)
  , w(ww) {
}

double Vector4d::dotProduct(const Vector4d& vec) const noexcept {
    return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
}

double Vector4d::normalize() noexcept {
    double len = length();
    if (len > EPSD) {
        *this /= len;
    }
    return len;
}

bool Vector4d::setLength(double len) noexcept {
    auto cur_len = length();
    if (isZero(cur_len)) return false;

    *this *= (len / cur_len);
    return true;
}

void Vector4d::set(double xx, double yy, double zz, double ww) noexcept {
    x = xx;
    y = yy;
    z = zz;
    w = ww;
}

void Vector4d::get(double& xx, double& yy, double& zz, double& ww) const noexcept {
    xx = x;
    yy = y;
    zz = z;
    ww = w;
}

double Vector4d::angleTo(const Vector4d& vec) const noexcept {
    if (isZero() || vec.isZero()) return 0.;
    auto v = *this * vec / length() * vec.length();
    if (v > 1.)
        v = 1.;
    else if (v < -1.)
        v = -1.;
    return acos(v);
}

double Vector4d::length() const noexcept {
    return sqrt(x * x + y * y + z * z + w * w);
}

double Vector4d::lengthSqrd() const noexcept {
    return x * x + y * y + z * z + w * w;
}

bool Vector4d::isZero(double eps) const noexcept {
    return ge::isZero(x, eps) && ge::isZero(y, eps) && ge::isZero(z, eps) && ge::isZero(w, eps);
}

bool Vector4d::isParalleTo(const Vector4d& vec, double eps) const noexcept {
    if (isZero(eps) || vec.isZero(eps)) return true;
#pragma message("[not impl] Vector4d::isParalleTo")
    return false;
}

bool Vector4d::isPerpendicularTo(const Vector4d& vec, double eps) const noexcept {
    if (isZero(eps) || vec.isZero(eps)) return true;
    return ge::isZero(dotProduct(vec), eps);
}

bool Vector4d::isNormalized(double eps) const noexcept {
    return ge::isEqual(length(), 1.0, eps);
}

bool Vector4d::equals(const Vector4d& other, double eps) const {
    return ge::isEqual(x, other.x, eps) && ge::isEqual(y, other.y, eps) && ge::isEqual(z, other.z, eps);
}

const Vector3d& Vector4d::asVector3d() const noexcept {
    return reinterpret_cast<const Vector3d&>(*this);
}

bool Vector4d::operator==(const Vector4d& right) const noexcept {
    return x == right.x && y == right.y && z == right.z;
}

bool Vector4d::operator!=(const Vector4d& right) const noexcept {
    return !(*this == right);
}

Vector4d Vector4d::operator-(const Vector4d& right) const noexcept {
    return Vector4d(x - right.x, y - right.y, z - right.z, w - right.w);
}

Vector4d Vector4d::operator+(const Vector4d& right) const noexcept {
    return Vector4d(x + right.x, y + right.y, z + right.z, w + right.w);
}

Vector4d& Vector4d::operator+=(const Vector4d& right) noexcept {
    x += right.x;
    y += right.y;
    z += right.z;
    w += right.w;
    return *this;
}

Vector4d& Vector4d::operator-=(const Vector4d& right) noexcept {
    x -= right.x;
    y -= right.y;
    z -= right.z;
    z -= right.w;
    return *this;
}

Vector4d Vector4d::operator*(double scale) const noexcept {
    Vector4d v(*this);
    v.x *= scale;
    v.y *= scale;
    v.z *= scale;
    v.w *= scale;
    return v;
}

Vector4d& Vector4d::operator*=(double scale) noexcept {
    x *= scale;
    y *= scale;
    z *= scale;
    w *= scale;
    return *this;
}

Vector4d Vector4d::operator/(double scale) const {
    Vector4d v(*this);
    v.x /= scale;
    v.y /= scale;
    v.z /= scale;
    v.w /= scale;
    return v;
}

Vector4d& Vector4d::operator/=(double scale) {
    x /= scale;
    y /= scale;
    z /= scale;
    w /= scale;
    return *this;
}

double Vector4d::operator*(const Vector4d& vec) const noexcept {
    return dotProduct(vec);
}

double& Vector4d::operator[](size_t index) {
    if (index > 3) throw std::out_of_range("index out of range");
    return *(&x + index);
}
VI_GE_NS_END