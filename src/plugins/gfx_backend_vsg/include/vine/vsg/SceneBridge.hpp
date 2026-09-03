#pragma once
#include "vsg_global.hpp"

#include <vsg/nodes/Group.h>
#include <vsg/nodes/Node.h>
#include <vsg/core/ref_ptr.h>
#include <vsg/utils/ShaderSet.h>
#include <vsg/utils/SharedObjects.h>

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <vine/raw_ptr.hpp>
#include <vine/vsg/VsgMaterialManager.hpp>

namespace vine::graphics
{
class Geometry;
class Scene;
class Node;
class ShaderProgram;
struct RenderCommand;
struct ResolvedRenderState;
}

V_VSG_NS_BEGIN

/**
 * @brief Translates Vine render commands into a retained vsg scene graph.
 *
 * SceneBridge is the bridge between Vine's platform-independent scene graph
 * (Scene/Node/Drawable) and VulkanSceneGraph. Rendering is driven by the
 * per-frame render command list produced by Scene::collectRenderCommands:
 * syncRenderCommands() reconciles a stable vsg::Group against the commands so
 * drawables that move or change color only update their matrix / material in
 * place, and a vsg node (matrix transform -> state group -> vertex draw) is
 * only (re)built when a geometry first appears, disappears, or changes its
 * shape or material binding. This keeps runtime scene edits visible without
 * re-initializing the backend.
 */
class V_VSG_API SceneBridge {
  public:
    SceneBridge();
    ~SceneBridge();

    /** @brief Sets the shader set used to build per-geometry pipelines.
     *
     * @param shaderSet Shader set to use; defaults to flat shaded when unset.
     */
    void setShaderSet(::vsg::ref_ptr<::vsg::ShaderSet> shaderSet);

    /** @brief Sets the material manager used to obtain Phong resources.
     *
     * Must outlive the bridge. When unset, a default VsgMaterialManager is
     * created lazily.
     *
     * @param manager Material manager to use.
     */
    void setMaterialManager(vine::raw_ptr<VsgMaterialManager> manager);

    /** @brief Reconciles the retained scene under root against the commands.
     *
     * Each render command contributes one retained child (a vsg::MatrixTransform
     * holding the geometry state group). Existing children are updated in place;
     * a child is (re)built when its geometry has no cached node or its shape /
     * material binding changed, and children whose geometry is no longer drawn
     * are dropped.
     *
     * @param commands Render commands for the current frame.
     * @param root     Stable vsg root group the retained children live under.
     * @param created  Optional: receives the subtrees newly built this frame
     *                 (they still need GPU compilation before recording).
     * @return true when the graph changed structurally; only newly built
     *         subtrees in @p created require compilation.
     */
    bool syncRenderCommands(
        const std::vector<vine::graphics::RenderCommand>& commands,
        ::vsg::Group* root,
        std::vector<::vsg::ref_ptr<::vsg::Node>>* created = nullptr);

    /** @brief Releases all retained per-geometry vsg nodes. */
    void clearCache();

  private:
    /** @brief Retained per-geometry render node (defined in the .cpp). */
    struct Item;

    /** @brief Builds (or rebuilds) the vsg subtree for one geometry/material.
     *
     * The geometry's pipeline is assembled from @p state (depth, culling,
     * polygon mode, blending factors, topology) via the RenderStateMapper; a
     * geometry whose resolved state changed is rebuilt by the caller. When
     * @p program is set, the pipeline is assembled from the user program
     * instead of the built-in shader set (its GLSL is compiled at run time
     * and bound via the official vsg contract: vsg_Vertex + the "pc" push
     * constant carrying projection/modelView).
     *
     * @param geometry   Geometry to build.
     * @param material   Bound material (may be null).
     * @param out_colors Receives the per-vertex color array the caller keeps
     *                   to drive per-drawable opacity each frame (null when a
     *                   user program renders instead).
     * @param state      Resolved render state the pipeline must honour.
     * @param program    User shader program, or null for the built-in default.
     * @return Built vsg node, or null when not buildable.
     */
    ::vsg::ref_ptr<::vsg::Node> buildGeometry(
        vine::raw_ptr<const vine::graphics::Geometry> geometry,
        vine::raw_ptr<vine::graphics::Material> material,
        ::vsg::ref_ptr<::vsg::vec4Array>& out_colors,
        const vine::graphics::ResolvedRenderState& state,
        vine::raw_ptr<const vine::graphics::ShaderProgram> program);

    ::vsg::ref_ptr<::vsg::ShaderSet> shader_set_;
    ::vsg::ref_ptr<::vsg::SharedObjects> shared_objects_;
    vine::raw_ptr<VsgMaterialManager> material_manager_ = nullptr;
    // Default manager used when the renderer does not inject one.
    VsgMaterialManager default_manager_;
    // Retained per-geometry nodes, keyed by geometry pointer for O(1) lookup.
    std::unordered_map<const vine::graphics::Geometry*, std::unique_ptr<Item>> cache_;
};

V_VSG_NS_END
