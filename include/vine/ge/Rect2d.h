#pragma once

#include "ge_global.h"

#include "Point2d.h"
#include "Vector2d.h"

VI_GE_NS_BEGIN
class VI_GE_API Rect2d
{
public:
    Rect2d();
    Rect2d(const Point2d& top_left, const Vector2d& size);
    Rect2d(double x, double y, double w, double h);

public:
   Point2d topLeft() const;
   Point2d topRight() const;
   Point2d bottomLeft() const;
   Point2d bottomRight() const;

   Vector2d size() const;

public:
    double x, y, w, h;
};
VI_GE_NS_END