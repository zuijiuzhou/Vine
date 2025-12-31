#include <vine/math/Point2.hpp>

#include <cassert>
#include <cmath>
#include <cstdint>

#include <vine/math/Math.hpp>
#include <vine/math/Vector2.hpp>

#include "vine/math/Comm.hpp"

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Point2<T>::Point2()
  : x(T())
  , y(T())
{}

TMPL_PREFIX Point2<T>::Point2(T xx, T yy)
  : x(xx)
  , y(yy)
{}

TMPL_PREFIX const Vector2<T>& Point2<T>::asVector() const
{
    return reinterpret_cast<const Vector2<T>&>(*this);
}

TMPL_PREFIX Vector2<T> Point2<T>::toVector() const
{
    return Vector2<T>(x, y);
}

TMPL_PREFIX double Point2<T>::distanceTo(const Point2<T>& pt) const
{
    return sqrt(x * pt.x + y * pt.y);
}

TMPL_PREFIX bool Point2<T>::isZero() const
{
    return x == T() && y == T();
}

TMPL_PREFIX bool Point2<T>::isZero(T eps) const requires(Real<T>)
{
    return math::isZero<T>(x, eps) && math::isZero<T>(y, eps);
}

TMPL_PREFIX bool Point2<T>::isEqual(const Point2<T>& other) const
{
    return *this == other;
}

TMPL_PREFIX bool Point2<T>::isEqual(const Point2<T>& other, T eps) const requires(Real<T>)
{
    return math::isEqual<T>(x, other.x, eps) && math::isEqual<T>(y, other.y, eps);
}

TMPL_PREFIX bool Point2<T>::operator==(const Point2<T>& right) const
{
    return x == right.x && y == right.y;
}

TMPL_PREFIX bool Point2<T>::operator!=(const Point2<T>& right) const
{
    return !(*this == right);
}

TMPL_PREFIX Point2<T> Point2<T>::operator+(const Vector2<T>& right) const
{
    return Point2<T>(arithmetic_add(x, right.x), arithmetic_add(y, right.y));
}

TMPL_PREFIX Vector2<T> Point2<T>::operator-(const Point2<T>& right) const
{
    return Vector2<T>(arithmetic_sub(x, right.x), arithmetic_sub(y, right.y));
}

TMPL_PREFIX Point2<T>& Point2<T>::operator+=(const Vector2<T>& right)
{
    x = arithmetic_add(x, right.x);
    y = arithmetic_add(y, right.y);

    return *this;
}

TMPL_PREFIX Point2<T>& Point2<T>::operator-=(const Vector2<T>& right)
{
    x = arithmetic_sub(x, right.x);
    y = arithmetic_sub(y, right.y);

    return *this;
}

TMPL_PREFIX Point2<T> Point2<T>::operator-() const
{
    return Point2<T>(-x, -y);
}

TMPL_PREFIX T& Point2<T>::operator[](size_t index)
{
    assert(index < 2);
    return data[index];
}

TMPL_PREFIX const T& Point2<T>::operator[](size_t index) const
{
    assert(index < 2);
    return data[index];
}

#undef TMPL_PREFIX

template class VI_MATH_API Point2<float>;
template class VI_MATH_API Point2<double>;
template class VI_MATH_API Point2<bool>;
template class VI_MATH_API Point2<int8_t>;
template class VI_MATH_API Point2<uint8_t>;
template class VI_MATH_API Point2<int16_t>;
template class VI_MATH_API Point2<uint16_t>;
template class VI_MATH_API Point2<int32_t>;
template class VI_MATH_API Point2<uint32_t>;
template class VI_MATH_API Point2<int64_t>;
template class VI_MATH_API Point2<uint64_t>;

VI_MATH_NS_END