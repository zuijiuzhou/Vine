#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <cstddef>
#include <vector>

#include <vine/String.hpp>
#include <vine/math/Isometry3.hpp>
#include <vine/robotics/kinematics/QState.hpp>

#include "Device.hpp"

V_ROBOTICS_WORKCELL_NS_BEGIN

/**
 * @brief A device with actuated kinematics (robot, external axis, ...).
 *
 * A MotionDevice computes forward kinematics over the serial chain of Links
 * and Joints inherited from Device. It is stateless: the joint values are
 * read from a scene State passed in from outside.
 */
class MotionDevice : public Device
{
  public:
    /**
     * @brief Constructs a motion device with the given name.
     *
     * @param name The device name.
     */
    explicit MotionDevice(const String& name)
      : Device(name)
    {}

    /**
     * @brief Destroys the device.
     */
    ~MotionDevice() override = default;


    
  private: 
};

V_ROBOTICS_WORKCELL_NS_END
