#pragma once

#include "math_global.hpp"

#include <algorithm>
#include <cstdint>

#include "Point2.hpp"
#include "Vector2.hpp"

VI_MATH_NS_BEGIN

/**
 * @brief A class representing a rectangle in 2D space, defined by its top-left corner and size.
 *        The rectangle is axis-aligned, meaning its edges are parallel to the coordinate axes.
 * @tparam T Only accepts float double and integers(not boolean)
 */
template <typename T>
class Rect2 {
  public:
    using value_type = T;

  public:
        /**
         * @brief Construct a zero-sized rectangle at origin.
         */
    constexpr Rect2()
      : x(T())
      , y(T())
      , w(T())
      , h(T())
    {}

        /**
         * @brief Construct a rectangle from corner and size.
         * @param corner Top-left style corner.
         * @param size Rectangle size vector.
         */
    constexpr Rect2(const Point2<T>& corner, const Vector2<T>& size)
      : x(corner.x)
      , y(corner.y)
      , w(size.x)
      , h(size.y)
    {}

        /**
         * @brief Construct a rectangle from raw components.
         * @param xx X coordinate.
         * @param yy Y coordinate.
         * @param ww Width.
         * @param hh Height.
         */
    constexpr Rect2(T xx, T yy, T ww, T hh)
      : x(xx)
      , y(yy)
      , w(ww)
      , h(hh)
    {}

  public:
        /**
         * @brief Get top boundary value.
         * @return Maximum y boundary.
         */
    [[nodiscard]]
    constexpr T top() const
    {
        return (std::max<T>)(y, y + h);
    }

    /**
     * @brief Get bottom boundary value.
     * @return Minimum y boundary.
     */
    [[nodiscard]]
    constexpr T bottom() const
    {
        return (std::min<T>)(y, y + h);
    }

    /**
     * @brief Get left boundary value.
     * @return Minimum x boundary.
     */
    [[nodiscard]]
    constexpr T left() const
    {
        return (std::min<T>)(x, x + w);
    }

    /**
     * @brief Get right boundary value.
     * @return Maximum x boundary.
     */
    [[nodiscard]]
    constexpr T right() const
    {
        return (std::max<T>)(x, x + w);
    }

    /**
     * @brief Get minimum corner of this rectangle.
     * @return Bottom-left corner.
     */
    [[nodiscard]]
    constexpr Point2<T> bottomLeft() const
    {
        return Point2<T>((std::min<T>)(x, x + w), (std::min<T>)(y, y + h));
    }

    /**
     * @brief Get bottom-right corner.
     * @return Bottom-right corner point.
     */
    [[nodiscard]]
    constexpr Point2<T> bottomRight() const
    {
        return Point2<T>((std::max<T>)(x, x + w), (std::min<T>)(y, y + h));
    }

    /**
     * @brief Get top-left corner.
     * @return Top-left corner point.
     */
    [[nodiscard]]
    constexpr Point2<T> topLeft() const
    {
        return Point2<T>((std::min<T>)(x, x + w), (std::max<T>)(y, y + h));
    }

    /**
     * @brief Get maximum corner of this rectangle.
     * @return Top-right corner.
     */
    [[nodiscard]]
    constexpr Point2<T> topRight() const
    {
        return Point2<T>((std::max<T>)(x, x + w), (std::max<T>)(y, y + h));
    }

    /**
     * @brief Get width component.
     * @return Stored width.
     */
    [[nodiscard]]
    constexpr T width() const
    {
        return w;
    }

    /**
     * @brief Get height component.
     * @return Stored height.
     */
    [[nodiscard]]
    constexpr T height() const
    {
        return h;
    }

    /**
     * @brief Get size vector.
     * @return Width/height vector.
     */
    [[nodiscard]]
    constexpr Vector2<T> size() const
    {
        return Vector2<T>(w, h);
    }

    /**
     * @brief Check whether a point coordinate lies inside rectangle.
     * @param x X coordinate to test.
     * @param y Y coordinate to test.
     * @return True when point is inside or on boundary.
     */
    [[nodiscard]]
    bool contains(T x, T y) const;
    /**
     * @brief Check whether a point lies inside rectangle.
     * @param pt Point to test.
     * @return True when point is inside or on boundary.
     */
    [[nodiscard]]
    bool contains(const Point2<T>& pt) const;

    /**
     * @brief Expand rectangle to include a point offset.
     * @param pt Point or offset used for expansion.
     */
    void expandBy(const Vector2<T>& pt);
    /**
     * @brief Expand rectangle to include another rectangle.
     * @param rect Rectangle to include.
     */
    void expandBy(const Rect2<T>& rect);

    /**
     * @brief Compute intersection with another rectangle.
     * @param rect Other rectangle.
     * @return Overlapping rectangle.
     */
    [[nodiscard]]
    Rect2<T> intersectWith(const Rect2<T>& rect) const;

    /**
     * @brief Check whether size is exactly zero.
     * @return True when width and height are zero.
     */
    [[nodiscard]]
    constexpr bool isZero() const
    {
        return w == T() && h == T();
    }

    /**
     * @brief Check whether size is near zero.
     * @param eps Tolerance used for comparison.
     * @return True when width and height are within tolerance of zero.
     */
    [[nodiscard]]
    constexpr bool isZero(T eps) const requires(Real<T>)
    {
        return math::isZero<T>(w, eps) && math::isZero<T>(h, eps);
    }

    /**
     * @brief Equality operator.
     * @param right Right-hand rectangle.
     * @return True when all components are equal.
     */
    [[nodiscard]]
    constexpr bool operator==(const Rect2<T>& right) const
    {
        return x == right.x && y == right.y && w == right.w && h == right.h;
    }

    /**
     * @brief Inequality operator.
     * @param right Right-hand rectangle.
     * @return True when any component differs.
     */
    [[nodiscard]]
    constexpr bool operator!=(const Rect2<T>& right) const
    {
        return !(*this == right);
    }

  public:
    union
    {
        struct {
            T x, y, w, h;
        };

        T data[4];
    };
};

using Rect2i = Rect2<int32_t>;
using Rect2f = Rect2<float>;
using Rect2d = Rect2<double>;

VI_MATH_NS_END
