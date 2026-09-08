#pragma once
#include "vsg_global.hpp"

#include <vine/raw_ptr.hpp>

#include <vsg/app/Camera.h>
#include <vsg/core/ref_ptr.h>

namespace vine::graphics
{
class Camera;
}

V_VSG_NS_BEGIN

/**
 * @brief Bridges a vine::graphics::Camera to a vsg::Camera.
 *
 * Vine's Camera stores complete view/projection matrices (OSG style), so the
 * bridge simply copies them into the vsg ViewMatrix/ProjectionMatrix. Use
 * apply() to keep a vsg::Camera in sync when the Vine camera changes.
 */
class V_VSG_API CameraBridge {
  public:
    /** @brief Creates a vsg::Camera from a Vine camera.
     *
     * @param camera Vine camera to translate.
     * @return Newly created vsg camera.
     */
    ::vsg::ref_ptr<::vsg::Camera> create(vine::raw_ptr<vine::graphics::Camera> camera);

    /** @brief Copies Vine camera matrices into an existing vsg camera.
     *
     * @param camera    Vine camera source.
     * @param vsgCamera Target vsg camera (matrices are updated in place).
     */
    void apply(vine::raw_ptr<vine::graphics::Camera> camera, ::vsg::ref_ptr<::vsg::Camera> vsgCamera);
};

V_VSG_NS_END
