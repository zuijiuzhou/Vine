#pragma once

#include "Point3.hpp"
#include "Vector3.hpp"

VI_MATH_NS_BEGIN

/**
 * @brief
 * @tparam T Only accepts float and double.
 */
template <typename T>
class Line {
  public:
    using value_type = T;

  public:
    Line(const Point3<T>& origin, const Vector3<T>& direction);

  public:
    bool intersectWith(const Line& line, Point3<T>& intersectionPt, T tol) const;

  public:
    Point3<T>  origin;
    Vector3<T> direction;
};

using Linef = Line<float>;
using Lined = Line<double>;

VI_MATH_NS_END