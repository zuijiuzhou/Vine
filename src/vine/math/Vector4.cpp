#include <vine/math/Vector4.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <type_traits>

#include <vine/math/Math.hpp>
#include <vine/math/Vector3.hpp>

#include "Comm.hpp"

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Vector4<T>::Vector4()
  : x(T())
  , y(T())
  , z(T())
  , w(T())
{}

TMPL_PREFIX Vector4<T>::Vector4(const Vector3<T>& v, T ww)
  : x(v.x)
  , y(v.y)
  , z(v.z)
  , w(ww)
{}

TMPL_PREFIX Vector4<T>::Vector4(T xx, T yy, T zz, T ww)
  : x(xx)
  , y(yy)
  , z(zz)
  , w(ww)
{}

TMPL_PREFIX void Vector4<T>::set(const Vector3<T>& vec3)
{
    x = vec3.x;
    y = vec3.y;
    z = vec3.z;
}

TMPL_PREFIX void Vector4<T>::set(const Vector3<T>& vec3, T ww)
{
    x = vec3.x;
    y = vec3.y;
    z = vec3.z;
    w = ww;
}

TMPL_PREFIX void Vector4<T>::set(T xx, T yy, T zz, T ww)
{
    x = xx;
    y = yy;
    z = zz;
    w = ww;
}

TMPL_PREFIX void Vector4<T>::get(T& xx, T& yy, T& zz, T& ww) const
{
    xx = x;
    yy = y;
    zz = z;
    ww = w;
}

TMPL_PREFIX const Vector3<T>& Vector4<T>::asVector3() const
{
    return reinterpret_cast<const Vector3<T>&>(*this);
}

TMPL_PREFIX TypeF<T> Vector4<T>::length() const requires(Real<T>)
{
    return calc_vec_len_safe(x, y, w);
}

TMPL_PREFIX TypeF<T> Vector4<T>::length2() const requires(Real<T>)
{
    return calc_vec_len2_safe(x, y, z, w);
}

TMPL_PREFIX TypeF<T> Vector4<T>::angleTo(const Vector4<T>& other) const requires(Real<T>)
{
    auto llen = length();
    auto rlen = other.length();

    if (llen == 0. || std::isnan(llen) || std::isinf(llen))
        return T(0);

    if (rlen == 0. || std::isnan(rlen) || std::isinf(rlen))
        return T(0);

    return std::acos(std::clamp<T>(dot(other) / (llen * rlen), T(-1), T(1)));
}

TMPL_PREFIX T Vector4<T>::dot(const Vector4<T>& other) const requires(Real<T>)
{
    return x * other.x + y * other.y + z * other.z + w * other.w;
}

TMPL_PREFIX bool Vector4<T>::isZero() const
{
    return x == T() && y == T() && z == T() && w == T();
}

TMPL_PREFIX bool Vector4<T>::isZero(T eps) const requires(Real<T>)
{
    return math::isZero<T>(x, eps) && math::isZero<T>(y, eps) && math::isZero<T>(z, eps) && math::isZero<T>(w, eps);
}

TMPL_PREFIX bool Vector4<T>::isEqual(const Vector4<T>& other) const
{
    return *this == other;
}

TMPL_PREFIX bool Vector4<T>::isEqual(const Vector4<T>& other, T eps) const requires(Real<T>)
{
    return math::isEqual<T>(x, other.x, eps) && math::isEqual<T>(y, other.y, eps) && math::isEqual<T>(z, other.z, eps) &&
           math::isEqual<T>(w, other.w, eps);
}

TMPL_PREFIX bool Vector4<T>::operator==(const Vector4<T>& right) const
{
    return x == right.x && y == right.y && z == right.z;
}

TMPL_PREFIX bool Vector4<T>::operator!=(const Vector4<T>& right) const
{
    return !(*this == right);
}

TMPL_PREFIX Vector4<T> Vector4<T>::operator+(const Vector4<T>& right) const
{
    return Vector4<T>(arithmetic_add(x, right.x),
                      arithmetic_add(y, right.y),
                      arithmetic_add(z, right.z),
                      arithmetic_add(w, right.w));
}

TMPL_PREFIX Vector4<T> Vector4<T>::operator-(const Vector4<T>& right) const
{
    return Vector4<T>(arithmetic_sub(x, right.x),
                      arithmetic_sub(y, right.y),
                      arithmetic_sub(z, right.z),
                      arithmetic_sub(w, right.w));
}

TMPL_PREFIX Vector4<T> Vector4<T>::operator*(T scale) const
{
    Vector4<T> v(*this);

    v.x = arithmetic_multiply(x, scale);
    v.y = arithmetic_multiply(y, scale);
    v.z = arithmetic_multiply(z, scale);
    v.w = arithmetic_multiply(w, scale);

    return v;
}

TMPL_PREFIX Vector4<T> Vector4<T>::operator/(T scale) const
{
    Vector4<T> v(*this);

    v.x = arithmetic_division(x, scale);
    v.y = arithmetic_division(y, scale);
    v.z = arithmetic_division(z, scale);
    v.w = arithmetic_division(w, scale);

    return v;
}

TMPL_PREFIX Vector4<T>& Vector4<T>::operator+=(const Vector4<T>& right)
{
    x = arithmetic_add(x, right.x);
    y = arithmetic_add(y, right.y);
    z = arithmetic_add(z, right.z);
    w = arithmetic_add(w, right.w);

    return *this;
}

TMPL_PREFIX Vector4<T>& Vector4<T>::operator-=(const Vector4<T>& right)
{
    x = arithmetic_sub(x, right.x);
    y = arithmetic_sub(y, right.y);
    z = arithmetic_sub(z, right.z);
    w = arithmetic_sub(w, right.w);

    return *this;
}

TMPL_PREFIX Vector4<T>& Vector4<T>::operator*=(T scale)
{
    x = arithmetic_multiply(x, scale);
    y = arithmetic_multiply(y, scale);
    z = arithmetic_multiply(z, scale);
    w = arithmetic_multiply(w, scale);

    return *this;
}

TMPL_PREFIX Vector4<T>& Vector4<T>::operator/=(T scale)
{
    x = arithmetic_division(x, scale);
    y = arithmetic_division(y, scale);
    z = arithmetic_division(z, scale);
    w = arithmetic_division(w, scale);

    return *this;
}

TMPL_PREFIX Vector4<T> Vector4<T>::operator-() const
{
    return Vector4<T>(-x, -y, -z, -w);
}

TMPL_PREFIX T Vector4<T>::operator*(const Vector4<T>& other) const requires(Real<T>)
{
    return dot(other);
}

TMPL_PREFIX T& Vector4<T>::operator[](size_t index)
{
    assert(index < 4);
    return data[index];
}

TMPL_PREFIX const T& Vector4<T>::operator[](size_t index) const
{
    assert(index < 4);
    return data[index];
}

#undef TMPL_PREFIX

template class VI_MATH_API Vector4<float>;
template class VI_MATH_API Vector4<double>;
template class VI_MATH_API Vector4<bool>;
template class VI_MATH_API Vector4<int8_t>;
template class VI_MATH_API Vector4<uint8_t>;
template class VI_MATH_API Vector4<int16_t>;
template class VI_MATH_API Vector4<uint16_t>;
template class VI_MATH_API Vector4<int32_t>;
template class VI_MATH_API Vector4<uint32_t>;
template class VI_MATH_API Vector4<int64_t>;
template class VI_MATH_API Vector4<uint64_t>;

VI_MATH_NS_END