
#include <vine/ge/Vector2.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <type_traits>

#include <vine/ge/Math.hpp>
#include <vine/ge/Point2.hpp>

#include "Comm.hpp"

VI_GE_NS_BEGIN

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
    return ge::isZero<T>(x, eps) && ge::isZero<T>(y, eps);
}

TMPL_PREFIX bool Vector2<T>::isEqual(const Vector2<T>& other) const
{
    return *this == other;
}

TMPL_PREFIX bool Vector2<T>::isEqual(const Vector2<T>& other, T eps) const requires(Real<T>)
{
    return ge::isEqual<T>(x, other.x, eps) && ge::isEqual<T>(y, other.y, eps);
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
    return Vector2<T>(advance_add(x, right.x), advance_add(y, right.y));
}

TMPL_PREFIX Vector2<T> Vector2<T>::operator-(const Vector2<T>& right) const
{
    return Vector2<T>(advance_sub(x, right.x), advance_sub(y, right.y));
}

TMPL_PREFIX Vector2<T> Vector2<T>::operator*(T scale) const
{
    Vector2<T> v;

    v.x = advance_multiply(x, scale);
    v.y = advance_multiply(y, scale);

    return v;
}

TMPL_PREFIX Vector2<T> Vector2<T>::operator/(T scale) const
{
    Vector2<T> v;

    v.x = advance_division(x, scale);
    v.y = advance_division(y, scale);

    return v;
}

TMPL_PREFIX Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& right)
{
    x = advance_add(x, right.x);
    y = advance_add(y, right.y);

    return *this;
}

TMPL_PREFIX Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& right)
{
    x = advance_sub(x, right.x);
    y = advance_sub(y, right.y);

    return *this;
}

TMPL_PREFIX Vector2<T>& Vector2<T>::operator*=(T scale)
{
    x = advance_multiply(x, scale);
    y = advance_multiply(y, scale);

    return *this;
}

TMPL_PREFIX Vector2<T>& Vector2<T>::operator/=(T scale)
{
    x = advance_division(x, scale);
    y = advance_division(y, scale);

    return *this;
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