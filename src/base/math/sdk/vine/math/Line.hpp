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

    struct Intersection {
        // 是否相交
        bool valid{ false };
        // 是否重合
        bool   coincident{ false };
        double t1{ 0 };
        double t2{ 0 };
    };

  public:
    Line(const Point3<T>& origin, const Vector3<T>& direction);

  public:
    Intersection intersectWith(const Line& line, T eps) const;

  public:
    Point3<T>  origin;
    Vector3<T> direction;
};

using Linef = Line<float>;
using Lined = Line<double>;

VI_MATH_NS_END