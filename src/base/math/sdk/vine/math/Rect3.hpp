#pragma once

#include "math_global.hpp"

#include <cstdint>
#include <algorithm>

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
    /**
     * @brief Construct a zero-sized box at origin.
     */
    constexpr Rect3()
      : x(T())
      , y(T())
      , z(T())
      , l(T())
      , w(T())
      , h(T())
    {}

    /**
     * @brief Construct a box from corner and size.
     * @param corner Lower bound corner.
     * @param size Box size vector.
     */
    constexpr Rect3(const Point3<T>& corner, const Vector3<T>& size)
      : x(corner.x)
      , y(corner.y)
      , z(corner.z)
      , l(size.x)
      , w(size.y)
      , h(size.z)
    {}

    /**
     * @brief Construct a box from raw components.
     * @param xx X coordinate.
     * @param yy Y coordinate.
     * @param zz Z coordinate.
     * @param ll Length.
     * @param ww Width.
     * @param hh Height.
     */
    constexpr Rect3(T xx, T yy, T zz, T ll, T ww, T hh)
      : x(xx)
      , y(yy)
      , z(zz)
      , l(ll)
      , w(ww)
      , h(hh)
    {}

  public:
    /**
     * @brief Get lower bound corner.
     * @return Point with minimum x/y/z values.
     */
    [[nodiscard]]
    constexpr Point3<T> lowerBound() const
    {
        return Point3<T>(std::min<T>(x, x + l), std::min<T>(y, y + w), std::min<T>(z, z + h));
    }

    /**
     * @brief Get upper bound corner.
     * @return Point with maximum x/y/z values.
     */
    [[nodiscard]]
    constexpr Point3<T> upperBound() const
    {
        return Point3<T>(std::max<T>(x, x + l), std::max<T>(y, y + w), std::max<T>(z, z + h));
    }

    /**
     * @brief Get absolute length.
     * @return Non-negative length.
     */
    [[nodiscard]]
    constexpr T length() const
    {
        return l < 0 ? -l : l;
    }

    /**
     * @brief Get absolute width.
     * @return Non-negative width.
     */
    [[nodiscard]]
    constexpr T width() const
    {
        return w < 0 ? -w : w;
    }

    /**
     * @brief Get absolute height.
     * @return Non-negative height.
     */
    [[nodiscard]]
    constexpr T height() const
    {
        return h < 0 ? -h : h;
    }

    /**
     * @brief Get absolute size vector.
     * @return Non-negative size components.
     */
    [[nodiscard]]
    constexpr Vector3<T> size() const
    {
        return Vector3<T>(l < 0 ? -l : l, w < 0 ? -w : w, h < 0 ? -h : h);
    }

    /**
     * @brief Check whether a coordinate lies inside box.
     * @param x X coordinate to test.
     * @param y Y coordinate to test.
     * @param z Z coordinate to test.
     * @return True when point is inside or on boundary.
     */
    [[nodiscard]]
    bool contains(T x, T y, T z) const;
    /**
     * @brief Check whether a point lies inside box.
     * @param pt Point to test.
     * @return True when point is inside or on boundary.
     */
    [[nodiscard]]
    bool contains(const Point3<T>& pt) const;

    /**
     * @brief Expand box to include a point.
     * @param pt Point to include.
     */
    void expandBy(const Point3<T>& pt);
    /**
     * @brief Expand box to include another box.
     * @param rect Box to include.
     */
    void expandBy(const Rect3<T>& rect);

    /**
     * @brief Compute intersection with another box.
     * @param rect Other box.
     * @return Overlapping box.
     */
    [[nodiscard]]
    Rect3<T> intersectWith(const Rect3<T>& rect) const;

    /**
     * @brief Check whether size is exactly zero.
     * @return True when length, width and height are zero.
     */
    [[nodiscard]]
    constexpr bool isZero() const
    {
        return l == T() && w == T() && h == T();
    }

    /**
     * @brief Check whether size is near zero.
     * @param eps Tolerance used for comparison.
     * @return True when all size components are within tolerance of zero.
     */
    [[nodiscard]]
    constexpr bool isZero(T eps) const
    {
        return math::isZero<T>(l, eps) && math::isZero<T>(w, eps) && math::isZero<T>(h, eps);
    }

    /**
     * @brief Equality operator.
     * @param right Right-hand box.
     * @return True when all components are equal.
     */
    [[nodiscard]]
    constexpr bool operator==(const Rect3<T>& right) const
    {
        return x == right.x && y == right.y && z == right.z && l == right.l && w == right.w && h == right.h;
    }

    /**
     * @brief Inequality operator.
     * @param right Right-hand box.
     * @return True when any component differs.
     */
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
