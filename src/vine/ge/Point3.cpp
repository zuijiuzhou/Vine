#include <vine/ge/Point3.hpp>

#include <cassert>

#include <vine/ge/Math.hpp>
#include <vine/ge/Point2.hpp>
#include <vine/ge/Vector3.hpp>

VI_GE_NS_BEGIN

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
    return ge::isZero<T>(x, eps) && ge::isZero<T>(y, eps) && ge::isZero<T>(z, eps);
}

TMPL_PREFIX bool Point3<T>::isEqual(const Point3<T>& other) const
{
    return *this == other;
}

TMPL_PREFIX bool Point3<T>::isEqual(const Point3<T>& other, T eps) const requires(Real<T>)
{
    return ge::isEqual<T>(x, other.x, eps) && ge::isEqual<T>(y, other.y, eps) && ge::isEqual<T>(z, other.z, eps);
}

TMPL_PREFIX bool Point3<T>::operator==(const Point3<T>& right) const
{
    return x == right.x && y == right.y && z == right.z;
}

TMPL_PREFIX bool Point3<T>::operator!=(const Point3<T>& right) const
{
    return !(*this == right);
}

TMPL_PREFIX Vector3<T> Point3<T>::operator-(const Point3<T>& right) const
{
    return Vector3<T>(x - right.x, y - right.y, z - right.z);
}

TMPL_PREFIX Point3<T> Point3<T>::operator+(const Vector3<T>& right) const
{
    return Point3<T>(x + right.x, y + right.y, z + right.z);
}

TMPL_PREFIX Point3<T>& Point3<T>::operator+=(const Vector3<T>& right)
{
    x += right.x;
    y += right.y;
    z += right.z;
    return *this;
}

TMPL_PREFIX Point3<T>& Point3<T>::operator-=(const Vector3<T>& right)
{
    x -= right.x;
    y -= right.y;
    z -= right.z;
    return *this;
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