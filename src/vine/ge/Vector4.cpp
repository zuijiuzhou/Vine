#include <vine/ge/Vector4.hpp>

#include <cassert>
#include <stdexcept>

#include <vine/ge/Math.hpp>
#include <vine/ge/Vector3.hpp>

#include "Comm.hpp"

VI_GE_NS_BEGIN

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

TMPL_PREFIX bool Vector4<T>::isZero() const
{
    return x == T() && y == T() && z == T() && w == T();
}

TMPL_PREFIX bool Vector4<T>::isZero(T eps) const requires(Real<T>)
{
    return ge::isZero<T>(x, eps) && ge::isZero<T>(y, eps) && ge::isZero<T>(z, eps) && ge::isZero<T>(w, eps);
}

TMPL_PREFIX bool Vector4<T>::isEqual(const Vector4<T>& other) const
{
    return *this == other;
}

TMPL_PREFIX bool Vector4<T>::isEqual(const Vector4<T>& other, T eps) const requires(Real<T>)
{
    return ge::isEqual<T>(x, other.x, eps) && ge::isEqual<T>(y, other.y, eps) && ge::isEqual<T>(z, other.z, eps) &&
           ge::isEqual<T>(w, other.w, eps);
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
    return Vector4<T>(advance_add(x, right.x),
                      advance_add(y, right.y),
                      advance_add(z, right.z),
                      advance_add(w, right.w));
}

TMPL_PREFIX Vector4<T> Vector4<T>::operator-(const Vector4<T>& right) const
{
    return Vector4<T>(advance_sub(x, right.x),
                      advance_sub(y, right.y),
                      advance_sub(z, right.z),
                      advance_sub(w, right.w));
}

TMPL_PREFIX Vector4<T> Vector4<T>::operator*(T scale) const
{
    Vector4<T> v(*this);

    v.x = advance_multiply(x, scale);
    v.y = advance_multiply(y, scale);
    v.z = advance_multiply(z, scale);
    v.w = advance_multiply(w, scale);

    return v;
}

TMPL_PREFIX Vector4<T> Vector4<T>::operator/(T scale) const
{
    Vector4<T> v(*this);

    v.x = advance_division(x, scale);
    v.y = advance_division(y, scale);
    v.z = advance_division(z, scale);
    v.w = advance_division(w, scale);

    return v;
}

TMPL_PREFIX Vector4<T>& Vector4<T>::operator+=(const Vector4<T>& right)
{
    x = advance_add(x, right.x);
    y = advance_add(y, right.y);
    z = advance_add(z, right.z);
    w = advance_add(w, right.w);

    return *this;
}

TMPL_PREFIX Vector4<T>& Vector4<T>::operator-=(const Vector4<T>& right)
{
    x = advance_sub(x, right.x);
    y = advance_sub(y, right.y);
    z = advance_sub(z, right.z);
    w = advance_sub(w, right.w);

    return *this;
}

TMPL_PREFIX Vector4<T>& Vector4<T>::operator*=(T scale)
{
    x = advance_multiply(x, scale);
    y = advance_multiply(y, scale);
    z = advance_multiply(z, scale);
    w = advance_multiply(w, scale);

    return *this;
}

TMPL_PREFIX Vector4<T>& Vector4<T>::operator/=(T scale)
{
    x = advance_division(x, scale);
    y = advance_division(y, scale);
    z = advance_division(z, scale);
    w = advance_division(w, scale);

    return *this;
}

// TMPL_PREFIX Vector4<T>::ValueTypePromoted Vector4<T>::operator*(const Vector4<T>& vec) const  {
//     return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
// }
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