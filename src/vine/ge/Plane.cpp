#include <vine/ge/Plane.hpp>

VI_GE_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Plane<T>::Plane(const Point3<T>& origin, const Vector3<T>& normal)
  : origin(origin)
  , normal(normal) {
}

TMPL_PREFIX bool Plane<T>::intersectWith(const Line<T>& line, Point3<T>& intersectionPt, T tol) const {
    return false;
}

template class VI_GE_API Plane<float>;
template class VI_GE_API Plane<double>;
VI_GE_NS_END