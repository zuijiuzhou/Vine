#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <vine/RefObject.hpp>
#include <vine/SmallVector.hpp>
#include <vine/math/Isometry3.hpp>

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

class V_ROBOTICS_CORE_API Frame : public vine::RefObject {
    V_OBJECT_META(Frame, vine::RefObject);

  protected:
    Frame(FrameType type);

  public:
    const String& getName() const
    {
        return name_;
    }

    void setName(const String& name)
    {
        name_ = name;
    }

    FrameType getFrameType() const
    {
        return type_;
    }

    math::Isometry3d getFixedTransform() const
    {
        return fixed_tf_;
    }

    void setFixedTransform(const math::Isometry3d& tf)
    {
        fixed_tf_ = tf;
    }

    virtual math::Isometry3d getTransform() = 0;

  private:
    String           name_;
    FrameType        type_;
    math::Isometry3d fixed_tf_;
    ;
};

V_ROBOTICS_KINEMATICS_NS_END