#pragma once

#include "ge_global.h"

VI_GE_NS_BEGIN
class Vector2i;
class VI_GE_API Point2i
{
public:
    Point2i();
    Point2i(int xx, int yy);

public:
    const Vector2i &asVector() const;
    Vector2i toVector() const;
    double distanceTo(const Point2i &pt) const;

public:
    bool operator==(const Point2i &right) const;
    bool operator!=(const Point2i &right) const;
    Vector2i operator-(const Point2i &right) const;
    Point2i operator+(const Vector2i &right) const;
    Point2i &operator+=(const Vector2i &right);
    Point2i &operator-=(const Vector2i &right);

public:
    int x;
    int y;
};
VI_GE_NS_END