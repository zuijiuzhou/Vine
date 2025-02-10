#pragma once

#include "Ge.h"
#include "ge_global.h"

VI_GE_NS_BEGIN
class Point2f;
class Vector3f;
class Plane;
class VI_GE_API Point3f {
  public:
    Point3f();
    Point3f(const Point2f& pt, float zz = 0.f);
    Point3f(float xx, float yy, float zz);

  public:
    float distanceTo(const Point3f& pt) const;

    bool equals(const Point3f& other, float eps = EPSF) const noexcept;

    Vector3f        toVector() const;
    const Point2f&  asPoint2f() const;
    const Vector3f& asVector() const;

  public:
    bool operator==(const Point3f& right) const;
    bool operator!=(const Point3f& right) const;

    Vector3f operator-(const Point3f& right) const;
    Point3f  operator+(const Vector3f& right) const;
    Point3f& operator+=(const Vector3f& right);
    Point3f& operator-=(const Vector3f& right);

  public:
    float x;
    float y;
    float z;
};
VI_GE_NS_END