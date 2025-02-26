#pragma once

#include "Point3.h"
#include "Vector3.h"

VI_GE_NS_BEGIN
/**
 * @brief 
 * @tparam T Only accepts float and double.
 */
template <typename T> class Line {
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

VI_GE_NS_END