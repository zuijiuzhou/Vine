#include <vine/math/Line.hpp>

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Line<T>::Line(const Point3<T>& origin, const Vector3<T>& direction)
  : origin(origin)
  , direction(direction) {
}

TMPL_PREFIX bool Line<T>::intersectWith(const Line<T>& line, Point3<T>& intersectionPt, T tol) const {
    if (line.origin == origin) {
        intersectionPt = origin;
        return true;
    }

    return false;
}

template class VI_MATH_API Line<float>;
template class VI_MATH_API Line<double>;

VI_MATH_NS_END