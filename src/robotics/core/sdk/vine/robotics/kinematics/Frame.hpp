#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <cstddef>
#include <vector>

#include <vine/ITreeNode.hpp>
#include <vine/Object.hpp>
#include <vine/SmallVector.hpp>
#include <vine/String.hpp>
#include <vine/math/Isometry3.hpp>
#include <vine/robotics/kinematics/DofInfo.hpp>

V_ROBOTICS_KINEMATICS_NS_BEGIN

enum class FrameType
{
    /** 0 DoF */
    Fixed = 0,
    /** 1 DoF [theta] */
    RevoluteJoint,
    /** 1 DoF [x] */
    PrismaticJoint,
    /** 3 DoF [x y theta] */
    PlanarJoint
};

/**
 * @brief Kinematic frame (joint coordinate system) of a kinematic chain.
 *
 * Frames form a tree through parent/child links. The ancestor algorithm is
 * inherited from ITreeNode and reused instead of being re-implemented. The
 * tree links are non-owning: frames are owned externally (e.g. by the robot
 * model) and must outlive the tree that references them.
 */
class V_ROBOTICS_CORE_API Frame : public vine::Object, public vine::ITreeNode<Frame> {
    V_OBJECT_META(Frame, vine::Object);

    // 构造函数区块
  public:
    Frame();

  protected:
    Frame(FrameType type);

    // 方法区块
  public:
    const String& name() const
    {
        return name_;
    }

    void setName(const String& name)
    {
        name_ = name;
    }

    FrameType frameType() const
    {
        return type_;
    }

    math::Isometry3d fixedTransform() const
    {
        return fixed_tf_;
    }

    void setFixedTransform(const math::Isometry3d& tf)
    {
        fixed_tf_ = tf;
    }

    virtual math::Isometry3d transform()
    {
        return fixed_tf_;
    };

    virtual const std::vector<DofInfo>& dofInfos() const
    {
        static std::vector<DofInfo> empty;
        return empty;
    }

    /**
     * @brief Returns the parent frame.
     *
     * @return The parent frame, or null for a root frame.
     */
    Frame* parent() const noexcept override
    {
        return parent_;
    }

    /**
     * @brief Returns the number of child frames.
     *
     * @return The child count.
     */
    std::size_t childCount() const noexcept override
    {
        return children_.size();
    }

    /**
     * @brief Returns the child frame at the given index.
     *
     * @param index The child index.
     * @return The child frame, or null when the index is out of range.
     */
    Frame* childAt(std::size_t index) const override
    {
        return index < children_.size() ? children_[index] : nullptr;
    }

    /**
     * @brief Appends a child frame.
     *
     * The child must be non-null, must not be this frame and must not
     * already have a parent.
     *
     * @param child The child frame to append.
     * @throws std::invalid_argument on invalid input.
     */
    void addChild(Frame* child);

    /**
     * @brief Removes a direct child frame.
     *
     * @param child The direct child frame to remove.
     * @throws std::invalid_argument when child is not a direct child.
     */
    void removeChild(Frame* child);

    /**
     * @brief Returns the direct child frames.
     *
     * @return Read-only reference to the children.
     */
    const std::vector<Frame*>& children() const noexcept
    {
        return children_;
    }

    /**
     * @brief Checks whether this frame is a root frame.
     *
     * @return true when the frame has no parent.
     */
    bool isRoot() const noexcept
    {
        return parent_ == nullptr;
    }

    // 字段区块
  private:
    String              name_;
    FrameType           type_{ FrameType::Fixed };
    math::Isometry3d    fixed_tf_;
    Frame*              parent_{ nullptr };
    std::vector<Frame*> children_;
};

V_ROBOTICS_KINEMATICS_NS_END