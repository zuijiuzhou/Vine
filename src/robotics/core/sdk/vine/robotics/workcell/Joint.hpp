#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <vector>

#include <vine/robotics/kinematics/Frame.hpp>

V_ROBOTICS_WORKCELL_NS_BEGIN

/**
 * @brief A joint connecting two links of a Device.
 *
 * A joint is a kinematic Frame that carries its own degrees of freedom. Each
 * DoF is a DofInfo (motion type, axis, limits, its Q index, ...); the motion
 * is applied per DoF, so a planar joint stays a single Joint with three DoFs
 * (two prismatic + one revolute) instead of three separate joints.
 *
 * Owned by a Device through a smart pointer.
 */
class Joint : public kinematics::Frame
{
  public:
    /**
     * @brief Constructs a fixed joint.
     */
    Joint() = default;

    /**
     * @brief Constructs a joint with the given frame type.
     *
     * @param type The joint frame type.
     */
    explicit Joint(kinematics::FrameType type)
      : Frame(type)
    {}

    /**
     * @brief Destroys the joint.
     */
    ~Joint() override = default;

    /**
     * @brief Returns the joint's degrees of freedom.
     *
     * @return The DoF infos.
     */
    const std::vector<kinematics::DofInfo>& dofInfos() const override
    {
        return dof_infos_;
    }

    /**
     * @brief Sets the joint's degrees of freedom as a whole.
     *
     * @param dof_infos New DoF infos.
     */
    void setDofInfos(const std::vector<kinematics::DofInfo>& dof_infos)
    {
        dof_infos_ = dof_infos;
    }

  private:
    std::vector<kinematics::DofInfo> dof_infos_;
};

V_ROBOTICS_WORKCELL_NS_END
