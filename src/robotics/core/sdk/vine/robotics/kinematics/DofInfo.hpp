#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <vine/math/Isometry3.hpp>

V_ROBOTICS_KINEMATICS_NS_BEGIN

enum class DofType
{
    RevoluteJoint,
    /** 1 DoF [x] */
    PrismaticJoint,
};

struct DofInfo {
    math::Isometry3d origin;
    math::Vec3d      axis;
    double           lower{};
    double           upper{};
};

V_ROBOTICS_KINEMATICS_NS_END