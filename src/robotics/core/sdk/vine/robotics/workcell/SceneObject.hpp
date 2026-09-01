#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <memory>
#include <vector>

#include <vine/String.hpp>
#include <vine/robotics/kinematics/Frame.hpp>

V_ROBOTICS_WORKCELL_NS_BEGIN

/**
 * @brief Kind of a scene object.
 */
enum class SceneObjectKind
{
    Device,
    RigidObject,
};

/**
 * @brief Base class of user-managed objects in a Workcell.
 *
 * A SceneObject is something the user adds/removes/finds in the workcell
 * (a robot, a scanner, a part, a table, ...). The Workcell owns every
 * SceneObject (unique ownership); this base exposes identity plus each
 * object's own coordinate frames.
 *
 * Every SceneObject creates a base_frame (held by unique_ptr): the root of
 * the object's own coordinate system, and a frames_ registry (raw pointers)
 * of every frame the object carries — its base frame plus the joint and end
 * frames added by derived classes. Parent/child relationships between
 * objects are maintained by linking these frames — Frame is an IHierarchyNode
 * with parent/children — so the scene graph is the frame tree.
 *
 * Link is deliberately NOT a SceneObject: it is a Device-internal kinematic
 * body, so the workcell does not manage per-link objects.
 */
class SceneObject
{
  public:
    /**
     * @brief Destroys the scene object.
     */
    virtual ~SceneObject() = default;

    /**
     * @brief Returns the object kind.
     *
     * @return The kind.
     */
    virtual SceneObjectKind kind() const = 0;

    /**
     * @brief Returns the object name (unique within a Workcell).
     *
     * @return The name.
     */
    virtual const String& name() const = 0;

    /**
     * @brief Returns the object's base coordinate frame.
     *
     * This frame is the root of the object's own coordinate system; its fixed
     * transform is the object pose relative to its parent frame.
     *
     * @return The base frame.
     */
    kinematics::Frame* baseFrame()
    {
        return base_frame_.get();
    }

    /**
     * @brief Returns the object's base coordinate frame.
     *
     * @return The base frame.
     */
    const kinematics::Frame* baseFrame() const
    {
        return base_frame_.get();
    }

    /**
     * @brief Returns every frame of this object: base + joints + ends.
     *
     * The pointers are non-owning; the frames are owned by this object and
     * remain valid while the object lives.
     *
     * @return The frames.
     */
    const std::vector<kinematics::Frame*>& frames() const
    {
        return frames_;
    }

  protected:
    /// Creates the base frame and registers it in the frame registry.
    SceneObject()
      : base_frame_(std::make_unique<kinematics::Frame>())
    {
        frames_.push_back(base_frame_.get());
    }

  protected:
    std::unique_ptr<kinematics::Frame> base_frame_;
    std::vector<kinematics::Frame*>    frames_;
};

V_ROBOTICS_WORKCELL_NS_END
