#include <vine/ge/Vector3.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <type_traits>

#include <vine/ge/Math.hpp>
#include <vine/ge/Point3.hpp>
#include <vine/ge/Vector2.hpp>

#include "Comm.hpp"

VI_GE_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Vector3<T>::Vector3()
  : x(0)
  , y(0)
  , z(0)
{}

TMPL_PREFIX Vector3<T>::Vector3(const Vector2<T>& v, T zz)
  : x(v.x)
  , y(v.y)
  , z(zz)
{}

TMPL_PREFIX Vector3<T>::Vector3(T xx, T yy, T zz)
  : x(xx)
  , y(yy)
  , z(zz)
{}

TMPL_PREFIX void Vector3<T>::set(const Vector2<T>& vec2)
{
    x = vec2.x;
    y = vec2.y;
}

TMPL_PREFIX void Vector3<T>::set(const Vector2<T>& vec2, T zz)
{
    x = vec2.x;
    y = vec2.y;
    z = zz;
}

TMPL_PREFIX void Vector3<T>::set(T xx, T yy, T zz)
{
    x = xx;
    y = yy;
    z = zz;
}

TMPL_PREFIX void Vector3<T>::get(T& xx, T& yy, T& zz) const
{
    xx = x;
    yy = y;
    zz = z;
}

TMPL_PREFIX Point3<T> Vector3<T>::toPoint() const
{
    return Point3<T>(x, y, z);
}

TMPL_PREFIX const Point3<T>& Vector3<T>::asPoint() const
{
    return reinterpret_cast<const Point3<T>&>(*this);
}

TMPL_PREFIX const Vector2<T>& Vector3<T>::asVector2() const
{
    return reinterpret_cast<const Vector2<T>&>(*this);
}

TMPL_PREFIX TypeF<T> Vector3<T>::length() const requires(Real<T>)
{
    return calc_vec_len_safe(x, y, z);
}

TMPL_PREFIX TypeF<T> Vector3<T>::length2() const requires(Real<T>)
{
    return calc_vec_len2_safe(x, y, z);
}

TMPL_PREFIX TypeF<T> Vector3<T>::angleTo(const Vector3<T>& other) const requires(Real<T>)
{
    auto llen = length();
    auto rlen = other.length();

    if (llen == 0. || std::isnan(llen) || std::isinf(llen))
        return T(0);

    if (rlen == 0. || std::isnan(rlen) || std::isinf(rlen))
        return T(0);

    return std::acos(std::clamp<T>(dot(other) / (llen * rlen), T(-1), T(1)));
}

TMPL_PREFIX T Vector3<T>::dot(const Vector3<T>& other) const requires(Real<T>)
{
    return x * other.x + y * other.y + z * other.z;
}

TMPL_PREFIX Vector3<T> Vector3<T>::cross(const Vector3<T>& other) const requires(Real<T>)
{
    return Vector3<T>(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

TMPL_PREFIX T Vector3<T>::normalize() requires(FP<T>)
{
    auto len = length();

    if (len > T(0)) {
        x /= len;
        y /= len;
        z /= len;
    }
    else {
        x = T(0);
        y = T(0);
        z = T(0);
    }
    return len;
}

TMPL_PREFIX bool Vector3<T>::isZero() const
{
    return x == T() && y == T() && z == T();
}

TMPL_PREFIX bool Vector3<T>::isZero(T eps) const requires(Real<T>)
{
    return ge::isZero<T>(x, eps) && ge::isZero<T>(y, eps) && ge::isZero<T>(z, eps);
}

TMPL_PREFIX bool Vector3<T>::isEqual(const Vector3<T>& other) const
{
    return *this == other;
}

TMPL_PREFIX bool Vector3<T>::isEqual(const Vector3<T>& other, T eps) const requires(Real<T>)
{
    return ge::isEqual<T>(x, other.x, eps) && ge::isEqual<T>(y, other.y, eps) && ge::isEqual<T>(z, other.z, eps);
}

TMPL_PREFIX bool Vector3<T>::operator==(const Vector3<T>& right) const
{
    return x == right.x && y == right.y && z == right.z;
}

TMPL_PREFIX bool Vector3<T>::operator!=(const Vector3<T>& right) const
{
    return !(*this == right);
}

TMPL_PREFIX Vector3<T> Vector3<T>::operator+(const Vector3<T>& right) const
{
    return Vector3<T>(advance_add(x, right.x), advance_add(y, right.y), advance_add(z, right.z));
}

TMPL_PREFIX Vector3<T> Vector3<T>::operator-(const Vector3<T>& right) const
{
    return Vector3<T>(advance_sub(x, right.x), advance_sub(y, right.y), advance_sub(z, right.z));
}

TMPL_PREFIX Vector3<T> Vector3<T>::operator*(T scale) const
{
    Vector3<T> v(*this);

    v.x = advance_multiply(x, scale);
    v.y = advance_multiply(y, scale);
    v.z = advance_multiply(z, scale);

    return v;
}

TMPL_PREFIX Vector3<T> Vector3<T>::operator/(T scale) const
{
    Vector3<T> v(*this);

    v.x = advance_division(x, scale);
    v.y = advance_division(y, scale);
    v.z = advance_division(z, scale);

    return v;
}

TMPL_PREFIX Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& right)
{
    x = advance_add(x, right.x);
    y = advance_add(y, right.y);
    z = advance_add(z, right.z);

    return *this;
}

TMPL_PREFIX Vector3<T>& Vector3<T>::operator-=(const Vector3<T>& right)
{
    x = advance_sub(x, right.x);
    y = advance_sub(y, right.y);
    z = advance_sub(z, right.z);

    return *this;
}

TMPL_PREFIX Vector3<T>& Vector3<T>::operator*=(T scale)
{
    x = advance_multiply(x, scale);
    y = advance_multiply(y, scale);
    z = advance_multiply(z, scale);

    return *this;
}

TMPL_PREFIX Vector3<T>& Vector3<T>::operator/=(T scale)
{
    x = advance_division(x, scale);
    y = advance_division(y, scale);
    z = advance_division(z, scale);

    return *this;
}

TMPL_PREFIX T Vector3<T>::operator*(const Vector3<T>& other) const requires(Real<T>)
{
    return dot(other);
}

TMPL_PREFIX Vector3<T> Vector3<T>::operator^(const Vector3<T>& other) const requires(Real<T>)
{
    return cross(other);
}

TMPL_PREFIX T& Vector3<T>::operator[](size_t index)
{
    assert(index < 3);
    return data[index];
}

TMPL_PREFIX const T& Vector3<T>::operator[](size_t index) const
{
    assert(index < 3);
    return data[index];
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