#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <vine/String.hpp>
#include <vine/math/Isometry3.hpp>
#include <vine/robotics/workcell/Device.hpp>
#include <vine/robotics/workcell/RigidBody.hpp>

V_ROBOTICS_WORKCELL_NS_BEGIN

/**
 * @brief A measurement device (laser scanner, touch probe, camera, ...).
 *
 * A Scanner is a Device whose body is a RigidBody (root frame + visuals +
 * collisions) and which defines a measurement reference frame. It is usually
 * mounted on a motion device (e.g. a robot TCP), so its measurement frame
 * follows the parent's motion through the mount cascade.
 */
class Scanner : public Device
{
  public:
    /**
     * @brief Constructs a scanner with the given name.
     *
     * @param name The device name.
     */
    explicit Scanner(const String& name)
      : Device(name)
    {}

    /**
     * @brief Destroys the scanner.
     */
    ~Scanner() override = default;

    

  private:
    RigidBody          body_;
    math::Isometry3d   measurement_pose_;
};

V_ROBOTICS_WORKCELL_NS_END
