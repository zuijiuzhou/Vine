#pragma once

#include "Line.h"

VI_GE_NS_BEGIN
template <typename T> class Plane {
  public:
    Plane(const Point3<T>& origin, const Vector3<T>& normal);

  public:
    bool intersectWith(const Line<T>& line, Point3<T>& intersectionPt, T tol) const;

  public:
    Point3<T>  origin;
    Vector3<T> normal;
};
VI_GE_NS_END