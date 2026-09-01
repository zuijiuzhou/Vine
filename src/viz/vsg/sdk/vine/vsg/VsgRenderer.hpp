#pragma once
#include "vsg_global.hpp"

#include <vsg/app/Viewer.h>
#include <vsg/core/ref_ptr.h>

namespace vine::graphics
{
class Scene;
class Camera;
}

V_VSG_NS_BEGIN

/**
 * @brief High-level renderer that integrates Vine scene/camera with VSG.
 *
 * VsgRenderer bridges a Vine scene graph and camera to a vsg::Viewer running
 * on a vsg::Window. It owns the translation (SceneBridge/CameraBridge), the
 * flat-shader pipeline applied to geometry, and the render loop (frame()).
 */
class V_VSG_API VsgRenderer {
  public:
    /** @brief Constructs a renderer bound to a Vine scene and camera.
     *
     * @param scene  Vine scene to render (must outlive the renderer).
     * @param camera Vine camera used for the view.
     */
    VsgRenderer(vine::graphics::Scene* scene, vine::graphics::Camera* camera);
    ~VsgRenderer();

    /** @brief Initializes the window, viewer, camera and pipeline.
     *
     * @return true when initialization succeeded.
     */
    bool initialize();

    /** @brief Synchronizes Vine scene and camera into the vsg structures. */
    void update();

    /** @brief Renders one frame (advance, handle events, record, present). */
    void frame();

    /** @brief Closes the window and releases the viewer. */
    void shutdown();

    /** @brief Gets the underlying vsg viewer. */
    ::vsg::ref_ptr<::vsg::Viewer> viewer() const;

    /** @brief Gets the translated vsg camera. */
    ::vsg::ref_ptr<::vsg::Camera> vsgCamera() const;

    /** @brief Gets the translated vsg scene graph. */
    ::vsg::ref_ptr<::vsg::Node> vsgScene() const;

  private:
    struct Data;
    Data* const d;
};

V_VSG_NS_END
