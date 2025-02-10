#pragma once

#include "Ge.h"
#include "ge_global.h"

VI_GE_NS_BEGIN
class Vector2f;
class VI_GE_API Point2f {
  public:
    Point2f();
    Point2f(float xx, float yy);

  public:
    const Vector2f& asVector() const;
    Vector2f        toVector() const;
    float           distanceTo(const Point2f& pt) const;

    bool equals(const Point2f& other, float eps = EPSF) const noexcept;

  public:
    bool     operator==(const Point2f& right) const;
    bool     operator!=(const Point2f& right) const;
    Vector2f operator-(const Point2f& right) const;
    Point2f  operator+(const Vector2f& right) const;
    Point2f& operator+=(const Vector2f& right);
    Point2f& operator-=(const Vector2f& right);

  public:
    float x;
    float y;
};
VI_GE_NS_END