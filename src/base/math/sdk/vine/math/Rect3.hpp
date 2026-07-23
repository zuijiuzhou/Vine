#pragma once

#include "math_global.hpp"

#include <algorithm>
#include <cstdint>

#include "Math.hpp"

V_MATH_NS_BEGIN
template <typename T>
class Point3;
template <typename T>
class Vector3;

/**
 * @brief Axis-aligned 3D box, defined by its minimum and maximum corners.
 *
 * The box stores raw (xmin, ymin, zmin, xmax, ymax, zmax) without
 * enforcing ordering.  Two categories of members are provided:
 *
 * - **Raw accessors** (min(), max(), length(), width(), height(), size(),
 *   center()): return stored values as-is.  Extents may be negative when
 *   the box is inverted (max < min) — check with isNeg().
 *
 * - **Normalizing methods** (contains(), expandBy(), intersectWith()):
 *   internally normalize min/max before computing, producing correct
 *   results even when the stored bounds are inverted.
 *
 * @tparam T Scalar type (float, double, integer).
 */
template <typename T>
class Rect3 {
  public:
    using value_type = T;

    /**
     * @brief Construct a zero-sized box at the origin.
     */
    constexpr Rect3()
      : xmin(T())
      , ymin(T())
      , zmin(T())
      , xmax(T())
      , ymax(T())
      , zmax(T())
    {}

    /**
     * @brief Construct from minimum and maximum corners.
     * @param minPt Minimum corner.
     * @param maxPt Maximum corner.
     */
    constexpr Rect3(const Point3<T>& minPt, const Point3<T>& maxPt)
      : xmin(minPt.x)
      , ymin(minPt.y)
      , zmin(minPt.z)
      , xmax(maxPt.x)
      , ymax(maxPt.y)
      , zmax(maxPt.z)
    {}

    /**
     * @brief Convenience: construct from origin point and size vector.
     * @param origin Minimum corner.
     * @param size   Non-negative extents.
     */
    constexpr Rect3(const Point3<T>& origin, const Vector3<T>& size)
      : xmin(origin.x)
      , ymin(origin.y)
      , zmin(origin.z)
      , xmax(origin.x + size.x)
      , ymax(origin.y + size.y)
      , zmax(origin.z + size.z)
    {}

    /**
     * @brief Construct from raw scalar components.
     * @param x0 Minimum X.
     * @param y0 Minimum Y.
     * @param z0 Minimum Z.
     * @param x1 Maximum X.
     * @param y1 Maximum Y.
     * @param z1 Maximum Z.
     */
    constexpr Rect3(T x0, T y0, T z0, T x1, T y1, T z1)
      : xmin(x0)
      , ymin(y0)
      , zmin(z0)
      , xmax(x1)
      , ymax(y1)
      , zmax(z1)
    {}

    /* ---- accessors ---- */

    /**
     * @name Raw accessors
     *
     * These return the stored values as-is.  When the box is in a valid
     * state (min ≤ max), the results are the expected geometric
     * quantities.  When the box is inverted (max < min), the extents may
     * be negative — use isNeg() to check.
     * @{
     */

    /** @brief Minimum corner. */
    [[nodiscard]]
    constexpr Point3<T> min() const
    {
        return Point3<T>(xmin, ymin, zmin);
    }

    /** @brief Maximum corner. */
    [[nodiscard]]
    constexpr Point3<T> max() const
    {
        return Point3<T>(xmax, ymax, zmax);
    }

    /** @brief Size along X (xmax - xmin), negative if inverted. */
    [[nodiscard]]
    constexpr T length() const
    {
        return xmax - xmin;
    }

    /** @brief Size along Y (ymax - ymin), negative if inverted. */
    [[nodiscard]]
    constexpr T width() const
    {
        return ymax - ymin;
    }

    /** @brief Size along Z (zmax - zmin), negative if inverted. */
    [[nodiscard]]
    constexpr T height() const
    {
        return zmax - zmin;
    }

    /** @brief Size vector, components may be negative if inverted. */
    [[nodiscard]]
    constexpr Vector3<T> size() const
    {
        return Vector3<T>(length(), width(), height());
    }

    /** @brief Center point. */
    [[nodiscard]]
    constexpr Point3<T> center() const
    {
        return Point3<T>((xmin + xmax) / T(2), (ymin + ymax) / T(2), (zmin + zmax) / T(2));
    }

    /** @} */

    /* ---- queries ---- */

    /**
     * @name Defensively-normalizing methods
     *
     * These methods internally normalize the box (min/max ordering)
     * before performing their computation, so they produce correct results
     * even when the stored bounds are inverted.
     * @{
     */

    /**
     * @brief Check whether the box has negative extent (max < min in any axis).
     */
    [[nodiscard]]
    constexpr bool isNeg() const
    {
        return xmax < xmin || ymax < ymin || zmax < zmin;
    }

    /**
     * @brief Check whether the box has exactly zero volume.
     */
    [[nodiscard]]
    constexpr bool isZero() const
    {
        return xmax == xmin && ymax == ymin && zmax == zmin;
    }

    /**
     * @brief Check whether the box has near-zero volume.
     */
    [[nodiscard]]
    constexpr bool isZero(T eps) const requires(Real<T>)
    {
        return math::isZero<T>(length(), eps) && math::isZero<T>(width(), eps) && math::isZero<T>(height(), eps);
    }

    /**
     * @brief Check whether a point lies inside the box (inclusive).
     *
     * Normalizes min/max internally; works correctly for inverted boxes.
     */
    [[nodiscard]]
    bool contains(T x, T y, T z) const;

    /**
     * @brief Check whether a point lies inside the box (inclusive).
     *
     * Normalizes min/max internally; works correctly for inverted boxes.
     */
    [[nodiscard]]
    bool contains(const Point3<T>& pt) const;

    /* ---- modifiers ---- */

    /**
     * @brief Expand to include a point.
     *
     * Normalizes bounds internally before expanding.
     */
    void expandBy(const Point3<T>& pt);

    /**
     * @brief Expand to include another box.
     *
     * Normalizes both boxes internally before expanding.
     */
    void expandBy(const Rect3<T>& rect);

    /**
     * @brief Compute intersection with another box.
     *
     * Normalizes both boxes internally.  The output box is always
     * normalized (min ≤ max).
     *
     * @param rect Other box.
     * @param out  Normalized output.
     *             When disjoint, represents the gap between the two boxes.
     * @return true if intersecting, false if disjoint.
     */
    bool intersectWith(const Rect3<T>& rect, Rect3<T>& out) const;

    /** @} */

    /* ---- operators ---- */

    [[nodiscard]]
    constexpr bool operator==(const Rect3<T>& r) const
    {
        return xmin == r.xmin && ymin == r.ymin && zmin == r.zmin && xmax == r.xmax && ymax == r.ymax && zmax == r.zmax;
    }

    [[nodiscard]]
    constexpr bool operator!=(const Rect3<T>& r) const
    {
        return !(*this == r);
    }

  public:
    union
    {
        struct {
            T xmin, ymin, zmin, xmax, ymax, zmax;
        };

        T data[6];
    };
};

using Rect3i = Rect3<int32_t>;
using Rect3f = Rect3<float>;
using Rect3d = Rect3<double>;

V_MATH_NS_END
