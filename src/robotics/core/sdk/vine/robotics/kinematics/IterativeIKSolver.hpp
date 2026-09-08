#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <vine/robotics/kinematics/IKSolver.hpp>

V_ROBOTICS_KINEMATICS_NS_BEGIN

class V_ROBOTICS_CORE_API IterativeIKSolver : public IKSolver {

    using IKSolver::IKSolver;
};

V_ROBOTICS_KINEMATICS_NS_END