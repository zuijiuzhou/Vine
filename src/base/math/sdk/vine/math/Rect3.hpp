#pragma once

#include "math_global.hpp"

#include <cstdint>

#include "Math.hpp"

VI_MATH_NS_BEGIN
template <typename T>
class Point3;
template <typename T>
class Vector3;

/**
 * @brief A class representing a rectangle in 3D space, defined by its lower bound corner and size.
 *        The rectangle is axis-aligned, meaning its edges are parallel to the coordinate axes.
 * @tparam T Only accepts float double and integers(not boolean)
 */
template <typename T>
class Rect3 {
  public:
    using value_type = T;

  public:
    constexpr Rect3()
      : x(T())
      , y(T())
      , z(T())
      , l(T())
      , w(T())
      , h(T())
    {}

    constexpr Rect3(const Point3<T>& corner, const Vector3<T>& size)
      : x(corner.x)
      , y(corner.y)
      , z(corner.z)
      , l(size.x)
      , w(size.y)
      , h(size.z)
    {}

    constexpr Rect3(T xx, T yy, T zz, T ll, T ww, T hh)
      : x(xx)
      , y(yy)
      , z(zz)
      , l(ll)
      , w(ww)
      , h(hh)
    {}

  public:
    [[nodiscard]]
    constexpr Point3<T> lowerBound() const
    {
        return Point3<T>(std::min<T>(x, x + l), std::min<T>(y, y + w), std::min<T>(z, z + h));
    }

    [[nodiscard]]
    constexpr Point3<T> upperBound() const
    {
        return Point3<T>(std::max<T>(x, x + l), std::max<T>(y, y + w), std::max<T>(z, z + h));
    }

    [[nodiscard]]
    constexpr T length() const
    {
        return l < 0 ? -l : l;
    }

    [[nodiscard]]
    constexpr T width() const
    {
        return w < 0 ? -w : w;
    }

    [[nodiscard]]
    constexpr T height() const
    {
        return h < 0 ? -h : h;
    }

    [[nodiscard]]
    constexpr Vector3<T> size() const
    {
        return Vector3<T>(l < 0 ? -l : l, w < 0 ? -w : w, h < 0 ? -h : h);
    }

    [[nodiscard]]
    bool contains(T x, T y, T z) const;
    [[nodiscard]]
    bool contains(const Point3<T>& pt) const;

    void expandBy(const Point3<T>& pt);
    void expandBy(const Rect3<T>& rect);

    [[nodiscard]]
    Rect3<T> intersectWith(const Rect3<T>& rect) const;

    [[nodiscard]]
    constexpr bool isZero() const
    {
        return l == T() && w == T() && h == T();
    }

    [[nodiscard]]
    constexpr bool isZero(T eps) const
    {
        return math::isZero<T>(l, eps) && math::isZero<T>(w, eps) && math::isZero<T>(h, eps);
    }

    [[nodiscard]]
    constexpr bool operator==(const Rect3<T>& right) const
    {
        return x == right.x && y == right.y && z == right.z && l == right.l && w == right.w && h == right.h;
    }

    [[nodiscard]]
    constexpr bool operator!=(const Rect3<T>& right) const
    {
        return !(*this == right);
    }

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
