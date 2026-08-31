#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <vine/String.hpp> 

#include "Collision.hpp"
#include "RigidBody.hpp"
#include "SceneObject.hpp"
#include "Visual.hpp"



V_ROBOTICS_WORKCELL_NS_BEGIN

/**
 * @brief A standalone rigid body placed in the workcell (table, part, fence,
 *        fixture, ...).
 *
 * A RigidObject is a scene-managed object that has no device semantics: no
 * joints, no degrees of freedom, no kinematics. Its geometry (visuals +
 * collisions + frames) is carried by an embedded RigidBody.
 */
class RigidObject : public SceneObject
{
  public:
    /**
     * @brief Constructs a rigid object with the given name.
     *
     * @param name The object name.
     */
    explicit RigidObject(const String& name)
      : name_(name)
    {}

    /**
     * @brief Returns the object kind.
     *
     * @return SceneObjectKind::RigidObject.
     */
    SceneObjectKind kind() const override
    {
        return SceneObjectKind::RigidObject;
    }

    /**
     * @brief Returns the object name.
     *
     * @return The name.
     */
    const String& name() const override
    {
        return name_;
    }

    /**
     * @brief Returns the rigid body (root frame + geometry).
     *
     * @return The rigid body.
     */
    RigidBody& body()
    {
        return body_;
    }

    /**
     * @brief Returns the rigid body (root frame + geometry).
     *
     * @return The rigid body.
     */
    const RigidBody& body() const
    {
        return body_;
    }

  private:
    String    name_;
    RigidBody body_;
};

V_ROBOTICS_WORKCELL_NS_END
