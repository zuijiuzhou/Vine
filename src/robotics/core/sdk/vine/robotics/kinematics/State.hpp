#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <cstddef>
#include <unordered_map>
 
#include "QState.hpp"

V_ROBOTICS_KINEMATICS_NS_BEGIN

/**
 * @brief Scene state: the joint-space state of every device in a scene.
 *
 * The scene itself is stateless; a State is passed in from outside and
 * carries one QState per device, keyed by the device's base frame. A device
 * reads and writes its joint values through the QState of its own base frame.
 */
class State
{
  public:
    /**
     * @brief Constructs an empty scene state.
     */
    State() = default;

    /**
     * @brief Returns the QState of the device rooted at root, creating it
     *        when absent.
     *
     * @param root The device's base frame.
     * @return The device's QState.
     */
    QState& qstate(const Frame* root)
    {
        return qstate_;
    }

    

  private:

  QState qstate_;
};

V_ROBOTICS_KINEMATICS_NS_END
