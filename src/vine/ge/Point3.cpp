#include <vine/ge/Point3.h>

#include <cstdint>

#include <vine/ge/Point2.h>
#include <vine/ge/Vector3.h>

VI_GE_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Point3<T>::Point3()
  : x(0.)
  , y(0.)
  , z(0.) {
}
TMPL_PREFIX Point3<T>::Point3(const Point2<T>& pt, T zz)
  : x(pt.x)
  , y(pt.x)
  , z(zz) {
}
TMPL_PREFIX Point3<T>::Point3(T xx, T yy, T zz)
  : x(xx)
  , y(yy)
  , z(zz) {
}

TMPL_PREFIX Vector3<T> Point3<T>::toVector() const {
    return Vector3<T>(x, y, z);
}
TMPL_PREFIX const Point2<T>& Point3<T>::asPoint2() const {
    return reinterpret_cast<const Point2<T>&>(*this);
}
TMPL_PREFIX const Vector3<T>& Point3<T>::asVector() const {
    return reinterpret_cast<const Vector3<T>&>(*this);
}
TMPL_PREFIX bool Point3<T>::operator==(const Point3<T>& right) const {
    return x == right.x && y == right.y && z == right.z;
}
TMPL_PREFIX bool Point3<T>::operator!=(const Point3<T>& right) const {
    return !(*this == right);
}
TMPL_PREFIX Vector3<T> Point3<T>::operator-(const Point3<T>& right) const {
    return Vector3<T>(x - right.x, y - right.y, z - right.z);
}
TMPL_PREFIX Point3<T> Point3<T>::operator+(const Vector3<T>& right) const {
    return Point3<T>(x + right.x, y + right.y, z + right.z);
}
TMPL_PREFIX Point3<T>& Point3<T>::operator+=(const Vector3<T>& right) {
    x += right.x;
    y += right.y;
    z += right.z;
    return *this;
}
TMPL_PREFIX Point3<T>& Point3<T>::operator-=(const Vector3<T>& right) {
    x -= right.x;
    y -= right.y;
    z -= right.z;
    return *this;
}

#undef TMPL_PREFIX

template class VI_GE_API Point3<float>;
template class VI_GE_API Point3<double>;
template class VI_GE_API Point3<bool>;
template class VI_GE_API Point3<int8_t>;
template class VI_GE_API Point3<uint8_t>;
template class VI_GE_API Point3<int16_t>;
template class VI_GE_API Point3<uint16_t>;
template class VI_GE_API Point3<int32_t>;
template class VI_GE_API Point3<uint32_t>;
template class VI_GE_API Point3<int64_t>;
template class VI_GE_API Point3<uint64_t>;

VI_GE_NS_END