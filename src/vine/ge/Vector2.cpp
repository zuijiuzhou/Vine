
#include <vine/ge/Vector2.h>

#include <cmath>
#include <type_traits>

#include <vine/ge/Ge.h>
#include <vine/ge/Point2.h>

VI_GE_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Vector2<T>::Vector2()
  : x(T())
  , y(T()) {
}
TMPL_PREFIX Vector2<T>::Vector2(T xx, T yy)
  : x(xx)
  , y(yy) {
}
TMPL_PREFIX void Vector2<T>::set(T xx, T yy) {
    x = xx;
    y = yy;
}
TMPL_PREFIX void Vector2<T>::get(T& xx, T& yy) const {
    xx = x;
    yy = y;
}

TMPL_PREFIX bool Vector2<T>::isZero(T eps) const {
    return ge::isZero(x, eps) && ge::isZero(y, eps);
}
TMPL_PREFIX Point2<T> Vector2<T>::toPoint() const {
    return Point2<T>(x, y);
}
TMPL_PREFIX const Point2<T>& Vector2<T>::asPoint() const {
    return reinterpret_cast<const Point2<T>&>(*this);
}
TMPL_PREFIX bool Vector2<T>::equals(const Vector2<T>& other, T eps) const {
    return ge::isEqual(x, other.x, eps) && ge::isEqual(y, other.y, eps);
}
TMPL_PREFIX bool Vector2<T>::operator==(const Vector2<T>& right) const {
    return x == right.x && y == right.y;
}
TMPL_PREFIX bool Vector2<T>::operator!=(const Vector2<T>& right) const {
    return !(*this == right);
}
TMPL_PREFIX Vector2<T> Vector2<T>::operator-(const Vector2<T>& right) const {
    return Vector2<T>(x - right.x, y - right.y);
}
TMPL_PREFIX Vector2<T> Vector2<T>::operator+(const Vector2<T>& right) const {
    return Vector2<T>(x + right.x, y + right.y);
}
TMPL_PREFIX Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& right) {
    x += right.x;
    y += right.y;
    return *this;
}
TMPL_PREFIX Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& right) {
    x -= right.x;
    y -= right.y;
    return *this;
}
TMPL_PREFIX Vector2<T> Vector2<T>::operator*(T scale) const {
    Vector2<T> v(*this);
    v.x *= scale;
    v.y *= scale;
    return v;
}
TMPL_PREFIX Vector2<T>& Vector2<T>::operator*=(T scale) {
    x *= scale;
    y *= scale;
    return *this;
}
TMPL_PREFIX Vector2<T> Vector2<T>::operator/(T scale) const {
    Vector2<T> v(*this);
    v.x /= scale;
    v.y /= scale;
    return v;
}
TMPL_PREFIX Vector2<T>& Vector2<T>::operator/=(T scale) {
    x /= scale;
    y /= scale;
    return *this;
}

#undef TMPL_PREFIX

template class VI_GE_API Vector2<float>;
template class VI_GE_API Vector2<double>;
template class VI_GE_API Vector2<bool>;
template class VI_GE_API Vector2<int8_t>;
template class VI_GE_API Vector2<uint8_t>;
template class VI_GE_API Vector2<int16_t>;
template class VI_GE_API Vector2<uint16_t>;
template class VI_GE_API Vector2<int32_t>;
template class VI_GE_API Vector2<uint32_t>;
template class VI_GE_API Vector2<int64_t>;
template class VI_GE_API Vector2<uint64_t>;

VI_GE_NS_END