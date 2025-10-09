#pragma once

#include "ge_global.hpp"

#include <cstdint>

#include "Point2.hpp"
#include "Vector2.hpp"

VI_GE_NS_BEGIN
template <typename T> class Rect2 {
  public:
    Rect2();
    Rect2(const Point2<T>& top_left, const Vector2<T>& size);
    Rect2(T xx, T yy, T ww, T hh);

  public:
    T top() const;
    T bottom() const;
    T left() const;
    T right() const;

    // the min corner
    Point2<T> bottomLeft() const;
    Point2<T> bottomRight() const;
    // the max corner
    Point2<T> topLeft() const;
    Point2<T> topRight() const;

    T width() const;
    T height() const;

    Vector2<T> size() const;

    bool contains(T x, T y) const;
    bool contains(const Point2<T>& pt) const;

    void expandBy(const Vector2<T>& pt);
    void expandBy(const Rect2<T>& rect);

    bool operator==(const Rect2<T>& right) const;
    bool operator!=(const Rect2<T>& right) const;

  public:
    T x, y, w, h;
};

using Rect2i = Rect2<int32_t>;
using Rect2f = Rect2<float>;
using Rect2d = Rect2<double>;

VI_GE_NS_END