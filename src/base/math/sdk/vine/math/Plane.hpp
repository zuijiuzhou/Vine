#pragma once

#include "Line.hpp"

VI_MATH_NS_BEGIN

/**
 * @brief
 * @tparam T Only accepts float and double.
 */
template <typename T>
class Plane {
  public:
    using value_type = T;

  public:
    constexpr Plane(const Point3<T>& origin, const Vector3<T>& normal)
      : origin(origin)
      , normal(normal)
    {}

  public:
    [[nodiscard]]
    bool intersectWith(const Line<T>& line, Point3<T>& intersection_pt, T eps = EPS<T>()) const;
    [[nodiscard]]
    bool intersectWith(const Plane<T>& plane, Line<T>& intersection_line, T eps = EPS<T>()) const;

  public:
    Point3<T>  origin;
    Vector3<T> normal;
};

using Planef = Plane<float>;
using Planed = Plane<double>;
VI_MATH_NS_END
