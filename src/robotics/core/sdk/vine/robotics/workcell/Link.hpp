#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <memory>

#include <vine/String.hpp>
#include <vine/robotics/kinematics/Frame.hpp> 

#include "Collision.hpp"
#include "RigidBody.hpp"
#include "Visual.hpp"

V_ROBOTICS_WORKCELL_NS_BEGIN

/**
 * @brief A kinematic link of a Device.
 *
 * A link is a rigid body that participates in the device's kinematics. It is
 * owned by a Device through a smart pointer and is deliberately NOT a
 * SceneObject: the workcell manages device-level objects only, so per-link
 * objects never appear in the scene.
 */
class Link
{
  public:
    /**
     * @brief Constructs a link with the given name.
     *
     * @param name The link name.
     */
    explicit Link(const String& name)
      : name_(name)
      , base_frame_(std::make_unique<kinematics::Frame>())
    {}

    /**
     * @brief Returns the link name.
     *
     * @return The name.
     */
    const String& name() const
    {
        return name_;
    }

    /**
     * @brief Returns the link's base coordinate frame.
     *
     * The link's body geometry (visuals, collisions, auxiliary frames) is
     * expressed relative to this frame; its fixed transform places the body
     * within the kinematic chain.
     *
     * @return The base frame.
     */
    kinematics::Frame* baseFrame()
    {
        return base_frame_.get();
    }

    /**
     * @brief Returns the link's base coordinate frame.
     *
     * @return The base frame.
     */
    const kinematics::Frame* baseFrame() const
    {
        return base_frame_.get();
    }

    /**
     * @brief Returns the link's rigid body (geometry + frames).
     *
     * @return The rigid body.
     */
    RigidBody& body()
    {
        return body_;
    }

    /**
     * @brief Returns the link's rigid body (geometry + frames).
     *
     * @return The rigid body.
     */
    const RigidBody& body() const
    {
        return body_;
    }

  private:
    String                         name_;
    std::unique_ptr<kinematics::Frame> base_frame_;
    RigidBody                      body_;
};

V_ROBOTICS_WORKCELL_NS_END
