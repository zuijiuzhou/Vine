#pragma once

#include "ge_global.h"

#include "Point2i.h"
#include "Vector2i.h"

VI_GE_NS_BEGIN
class VI_GE_API Rect2i
{
public:
    Rect2i();
    Rect2i(const Point2i& top_left, const Vector2i& size);
    Rect2i(int x, int y, int w, int h);

public:
   Point2i topLeft() const;
   Point2i topRight() const;
   Point2i bottomLeft() const;
   Point2i bottomRight() const;

   Vector2i size() const;

public:
    int x, y, w, h;
};
VI_GE_NS_END