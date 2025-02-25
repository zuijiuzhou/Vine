#include <cmath>
#include <vine/ge/Point2.h>
#include <vine/ge/Vector2.h>

#include <cstdint>

VI_GE_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Point2<T>::Point2()
  : x(T())
  , y(T()) {
}
TMPL_PREFIX Point2<T>::Point2(T xx, T yy)
  : x(xx)
  , y(yy) {
}
TMPL_PREFIX const Vector2<T>& Point2<T>::asVector() const {
    return reinterpret_cast<const Vector2<T>&>(*this);
}
TMPL_PREFIX Vector2<T> Point2<T>::toVector() const {
    return Vector2<T>(x, y);
}
TMPL_PREFIX double Point2<T>::distanceTo(const Point2<T>& pt) const {
    return sqrt(x * pt.x + y * pt.y);
}
TMPL_PREFIX bool Point2<T>::operator==(const Point2<T>& right) const {
    return x == right.x && y == right.y;
}
TMPL_PREFIX bool Point2<T>::operator!=(const Point2<T>& right) const {
    return !(*this == right);
}
TMPL_PREFIX Vector2<T> Point2<T>::operator-(const Point2<T>& right) const {
    return Vector2<T>(x - right.x, y - right.y);
}
TMPL_PREFIX Point2<T> Point2<T>::operator+(const Vector2<T>& right) const {
    return Point2<T>(x + right.x, y + right.y);
}
TMPL_PREFIX Point2<T>& Point2<T>::operator+=(const Vector2<T>& right) {
    x += right.x;
    y += right.y;
    return *this;
}
TMPL_PREFIX Point2<T>& Point2<T>::operator-=(const Vector2<T>& right) {
    x -= right.x;
    y -= right.y;
    return *this;
}

#undef TMPL_PREFIX

template class VI_GE_API Point2<float>;
template class VI_GE_API Point2<double>;
template class VI_GE_API Point2<bool>;
template class VI_GE_API Point2<int8_t>;
template class VI_GE_API Point2<uint8_t>;
template class VI_GE_API Point2<int16_t>;
template class VI_GE_API Point2<uint16_t>;
template class VI_GE_API Point2<int32_t>;
template class VI_GE_API Point2<uint32_t>;
template class VI_GE_API Point2<int64_t>;
template class VI_GE_API Point2<uint64_t>;

VI_GE_NS_END