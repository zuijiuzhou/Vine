#pragma once

#include <vine/math/Matrix4x4.hpp>
#include <vsg/maths/mat4.h>

V_VSG_NS_BEGIN

namespace detail
{

/**
 * @brief Converts a Vine Mat4d to a vsg::dmat4.
 *
 * Both are column-major; the 16-element array is column 0..3.
 *
 * @param m Vine matrix.
 * @return Equivalent vsg matrix.
 */
inline ::vsg::dmat4 toVsg(const vine::math::Mat4d& m)
{
    double v[16];
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            v[col * 4 + row] = m(row, col);
        }
    }
    return ::vsg::dmat4(v);
}

}  // namespace detail

V_VSG_NS_END
