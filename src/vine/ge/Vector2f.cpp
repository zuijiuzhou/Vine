#include <cmath>
#include <vine/ge/Point2f.h>
#include <vine/ge/Vector2f.h>

VI_GE_NS_BEGIN
Vector2f::Vector2f()
  : Vector2f(0., 0.) {
}

Vector2f::Vector2f(float xx, float yy)
  : x(xx)
  , y(yy) {
}

float Vector2f::dotProduct(const Vector2f& vec) const {
    return x * vec.x + y * vec.y;
}

float Vector2f::crossProduct(const Vector2f& vec) const {
    return x * vec.y - y * vec.x;
}

void Vector2f::normalize() {
    float len = length();
    if (len == 0) {
        return;
    }
    *this /= len;
}

Vector2f Vector2f::perpVector() const {
    return Vector2f(-y, x);
}

bool Vector2f::setLength(float len) {
    auto cur_len = length();
    if (isZero(cur_len)) return false;

    *this *= (len / cur_len);
    return true;
}

void Vector2f::set(float xx, float yy) {
    x = xx;
    y = yy;
}

void Vector2f::get(float& xx, float& yy) const {
    xx = x;
    yy = y;
}
// void Vector2f::rotate(const Matrixd4x4 &mat)
// {
// }
float Vector2f::angleTo(const Vector2f& vec) const {
    if (isZero() || vec.isZero()) return 0.;
    auto cos = (*this * vec) / (length() * vec.length());
    if (cos > 1.0)
        cos = 1.;
    else if (cos < -1.)
        cos = 1.;
    return acos(cos);
}

float Vector2f::length() const {
    return sqrt(x * x + y * y);
}

float Vector2f::lengthSqrd() const {
    return x * x + y * y;
}

bool Vector2f::isZero(float eps) const {
    return ge::isEqual(x, 0.f, eps) && ge::isEqual(y, 0.f, eps);
}

bool Vector2f::isParalleTo(const Vector2f& vec, float eps) const {
    if (isZero(eps) || vec.isZero(eps)) return true;
    return ge::isZero(crossProduct(vec), eps);
}

bool Vector2f::isPerpendicularTo(const Vector2f& vec, float eps) const {
    if (isZero(eps) || vec.isZero(eps)) return true;
    return ge::isZero(dotProduct(vec), eps);
}

bool Vector2f::isNormalized(float eps) const noexcept {
    return ge::isEqual(length(), 1.0f, eps);
}

Point2f Vector2f::toPoint() const {
    return Point2f(x, y);
}

const Point2f& Vector2f::asPoint() {
    return reinterpret_cast<const Point2f&>(*this);
}

bool Vector2f::equals(const Vector2f& other, float eps) const {
    return ge::isEqual(x, other.x, eps) && ge::isEqual(y, other.y, eps);
}

bool Vector2f::operator==(const Vector2f& right) const {
    return x == right.x && y == right.y;
}

bool Vector2f::operator!=(const Vector2f& right) const {
    return !(*this == right);
}

Vector2f Vector2f::operator-(const Vector2f& right) const {
    return Vector2f(x - right.x, y - right.y);
}

Vector2f Vector2f::operator+(const Vector2f& right) const {
    return Vector2f(x + right.x, y + right.y);
}

Vector2f& Vector2f::operator+=(const Vector2f& right) {
    x += right.x;
    y += right.y;
    return *this;
}

Vector2f& Vector2f::operator-=(const Vector2f& right) {
    x -= right.x;
    y -= right.y;
    return *this;
}

Vector2f Vector2f::operator*(float scale) const {
    Vector2f v(*this);
    v.x *= scale;
    v.y *= scale;
    return v;
}

Vector2f& Vector2f::operator*=(float scale) {
    x *= scale;
    y *= scale;
    return *this;
}

Vector2f Vector2f::operator/(float scale) const {
    Vector2f v(*this);
    v.x /= scale;
    v.y /= scale;
    return v;
}

Vector2f& Vector2f::operator/=(float scale) {
    x /= scale;
    y /= scale;
    return *this;
}

float Vector2f::operator^(const Vector2f& vec) const {
    return crossProduct(vec);
}

float Vector2f::operator*(const Vector2f& vec) const {
    return dotProduct(vec);
}
VI_GE_NS_END