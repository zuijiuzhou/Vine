#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <vine/SmallVector.hpp>

V_ROBOTICS_KINEMATICS_NS_BEGIN

class V_ROBOTICS_CORE_API Q : public vine::SmallVector<double, 8> {
  public:
    Q() = default;
};

V_ROBOTICS_KINEMATICS_NS_END