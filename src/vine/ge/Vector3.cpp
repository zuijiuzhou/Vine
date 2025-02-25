#include <vine/ge/Vector3.h>

#include <stdexcept>

#include <vine/ge/Ge.h>
#include <vine/ge/Point3.h>
#include <vine/ge/Vector2.h>

#include <cmath>

VI_GE_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Vector3<T>::Vector3()
  : x(0)
  , y(0)
  , z(0) {
}
TMPL_PREFIX Vector3<T>::Vector3(const Vector2<T>& v, T zz)
  : x(v.x)
  , y(v.y)
  , z(zz) {
}
TMPL_PREFIX Vector3<T>::Vector3(T xx, T yy, T zz)
  : x(xx)
  , y(yy)
  , z(zz) {
}

TMPL_PREFIX void Vector3<T>::set(T xx, T yy, T zz) noexcept {
    x = xx;
    y = yy;
    z = zz;
}
TMPL_PREFIX void Vector3<T>::get(T& xx, T& yy, T& zz) const noexcept {
    xx = x;
    yy = y;
    zz = z;
}
// TMPL_PREFIX T Vector3<T>::angleTo(const Vector3<T>& vec) const noexcept {
//     if (isZero() || vec.isZero()) return 0.;
//     auto v = *this * vec / length() * vec.length();
//     if (v > 1.)
//         v = 1.;
//     else if (v < -1.)
//         v = -1.;
//     return acos(v);
// }
// TMPL_PREFIX T Vector3<T>::angleTo(const Vector3<T>& vec, const Vector3<T>& refVec) const noexcept {
//     auto rad = angleTo(vec);
//     if ((*this ^ vec) * refVec > 0.) {
//         return rad;
//     }
//     return PIPI - rad;
// }
TMPL_PREFIX bool Vector3<T>::isZero(T eps) const noexcept {
    return ge::isZero(x, eps) && ge::isZero(y, eps) && ge::isZero(z, eps);
}

TMPL_PREFIX bool Vector3<T>::equals(const Vector3<T>& other, T eps) const {
    return ge::isEqual(x, other.x, eps) && ge::isEqual(y, other.y, eps) && ge::isEqual(z, other.z, eps);
}
TMPL_PREFIX Point3<T> Vector3<T>::toPoint() const noexcept {
    return Point3<T>(x, y, z);
}
TMPL_PREFIX const Point3<T>& Vector3<T>::asPoint() const noexcept {
    return reinterpret_cast<const Point3<T>&>(*this);
}
TMPL_PREFIX const Vector2<T>& Vector3<T>::asVector2() const noexcept {
    return reinterpret_cast<const Vector2<T>&>(*this);
}
TMPL_PREFIX bool Vector3<T>::operator==(const Vector3<T>& right) const noexcept {
    return x == right.x && y == right.y && z == right.z;
}
TMPL_PREFIX bool Vector3<T>::operator!=(const Vector3<T>& right) const noexcept {
    return !(*this == right);
}
TMPL_PREFIX Vector3<T> Vector3<T>::operator-(const Vector3<T>& right) const noexcept {
    return Vector3<T>(x - right.x, y - right.y, z - right.z);
}
TMPL_PREFIX Vector3<T> Vector3<T>::operator+(const Vector3<T>& right) const noexcept {
    return Vector3<T>(x + right.x, y + right.y, z + right.z);
}
TMPL_PREFIX Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& right) noexcept {
    x += right.x;
    y += right.y;
    z += right.z;
    return *this;
}
TMPL_PREFIX Vector3<T>& Vector3<T>::operator-=(const Vector3<T>& right) noexcept {
    x -= right.x;
    y -= right.y;
    z -= right.z;
    return *this;
}
TMPL_PREFIX Vector3<T> Vector3<T>::operator*(T scale) const noexcept {
    Vector3<T> v(*this);
    v.x *= scale;
    v.y *= scale;
    v.z *= scale;
    return v;
}
TMPL_PREFIX Vector3<T>& Vector3<T>::operator*=(T scale) noexcept {
    x *= scale;
    y *= scale;
    z *= scale;
    return *this;
}
TMPL_PREFIX Vector3<T> Vector3<T>::operator/(T scale) const {
    Vector3<T> v(*this);
    v.x /= scale;
    v.y /= scale;
    v.z /= scale;
    return v;
}
TMPL_PREFIX Vector3<T>& Vector3<T>::operator/=(T scale) {
    x /= scale;
    y /= scale;
    z /= scale;
    return *this;
}
// TMPL_PREFIX Vector3<T> Vector3<T>::operator^(const Vector3<T>& vec) const noexcept {
//     return Vector3<T>(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
// }
// TMPL_PREFIX Vector3<T>::ValueTypePromoted Vector3<T>::operator*(const Vector3<T>& vec) const noexcept {
//     return x * vec.x + y * vec.y + z * vec.z;
// }
TMPL_PREFIX T& Vector3<T>::operator[](size_t index) {
    if (index > 2) throw std::out_of_range("Index out of range.");
    return *(&x + index);
}
TMPL_PREFIX const T& Vector3<T>::operator[](size_t index) const {
    if (index > 2) throw std::out_of_range("Index out of range.");
    return *(&x + index);
}
#undef TMPL_PREFIX

template class VI_GE_API Vector3<float>;
template class VI_GE_API Vector3<double>;
template class VI_GE_API Vector3<bool>;
template class VI_GE_API Vector3<int8_t>;
template class VI_GE_API Vector3<uint8_t>;
template class VI_GE_API Vector3<int16_t>;
template class VI_GE_API Vector3<uint16_t>;
template class VI_GE_API Vector3<int32_t>;
template class VI_GE_API Vector3<uint32_t>;
template class VI_GE_API Vector3<int64_t>;
template class VI_GE_API Vector3<uint64_t>;

VI_GE_NS_END