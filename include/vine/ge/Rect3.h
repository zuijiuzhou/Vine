﻿#pragma once

#include "ge_global.h"

#include <cstdint>

VI_GE_NS_BEGIN
template <typename T> class Point3;
template <typename T> class Vector3;
template <typename T> class Rect3 {
  public:
    Rect3<T>();
    Rect3<T>(const Point3<T>& top_left, const Vector3<T>& size);
    Rect3<T>(T xx, T yy, T zz, T ll, T ww, T hh);

  public:
    Point3<T> lowerBound() const;
    Point3<T> upperBound() const;


    T lenght() const;
    T width() const;
    T height() const;

    Vector3<T> size() const;

    bool contains(T x, T y, T z) const;
    bool contains(const Point3<T>& pt) const;

    void expandBy(const Point3<T>& pt);
    void expandBy(const Rect3<T>& rect);

    Rect3<T> intersectWith(const Rect3<T>& rect);


    bool operator==(const Rect3<T>& right) const;
    bool operator!=(const Rect3<T>& right) const;

  public:
    T x, y, z, l, w, h;
};

using Rect3i = Rect3<int32_t>;
using Rect3f = Rect3<float>;
using Rect3d = Rect3<double>;

VI_GE_NS_END