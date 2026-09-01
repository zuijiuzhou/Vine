#pragma once
#include "graphics_global.hpp"

#include <vine/math/Vector3.hpp>
#include <algorithm>
#include <limits>

V_GRAPHICS_NS_BEGIN

using vine::math::Vec3d;

/**
 * @brief Axis-aligned bounding box in 3D space.
 *
 * Represents the minimum and maximum extents of an object or scene.
 */
struct V_GRAPHICS_API BoundingBox {
    /** Minimum corner (component-wise). */
    Vec3d min{ std::numeric_limits<double>::max(),
               std::numeric_limits<double>::max(),
               std::numeric_limits<double>::max() };

    /** Maximum corner (component-wise). */
    Vec3d max{ std::numeric_limits<double>::lowest(),
               std::numeric_limits<double>::lowest(),
               std::numeric_limits<double>::lowest() };

    /** @brief Returns whether the box is valid (has a non-degenerate extent). */
    bool isValid() const
    {
        return min.x <= max.x && min.y <= max.y && min.z <= max.z;
    }

    /** @brief Returns whether the box is empty (no extents set). */
    bool isEmpty() const
    {
        return min.x > max.x || min.y > max.y || min.z > max.z;
    }

    /** @brief Expands the box to include a point.
     *
     * @param p Point to include.
     */
    void expand(const Vec3d& p)
    {
        min.x = (p.x < min.x) ? p.x : min.x;
        min.y = (p.y < min.y) ? p.y : min.y;
        min.z = (p.z < min.z) ? p.z : min.z;
        max.x = (p.x > max.x) ? p.x : max.x;
        max.y = (p.y > max.y) ? p.y : max.y;
        max.z = (p.z > max.z) ? p.z : max.z;
    }

    /** @brief Expands the box to include another box.
     *
     * @param other Box to merge in.
     */
    void expand(const BoundingBox& other)
    {
        expand(other.min);
        expand(other.max);
    }

    /** @brief Computes the box center. */
    Vec3d center() const
    {
        return (min + max) * 0.5;
    }

    /** @brief Computes the box size (max - min). */
    Vec3d size() const
    {
        return max - min;
    }
};

V_GRAPHICS_NS_END
