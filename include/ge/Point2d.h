#pragma once

#include "ge_export.h"

ETD_GE_NS_BEGIN
class Vector2d;
class Line2d;
class ETD_GE_API Point2d
{
public:
    Point2d();
    Point2d(double xx, double yy);

public:
    const Vector2d &asVector() const;
    Vector2d toVector() const;
    double distanceTo(const Point2d &pt) const;
    double distanceTo(const Line2d &line) const;

public:
    bool operator==(const Point2d &right) const;
    bool operator!=(const Point2d &right) const;
    Vector2d operator-(const Point2d &right) const;
    Point2d operator+(const Vector2d &right) const;
    Point2d &operator+=(const Vector2d &right);
    Point2d &operator-=(const Vector2d &right);

public:
    double x;
    double y;
};
ETD_GE_NS_END