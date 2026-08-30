#pragma once

#include "geometry_global.hpp"

#include <cstdint>
#include <vector>

#include <vine/math/Point2.hpp>
#include <vine/math/Vector3.hpp>

V_GEOMETRY_NS_BEGIN

using Vec3fArray  = std::vector<vine::math::Vec3f>;
using Vec3dArray  = std::vector<vine::math::Vec3d>;
using Vec2fArray  = std::vector<vine::math::Vec2f>;
using Vec2dArray  = std::vector<vine::math::Vec2d>;
using UInt32Array = std::vector<std::uint32_t>;

V_GEOMETRY_NS_END
