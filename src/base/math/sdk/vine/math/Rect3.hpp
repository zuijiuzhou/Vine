#pragma once

#include "math_global.hpp"

#include <cstdint>

VI_MATH_NS_BEGIN
template <typename T>
class Point3;
template <typename T>
class Vector3;

/**
 * @brief
 * @tparam T
 */
template <typename T>
class Rect3 {
  public:
    using value_type = T;

  public:
    Rect3();
    Rect3(const Point3<T>& conner, const Vector3<T>& size);
    Rect3(T xx, T yy, T zz, T ll, T ww, T hh);

  public:
    Point3<T> lowerBound() const;
    Point3<T> upperBound() const;

    T length() const;
    T width() const;
    T height() const;

    Vector3<T> size() const;

    bool contains(T x, T y, T z) const;
    bool contains(const Point3<T>& pt) const;

    void expandBy(const Point3<T>& pt);
    void expandBy(const Rect3<T>& rect);

    Rect3<T> intersectWith(const Rect3<T>& rect) const;

    bool isZero() const;
    bool isZero(T eps) const;

    bool operator==(const Rect3<T>& right) const;
    bool operator!=(const Rect3<T>& right) const;

  public:
    union
    {
        struct {
            T x, y, z, l, w, h;
        };

        T data[6];
    };
};

using Rect3i = Rect3<int32_t>;
using Rect3f = Rect3<float>;
using Rect3d = Rect3<double>;

VI_MATH_NS_END