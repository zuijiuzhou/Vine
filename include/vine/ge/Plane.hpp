#pragma once

#include "Line.hpp"

VI_GE_NS_BEGIN

/**
 * @brief
 * @tparam T Only accepts float and double.
 */
template <typename T>
class Plane {
  public:
    using value_type = T;

  public:
    Plane(const Point3<T>& origin, const Vector3<T>& normal);

  public:
    bool intersectWith(const Line<T>& line, Point3<T>& intersectionPt, T tol) const;
    bool intersectWith(const Plane<T>& plane, Line<T>& intersectionLine, T tol) const;

  public:
    Point3<T>  origin;
    Vector3<T> normal;
};

using Planef = Plane<float>;
using Planed = Plane<double>;
VI_GE_NS_END