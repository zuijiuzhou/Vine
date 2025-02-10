#pragma once

#include "ge_global.h"

VI_GE_NS_BEGIN
template <typename T> class Point2 {
  public:
    using item_type = T;

  public:
    Point2();
    Point2(item_type xx, item_type yy);

  public:
    const Vector2i& asVector() const;
    Vector2i        toVector() const;
    double          distanceTo(const Point2& pt) const;

  public:
    bool     operator==(const Point2& right) const;
    bool     operator!=(const Point2& right) const;
    Vector2i operator-(const Point2& right) const;
    Point2  operator+(const Vector2i& right) const;
    Point2& operator+=(const Vector2i& right);
    Point2& operator-=(const Vector2i& right);

  public:
    int x;
    int y;
};
VI_GE_NS_END