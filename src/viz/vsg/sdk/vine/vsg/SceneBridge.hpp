#pragma once
#include "vsg_global.hpp"

#include <vsg/nodes/Node.h>
#include <vsg/core/ref_ptr.h>

namespace vine::graphics
{
class Scene;
class Node;
class Geometry;
}

V_VSG_NS_BEGIN

/**
 * @brief Translates a vine::graphics scene graph into a vsg scene graph.
 *
 * SceneBridge is the bridge layer between Vine's platform-independent scene
 * graph (Scene/Node/Drawable) and VulkanSceneGraph. It produces a ::vsg::Node
 * tree that can be rendered by ::vsg::Viewer or ::vsg::Renderer.
 */
class V_VSG_API SceneBridge {
  public:
    /** @brief Builds a vsg scene graph from a Vine scene.
     *
     * @param scene Vine scene to translate.
     * @return vsg root node (Group), or null when the scene is empty.
     */
    ::vsg::ref_ptr<::vsg::Node> build(vine::graphics::Scene* scene);

  private:
    /** @brief Translates a Vine node into a vsg transform node. */
    ::vsg::ref_ptr<::vsg::Node> buildNode(vine::graphics::Node* node);

    /** @brief Translates a Vine geometry into a vsg drawable node. */
    ::vsg::ref_ptr<::vsg::Node> buildGeometry(vine::graphics::Geometry* geometry);
};

V_VSG_NS_END
