#include <vine/ge/Vector4.h>

#include <cassert>
#include <stdexcept>


#include <vine/ge/Vector3.h>

#include "Comm.h"

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

TMPL_PREFIX void Vector4<T>::set(T xx, T yy, T zz, T ww) {
    x = xx;
    y = yy;
    z = zz;
    w = ww;
}
TMPL_PREFIX void Vector4<T>::get(T& xx, T& yy, T& zz, T& ww) const {
    xx = x;
    yy = y;
    zz = z;
    ww = w;
}
TMPL_PREFIX const Vector3<T>& Vector4<T>::asVector3() const {
    return reinterpret_cast<const Vector3<T>&>(*this);
}
TMPL_PREFIX bool Vector4<T>::isZero() const {
    return x == T(0) && y == T(0) && z == T(0) && w == T(0);
}
TMPL_PREFIX bool Vector4<T>::operator==(const Vector4<T>& right) const {
    return x == right.x && y == right.y && z == right.z;
}
TMPL_PREFIX bool Vector4<T>::operator!=(const Vector4<T>& right) const {
    return !(*this == right);
}
TMPL_PREFIX Vector4<T> Vector4<T>::operator+(const Vector4<T>& right) const {
    return Vector4<T>(x + right.x, y + right.y, z + right.z, w + right.w);
}
TMPL_PREFIX Vector4<T> Vector4<T>::operator-(const Vector4<T>& right) const {
    return Vector4<T>(x - right.x, y - right.y, z - right.z, w - right.w);
}
TMPL_PREFIX Vector4<T> Vector4<T>::operator*(T scale) const {
    Vector4<T> v(*this);
    v.x *= scale;
    v.y *= scale;
    v.z *= scale;
    v.w *= scale;
    return v;
}
TMPL_PREFIX Vector4<T> Vector4<T>::operator/(T scale) const {
    Vector4<T> v(*this);
    v.x /= scale;
    v.y /= scale;
    v.z /= scale;
    v.w /= scale;
    return v;
}
TMPL_PREFIX Vector4<T>& Vector4<T>::operator+=(const Vector4<T>& right) {
    _VI_ASSERT_DOES_NOT_SUPPORT_BOOL
    x += right.x;
    y += right.y;
    z += right.z;
    w += right.w;
    return *this;
}
TMPL_PREFIX Vector4<T>& Vector4<T>::operator-=(const Vector4<T>& right) {
    _VI_ASSERT_DOES_NOT_SUPPORT_BOOL
    x -= right.x;
    y -= right.y;
    z -= right.z;
    z -= right.w;
    return *this;
}
TMPL_PREFIX Vector4<T>& Vector4<T>::operator*=(T scale) {
    _VI_ASSERT_DOES_NOT_SUPPORT_BOOL
    x *= scale;
    y *= scale;
    z *= scale;
    w *= scale;
    return *this;
}
TMPL_PREFIX Vector4<T>& Vector4<T>::operator/=(T scale) {
    _VI_ASSERT_DOES_NOT_SUPPORT_BOOL
    x /= scale;
    y /= scale;
    z /= scale;
    w /= scale;
    return *this;
}
// TMPL_PREFIX Vector4<T>::ValueTypePromoted Vector4<T>::operator*(const Vector4<T>& vec) const  {
//     return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
// }
TMPL_PREFIX T& Vector4<T>::operator[](size_t index) {
    assert(index < 4);
    return *(&x + index);
}
TMPL_PREFIX const T& Vector4<T>::operator[](size_t index) const {
    assert(index < 4);
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