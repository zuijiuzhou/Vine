#include <vine/ge/Vector3.h>

#include <cassert>
#include <stdexcept>


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

TMPL_PREFIX void Vector3<T>::set(T xx, T yy, T zz) {
    x = xx;
    y = yy;
    z = zz;
}
TMPL_PREFIX void Vector3<T>::get(T& xx, T& yy, T& zz) const {
    xx = x;
    yy = y;
    zz = z;
}

TMPL_PREFIX Point3<T> Vector3<T>::toPoint() const {
    return Point3<T>(x, y, z);
}
TMPL_PREFIX const Point3<T>& Vector3<T>::asPoint() const {
    return reinterpret_cast<const Point3<T>&>(*this);
}
TMPL_PREFIX const Vector2<T>& Vector3<T>::asVector2() const {
    return reinterpret_cast<const Vector2<T>&>(*this);
}
TMPL_PREFIX bool Vector3<T>::isZero() const {
    return x == T(0) && y == T(0) && z == T(0);
}
TMPL_PREFIX bool Vector3<T>::operator==(const Vector3<T>& right) const {
    return x == right.x && y == right.y && z == right.z;
}
TMPL_PREFIX bool Vector3<T>::operator!=(const Vector3<T>& right) const {
    return !(*this == right);
}
TMPL_PREFIX Vector3<T> Vector3<T>::operator+(const Vector3<T>& right) const {
    return Vector3<T>(x + right.x, y + right.y, z + right.z);
}
TMPL_PREFIX Vector3<T> Vector3<T>::operator-(const Vector3<T>& right) const {
    return Vector3<T>(x - right.x, y - right.y, z - right.z);
}
TMPL_PREFIX Vector3<T> Vector3<T>::operator*(T scale) const {
    Vector3<T> v(*this);
    v.x *= scale;
    v.y *= scale;
    v.z *= scale;
    return v;
}
TMPL_PREFIX Vector3<T> Vector3<T>::operator/(T scale) const {
    Vector3<T> v(*this);
    v.x /= scale;
    v.y /= scale;
    v.z /= scale;
    return v;
}
TMPL_PREFIX Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& right) {
    x += right.x;
    y += right.y;
    z += right.z;
    return *this;
}
TMPL_PREFIX Vector3<T>& Vector3<T>::operator-=(const Vector3<T>& right) {
    x -= right.x;
    y -= right.y;
    z -= right.z;
    return *this;
}
TMPL_PREFIX Vector3<T>& Vector3<T>::operator*=(T scale) {
    x *= scale;
    y *= scale;
    z *= scale;
    return *this;
}
TMPL_PREFIX Vector3<T>& Vector3<T>::operator/=(T scale) {
    x /= scale;
    y /= scale;
    z /= scale;
    return *this;
}
TMPL_PREFIX T& Vector3<T>::operator[](size_t index) {
    assert(index < 3);
    return *(&x + index);
}
TMPL_PREFIX const T& Vector3<T>::operator[](size_t index) const {
    assert(index < 3);
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