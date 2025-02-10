#pragma once

#include "ge_global.h"

#include "Ge.h"

VI_GE_NS_BEGIN
class Point2d;
class Vector3d;
class VI_GE_API Point3d {
  public:
    Point3d();
    Point3d(const Point2d& pt, double zz = 0.);
    Point3d(double xx, double yy, double zz);

  public:
    double distanceTo(const Point3d& pt) const;

    bool equals(const Point3d& other, double eps = EPSD) const noexcept;

    Vector3d        toVector() const;
    const Point2d&  asPoint2d() const;
    const Vector3d& asVector() const;

  public:
    bool operator==(const Point3d& right) const;
    bool operator!=(const Point3d& right) const;

    Vector3d operator-(const Point3d& right) const;
    Point3d  operator+(const Vector3d& right) const;
    Point3d& operator+=(const Vector3d& right);
    Point3d& operator-=(const Vector3d& right);

  public:
    double x;
    double y;
    double z;
};
VI_GE_NS_END