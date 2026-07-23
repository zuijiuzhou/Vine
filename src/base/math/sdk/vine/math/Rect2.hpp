#pragma once

#include "math_global.hpp"

#include <algorithm>
#include <cstdint>

#include "Point2.hpp"
#include "Vector2.hpp"

V_MATH_NS_BEGIN

/**
 * @brief Axis-aligned 2D rectangle, defined by its minimum and maximum corners.
 *
 * The rectangle stores raw (xmin, ymin, xmax, ymax) without enforcing
 * ordering.  Two categories of members are provided:
 *
 * - **Raw accessors** (min(), max(), width(), height(), size(), center()):
 *   return stored values as-is.  Extents may be negative when the
 *   rectangle is inverted (max < min) — check with isNeg().
 *
 * - **Normalizing methods** (contains(), expandBy(), intersectWith()):
 *   internally normalize min/max before computing, producing correct
 *   results even when the stored bounds are inverted.
 *
 * @tparam T Scalar type (float, double, integer).
 */
template <typename T>
class Rect2 {
  public:
    using value_type = T;

    /**
     * @brief Construct a zero-sized rectangle at the origin.
     */
    constexpr Rect2()
      : xmin(T())
      , ymin(T())
      , xmax(T())
      , ymax(T())
    {}

    /**
     * @brief Construct from minimum and maximum corners.
     * @param minPt Minimum corner (lower-left in math coords).
     * @param maxPt Maximum corner (upper-right in math coords).
     */
    constexpr Rect2(const Point2<T>& minPt, const Point2<T>& maxPt)
      : xmin(minPt.x)
      , ymin(minPt.y)
      , xmax(maxPt.x)
      , ymax(maxPt.y)
    {}

    /**
     * @brief Convenience: construct from origin point and size vector.
     * @param origin Minimum corner.
     * @param size   Non-negative extents.
     */
    constexpr Rect2(const Point2<T>& origin, const Vector2<T>& size)
      : xmin(origin.x)
      , ymin(origin.y)
      , xmax(origin.x + size.x)
      , ymax(origin.y + size.y)
    {}

    /**
     * @brief Construct from raw scalar components.
     * @param x0 Minimum X.
     * @param y0 Minimum Y.
     * @param x1 Maximum X.
     * @param y1 Maximum Y.
     */
    constexpr Rect2(T x0, T y0, T x1, T y1)
      : xmin(x0)
      , ymin(y0)
      , xmax(x1)
      , ymax(y1)
    {}

    /* ---- accessors ---- */

    /**
     * @name Raw accessors
     *
     * These return the stored values as-is.  When the rectangle is in a
     * valid state (min ≤ max), the results are the expected geometric
     * quantities.  When the rectangle is inverted (max < min), the
     * extents may be negative — use isNeg() to check.
     * @{
     */

    /** @brief Minimum corner (left, bottom in math Y-up). */
    [[nodiscard]]
    constexpr Point2<T> min() const { return Point2<T>(xmin, ymin); }

    /** @brief Maximum corner (right, top in math Y-up). */
    [[nodiscard]]
    constexpr Point2<T> max() const { return Point2<T>(xmax, ymax); }

    /** @brief Width (xmax - xmin), negative if inverted. */
    [[nodiscard]]
    constexpr T width() const { return xmax - xmin; }

    /** @brief Height (ymax - ymin), negative if inverted. */
    [[nodiscard]]
    constexpr T height() const { return ymax - ymin; }

    /** @brief Size vector, components may be negative if inverted. */
    [[nodiscard]]
    constexpr Vector2<T> size() const { return Vector2<T>(width(), height()); }

    /** @brief Center point. */
    [[nodiscard]]
    constexpr Point2<T> center() const
    {
        return Point2<T>((xmin + xmax) / T(2), (ymin + ymax) / T(2));
    }

    /** @} */

    /* ---- queries ---- */

    /**
     * @name Defensively-normalizing methods
     *
     * These methods internally normalize the rectangle (min/max ordering)
     * before performing their computation, so they produce correct results
     * even when the stored bounds are inverted.
     * @{
     */

    /**
     * @brief Check whether the rectangle has negative extent (max < min in any axis).
     */
    [[nodiscard]]
    constexpr bool isNeg() const { return xmax < xmin || ymax < ymin; }

    /**
     * @brief Check whether the rectangle has exactly zero area.
     */
    [[nodiscard]]
    constexpr bool isZero() const { return xmax == xmin && ymax == ymin; }

    /**
     * @brief Check whether the rectangle has near-zero area.
     */
    [[nodiscard]]
    constexpr bool isZero(T eps) const requires(Real<T>)
    {
        return math::isZero<T>(width(), eps) && math::isZero<T>(height(), eps);
    }

    /**
     * @brief Check whether a point lies inside the rectangle (inclusive).
     *
     * Normalizes min/max internally; works correctly for inverted rectangles.
     */
    [[nodiscard]]
    bool contains(T x, T y) const;

    /**
     * @brief Check whether a point lies inside the rectangle (inclusive).
     *
     * Normalizes min/max internally; works correctly for inverted rectangles.
     */
    [[nodiscard]]
    bool contains(const Point2<T>& pt) const;

    /* ---- modifiers ---- */

    /**
     * @brief Expand to include a point.
     *
     * Normalizes bounds internally before expanding.
     */
    void expandBy(const Point2<T>& pt);

    /**
     * @brief Expand to include another rectangle.
     *
     * Normalizes both rectangles internally before expanding.
     */
    void expandBy(const Rect2<T>& rect);

    /**
     * @brief Compute intersection with another rectangle.
     *
     * Normalizes both rectangles internally.  The output rectangle is
     * always normalized (min ≤ max).
     *
     * @param rect Other rectangle.
     * @param out  Normalized output.
     *             When disjoint, represents the gap between the two rectangles.
     * @return true if intersecting, false if disjoint.
     */
    bool intersectWith(const Rect2<T>& rect, Rect2<T>& out) const;

    /** @} */

    /* ---- operators ---- */

    [[nodiscard]]
    constexpr bool operator==(const Rect2<T>& r) const
    {
        return xmin == r.xmin && ymin == r.ymin && xmax == r.xmax && ymax == r.ymax;
    }

    [[nodiscard]]
    constexpr bool operator!=(const Rect2<T>& r) const { return !(*this == r); }

  public:
    union {
        struct {
            T xmin, ymin, xmax, ymax;
        };
        T data[4];
    };
};

using Rect2i = Rect2<int32_t>;
using Rect2f = Rect2<float>;
using Rect2d = Rect2<double>;

V_MATH_NS_END
