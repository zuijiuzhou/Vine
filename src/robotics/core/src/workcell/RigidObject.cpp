#include <vine/robotics/workcell/RigidObject.hpp>

V_ROBOTICS_WORKCELL_NS_BEGIN

RigidObject::RigidObject(const String& name)
  : SceneObject(name)
{}

RigidObject::~RigidObject() = default;

SceneObjectKind RigidObject::kind() const
{
    return SceneObjectKind::RigidObject;
}

V_ROBOTICS_WORKCELL_NS_END
