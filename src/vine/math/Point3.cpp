#include <vine/math/Point3.hpp>

#include <cassert>

#include <vine/math/Math.hpp>
#include <vine/math/Point2.hpp>
#include <vine/math/Vector3.hpp>

#include "Comm.hpp"

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Point3<T>::Point3()
  : x(0.)
  , y(0.)
  , z(0.)
{}

TMPL_PREFIX Point3<T>::Point3(const Point2<T>& pt, T zz)
  : x(pt.x)
  , y(pt.x)
  , z(zz)
{}

TMPL_PREFIX Point3<T>::Point3(T xx, T yy, T zz)
  : x(xx)
  , y(yy)
  , z(zz)
{}

TMPL_PREFIX Vector3<T> Point3<T>::toVector() const
{
    return Vector3<T>(x, y, z);
}

TMPL_PREFIX const Point2<T>& Point3<T>::asPoint2() const
{
    return reinterpret_cast<const Point2<T>&>(*this);
}

TMPL_PREFIX const Vector3<T>& Point3<T>::asVector() const
{
    return reinterpret_cast<const Vector3<T>&>(*this);
}

TMPL_PREFIX bool Point3<T>::isZero() const
{
    return x == T() && y == T() && z == T();
}

TMPL_PREFIX bool Point3<T>::isZero(T eps) const requires(Real<T>)
{
    return math::isZero<T>(x, eps) && math::isZero<T>(y, eps) && math::isZero<T>(z, eps);
}

TMPL_PREFIX bool Point3<T>::isEqual(const Point3<T>& other) const
{
    return *this == other;
}

TMPL_PREFIX bool Point3<T>::isEqual(const Point3<T>& other, T eps) const requires(Real<T>)
{
    return math::isEqual<T>(x, other.x, eps) && math::isEqual<T>(y, other.y, eps) && math::isEqual<T>(z, other.z, eps);
}

TMPL_PREFIX bool Point3<T>::operator==(const Point3<T>& right) const
{
    return x == right.x && y == right.y && z == right.z;
}

TMPL_PREFIX bool Point3<T>::operator!=(const Point3<T>& right) const
{
    return !(*this == right);
}

TMPL_PREFIX Point3<T> Point3<T>::operator+(const Vector3<T>& right) const
{
    return Point3<T>(arithmetic_add(x, right.x), arithmetic_add(y, right.y), arithmetic_add(z, right.z));
}

TMPL_PREFIX Vector3<T> Point3<T>::operator-(const Point3<T>& right) const
{
    return Vector3<T>(arithmetic_sub(x, right.x), arithmetic_sub(y, right.y), arithmetic_sub(z, right.z));
}

TMPL_PREFIX Point3<T>& Point3<T>::operator+=(const Vector3<T>& right)
{
    x = arithmetic_add(x, right.x);
    y = arithmetic_add(y, right.y);
    z = arithmetic_add(z, right.z);

    return *this;
}

TMPL_PREFIX Point3<T>& Point3<T>::operator-=(const Vector3<T>& right)
{
    x = arithmetic_sub(x, right.x);
    y = arithmetic_sub(y, right.y);
    z = arithmetic_sub(z, right.z);

    return *this;
}

TMPL_PREFIX Point3<T> Point3<T>::operator-() const
{
    return Point3<T>(-x, -y, -z);
}

TMPL_PREFIX T& Point3<T>::operator[](size_t index)
{
    assert(index < 3);
    return data[index];
}

TMPL_PREFIX const T& Point3<T>::operator[](size_t index) const
{
    assert(index < 3);
    return data[index];
}

#undef TMPL_PREFIX

template class VI_MATH_API Point3<float>;
template class VI_MATH_API Point3<double>;
template class VI_MATH_API Point3<bool>;
template class VI_MATH_API Point3<int8_t>;
template class VI_MATH_API Point3<uint8_t>;
template class VI_MATH_API Point3<int16_t>;
template class VI_MATH_API Point3<uint16_t>;
template class VI_MATH_API Point3<int32_t>;
template class VI_MATH_API Point3<uint32_t>;
template class VI_MATH_API Point3<int64_t>;
template class VI_MATH_API Point3<uint64_t>;

VI_MATH_NS_END