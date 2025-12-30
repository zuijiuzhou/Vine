#include <vine/math/Plane.hpp>

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Plane<T>::Plane(const Point3<T>& origin, const Vector3<T>& normal)
  : origin(origin)
  , normal(normal)
{}

TMPL_PREFIX bool Plane<T>::intersectWith(const Line<T>& line, Point3<T>& intersectionPt, T tol) const
{
    return false;
}

TMPL_PREFIX
bool Plane<T>::intersectWith(const Plane<T>& plane, Line<T>& intersectionLine, T tol) const
{
    return false;
}

template class VI_MATH_API Plane<float>;
template class VI_MATH_API Plane<double>;
VI_MATH_NS_END