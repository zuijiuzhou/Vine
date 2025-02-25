#include <vine/ge/Vector4.h>

#include <stdexcept>

#include <vine/ge/Ge.h>
#include <vine/ge/Vector3.h>

VI_GE_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Vector4<T>::Vector4()
  : x(T())
  , y(T())
  , z(T())
  , w(T()) {
}
TMPL_PREFIX Vector4<T>::Vector4(const Vector3<T>& v, T ww)
  : x(v.x)
  , y(v.y)
  , z(v.z)
  , w(ww) {
}
TMPL_PREFIX Vector4<T>::Vector4(T xx, T yy, T zz, T ww)
  : x(xx)
  , y(yy)
  , z(zz)
  , w(ww) {
}

TMPL_PREFIX void Vector4<T>::set(T xx, T yy, T zz, T ww) noexcept {
    x = xx;
    y = yy;
    z = zz;
    w = ww;
}
TMPL_PREFIX void Vector4<T>::get(T& xx, T& yy, T& zz, T& ww) const noexcept {
    xx = x;
    yy = y;
    zz = z;
    ww = w;
}
TMPL_PREFIX bool Vector4<T>::isZero(T eps) const noexcept {
    return ge::isZero(x, eps) && ge::isZero(y, eps) && ge::isZero(z, eps) && ge::isZero(w, eps);
}
TMPL_PREFIX bool Vector4<T>::equals(const Vector4<T>& other, T eps) const {
    return ge::isEqual(x, other.x, eps) && ge::isEqual(y, other.y, eps) && ge::isEqual(z, other.z, eps);
}
TMPL_PREFIX const Vector3<T>& Vector4<T>::asVector3() const noexcept {
    return reinterpret_cast<const Vector3<T>&>(*this);
}
TMPL_PREFIX bool Vector4<T>::operator==(const Vector4<T>& right) const noexcept {
    return x == right.x && y == right.y && z == right.z;
}
TMPL_PREFIX bool Vector4<T>::operator!=(const Vector4<T>& right) const noexcept {
    return !(*this == right);
}
TMPL_PREFIX Vector4<T> Vector4<T>::operator-(const Vector4<T>& right) const noexcept {
    return Vector4<T>(x - right.x, y - right.y, z - right.z, w - right.w);
}
TMPL_PREFIX Vector4<T> Vector4<T>::operator+(const Vector4<T>& right) const noexcept {
    return Vector4<T>(x + right.x, y + right.y, z + right.z, w + right.w);
}
TMPL_PREFIX Vector4<T>& Vector4<T>::operator+=(const Vector4<T>& right) noexcept {
    x += right.x;
    y += right.y;
    z += right.z;
    w += right.w;
    return *this;
}
TMPL_PREFIX Vector4<T>& Vector4<T>::operator-=(const Vector4<T>& right) noexcept {
    x -= right.x;
    y -= right.y;
    z -= right.z;
    z -= right.w;
    return *this;
}
TMPL_PREFIX Vector4<T> Vector4<T>::operator*(T scale) const noexcept {
    Vector4<T> v(*this);
    v.x *= scale;
    v.y *= scale;
    v.z *= scale;
    v.w *= scale;
    return v;
}
TMPL_PREFIX Vector4<T>& Vector4<T>::operator*=(T scale) noexcept {
    x *= scale;
    y *= scale;
    z *= scale;
    w *= scale;
    return *this;
}
TMPL_PREFIX Vector4<T> Vector4<T>::operator/(T scale) const {
    Vector4<T> v(*this);
    v.x /= scale;
    v.y /= scale;
    v.z /= scale;
    v.w /= scale;
    return v;
}
TMPL_PREFIX Vector4<T>& Vector4<T>::operator/=(T scale) {
    x /= scale;
    y /= scale;
    z /= scale;
    w /= scale;
    return *this;
}
//TMPL_PREFIX Vector4<T>::ValueTypePromoted Vector4<T>::operator*(const Vector4<T>& vec) const noexcept {
//    return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
//}
TMPL_PREFIX T& Vector4<T>::operator[](size_t index) {
    if (index > 3) throw std::out_of_range("Index out of range.");
    return *(&x + index);
}
TMPL_PREFIX const T& Vector4<T>::operator[](size_t index) const {
    if (index > 3) throw std::out_of_range("Index out of range.");
    return *(&x + index);
}

#undef TMPL_PREFIX

template class VI_GE_API Vector4<float>;
template class VI_GE_API Vector4<double>;
template class VI_GE_API Vector4<bool>;
template class VI_GE_API Vector4<int8_t>;
template class VI_GE_API Vector4<uint8_t>;
template class VI_GE_API Vector4<int16_t>;
template class VI_GE_API Vector4<uint16_t>;
template class VI_GE_API Vector4<int32_t>;
template class VI_GE_API Vector4<uint32_t>;
template class VI_GE_API Vector4<int64_t>;
template class VI_GE_API Vector4<uint64_t>;

VI_GE_NS_END