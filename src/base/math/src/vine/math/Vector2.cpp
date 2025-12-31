
#include <vine/math/Vector2.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <type_traits>

#include <vine/math/Math.hpp>
#include <vine/math/Point2.hpp>

#include "vine/math/Comm.hpp"

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Vector2<T>::Vector2()
  : x(T())
  , y(T())
{}

TMPL_PREFIX Vector2<T>::Vector2(T xx, T yy)
  : x(xx)
  , y(yy)
{}

TMPL_PREFIX void Vector2<T>::set(T xx, T yy)
{
    x = xx;
    y = yy;
}

TMPL_PREFIX void Vector2<T>::get(T& xx, T& yy) const
{
    xx = x;
    yy = y;
}

TMPL_PREFIX Point2<T> Vector2<T>::toPoint() const
{
    return Point2<T>(x, y);
}

TMPL_PREFIX const Point2<T>& Vector2<T>::asPoint() const
{
    return reinterpret_cast<const Point2<T>&>(*this);
}

TMPL_PREFIX TypeF<T> Vector2<T>::length() const requires(Real<T>)
{
    return calc_vec_len_safe(x, y);
}

TMPL_PREFIX TypeF<T> Vector2<T>::length2() const requires(Real<T>)
{
    return calc_vec_len2_safe(x, y);
}

TMPL_PREFIX TypeF<T> Vector2<T>::angleTo(const Vector2<T>& other) const requires(Real<T>)
{
    auto llen = length();
    auto rlen = other.length();

    if (llen == 0. || std::isnan(llen) || std::isinf(llen))
        return T(0);

    if (rlen == 0. || std::isnan(rlen) || std::isinf(rlen))
        return T(0);

    return std::acos(std::clamp<T>(dot(other) / (llen * rlen), T(-1), T(1)));
}

TMPL_PREFIX T Vector2<T>::dot(const Vector2<T>& other) const requires(Real<T>)
{
    // return multiply_safe(x, other.x) + multiply_safe(y, other.y);
    return x * other.x + y * other.y;
}

TMPL_PREFIX T Vector2<T>::cross(const Vector2<T>& other) const requires(Real<T>)
{
    // return multiply_safe(x, other.y) - multiply_safe(y, other.x);
    return x * other.y - y * other.x;
}

TMPL_PREFIX T Vector2<T>::normalize() requires(FP<T>)
{
    auto len = length();

    if (len > T(0)) {
        x /= len;
        y /= len;
    }
    else {
        x = T(0);
        y = T(0);
    }
    return len;
}

TMPL_PREFIX bool Vector2<T>::isZero() const
{
    return x == T() && y == T();
}

TMPL_PREFIX bool Vector2<T>::isZero(T eps) const requires(Real<T>)
{
    return math::isZero<T>(x, eps) && math::isZero<T>(y, eps);
}

TMPL_PREFIX bool Vector2<T>::isEqual(const Vector2<T>& other) const
{
    return *this == other;
}

TMPL_PREFIX bool Vector2<T>::isEqual(const Vector2<T>& other, T eps) const requires(Real<T>)
{
    return math::isEqual<T>(x, other.x, eps) && math::isEqual<T>(y, other.y, eps);
}

TMPL_PREFIX bool Vector2<T>::operator==(const Vector2<T>& right) const
{
    return x == right.x && y == right.y;
}

TMPL_PREFIX bool Vector2<T>::operator!=(const Vector2<T>& right) const
{
    return !(*this == right);
}

TMPL_PREFIX Vector2<T> Vector2<T>::operator+(const Vector2<T>& right) const
{
    return Vector2<T>(arithmetic_add(x, right.x), arithmetic_add(y, right.y));
}

TMPL_PREFIX Vector2<T> Vector2<T>::operator-(const Vector2<T>& right) const
{
    return Vector2<T>(arithmetic_sub(x, right.x), arithmetic_sub(y, right.y));
}

TMPL_PREFIX Vector2<T> Vector2<T>::operator*(T scale) const
{
    Vector2<T> v;

    v.x = arithmetic_multiply(x, scale);
    v.y = arithmetic_multiply(y, scale);

    return v;
}

TMPL_PREFIX Vector2<T> Vector2<T>::operator/(T scale) const
{
    Vector2<T> v;

    v.x = arithmetic_division(x, scale);
    v.y = arithmetic_division(y, scale);

    return v;
}

TMPL_PREFIX Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& right)
{
    x = arithmetic_add(x, right.x);
    y = arithmetic_add(y, right.y);

    return *this;
}

TMPL_PREFIX Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& right)
{
    x = arithmetic_sub(x, right.x);
    y = arithmetic_sub(y, right.y);

    return *this;
}

TMPL_PREFIX Vector2<T>& Vector2<T>::operator*=(T scale)
{
    x = arithmetic_multiply(x, scale);
    y = arithmetic_multiply(y, scale);

    return *this;
}

TMPL_PREFIX Vector2<T>& Vector2<T>::operator/=(T scale)
{
    x = arithmetic_division(x, scale);
    y = arithmetic_division(y, scale);

    return *this;
}

TMPL_PREFIX Vector2<T> Vector2<T>::operator-() const
{
    return Vector2<T>(-x, -y);
}

TMPL_PREFIX T Vector2<T>::operator*(const Vector2<T>& other) const requires(Real<T>)
{
    return dot(other);
}

TMPL_PREFIX T Vector2<T>::operator^(const Vector2<T>& other) const requires(Real<T>)
{
    return cross(other);
}

TMPL_PREFIX T& Vector2<T>::operator[](size_t index)
{
    assert(index < 2);
    return data[index];
}

TMPL_PREFIX const T& Vector2<T>::operator[](size_t index) const
{
    assert(index < 2);
    return data[index];
}

#undef TMPL_PREFIX

template class VI_MATH_API Vector2<float>;
template class VI_MATH_API Vector2<double>;
template class VI_MATH_API Vector2<bool>;
template class VI_MATH_API Vector2<int8_t>;
template class VI_MATH_API Vector2<uint8_t>;
template class VI_MATH_API Vector2<int16_t>;
template class VI_MATH_API Vector2<uint16_t>;
template class VI_MATH_API Vector2<int32_t>;
template class VI_MATH_API Vector2<uint32_t>;
template class VI_MATH_API Vector2<int64_t>;
template class VI_MATH_API Vector2<uint64_t>;

VI_MATH_NS_END