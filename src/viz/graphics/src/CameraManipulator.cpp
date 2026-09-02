#include <vine/graphics/CameraManipulator.hpp>

V_GRAPHICS_NS_BEGIN

CameraManipulator::CameraManipulator(Camera* camera)
  : camera_(camera)
{
}

CameraManipulator::~CameraManipulator() = default;

Camera* CameraManipulator::camera() const
{
    return camera_;
}

CameraManipulator::Mode CameraManipulator::mode() const
{
    return mode_;
}

void CameraManipulator::setMode(Mode m)
{
    mode_ = m;
}

V_GRAPHICS_NS_END
