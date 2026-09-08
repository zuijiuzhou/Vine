#pragma once
#include "vsg_global.hpp"

#include <vsg/commands/Commands.h>
#include <vsg/nodes/Group.h>
#include <vsg/nodes/Node.h>
#include <vsg/nodes/StateGroup.h>
#include <vsg/core/ref_ptr.h>
#include <vsg/utils/ShaderSet.h>
#include <vsg/utils/SharedObjects.h>

#include <cstddef>
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

    /** @brief Gets the number of distinct compiled pipeline variants.
     *
     * Counts how many genuinely distinct vsg::GraphicsPipeline objects this
     * bridge registered with its shared-objects cache. Geometries that
     * resolve to the same (shader, render state, subpass) variant share one
     * pipeline, so loading N geometries whose variant count stays far below N
     * confirms pipeline sharing is collapsing duplicates (pipeline count
     * follows state variants, not geometry count).
     *
     * @return Number of distinct pipeline variants built so far.
     */
    std::size_t pipelineVariantCount() const { return pipeline_variants_; }

    /** @brief Gets how many times geometry reused a cached pipeline variant.
     *
     * Incremented whenever buildGeometry reuses an already-built (program,
     * material, render-state) template instead of running a fresh
     * GraphicsPipelineConfigurator. A load whose reuse count is close to its
     * geometry count (minus the distinct variants) confirms the L2 fast path
     * is collapsing repeated variant setup.
     *
     * @return Number of variant-template reuses so far.
     */
    std::size_t variantReuseCount() const { return variant_reuses_; }

  private:
    /** @brief Retained per-geometry render node (defined in the .cpp). */
    struct Item;

    /** @brief Builds (or rebuilds) the retained vertex-data node of a geometry.
     *
     * Materialises the geometry's attribute buffers into vsg arrays and wraps
     * them in bind/draw commands. The node is geometry-data only: it carries
     * no pipeline, so it is reused verbatim across material / render-state /
     * program changes (only the state wrapper is rebuilt then), and it stays
     * stable so a later geometry-only edit never re-uploads unchanged meshes.
     * When @p opacity_carrier is true (built-in path), the per-vertex colour
     * array is marked DYNAMIC and returned via @p out_colors so per-drawable
     * opacity edits after upload are re-transferred on dirty().
     *
     * @param geometry        Geometry to build.
     * @param opacity_carrier True when the built-in path drives per-drawable
     *                        opacity through the vertex-colour alpha (false
     *                        when a user program owns opacity).
     * @param out_colors      Receives the per-vertex colour array the caller
     *                        keeps to drive opacity each frame (null when
     *                        @p opacity_carrier is false).
     * @return Data commands node, or null when not buildable.
     */
    ::vsg::ref_ptr<::vsg::Commands> buildGeometryData(
        vine::raw_ptr<const vine::graphics::Geometry> geometry,
        bool opacity_carrier,
        ::vsg::ref_ptr<::vsg::vec4Array>& out_colors);

    /** @brief Builds (or rebuilds) the state wrapper around a data node.
     *
     * The wrapper is a vsg::StateGroup carrying the pipeline + descriptor-set
     * binds for one (program, material, resolved-state) variant; @p data is
     * attached as its child by the caller. Pipelines are resolved through the
     * per-(slot, program) L1 ShaderSet cache and the per-variant L2 template
     * cache, so repeated variants skip the configurator entirely.
     *
     * @param data     The retained vertex-data node to wrap (non-null).
     * @param material Bound material (may be null).
     * @param state    Resolved render state the pipeline must honour.
     * @param program  User shader program, or null for the built-in default.
     * @return State wrapper, or null when not buildable.
     */
    ::vsg::ref_ptr<::vsg::StateGroup> buildStateGroup(
        ::vsg::ref_ptr<::vsg::Node> data,
        vine::raw_ptr<vine::graphics::Material> material,
        const vine::graphics::ResolvedRenderState& state,
        vine::raw_ptr<const vine::graphics::ShaderProgram> program);

    /** @brief Gets (and caches) the run-time compiled ShaderSet for a program.
     *
     * Compiles the program's stages once per (program, slot) instead of once
     * per geometry, so N geometry bound to the same program share a single
     * glslang compile and ShaderSet. A compile/assembly failure is cached too
     * (null), so later geometry does not retry the failed compile each time.
     *
     * @param program User program (non-null).
     * @return Compiled shader set, or null when it could not be built.
     */
    ::vsg::ref_ptr<::vsg::ShaderSet> getProgramShaderSet(
        vine::raw_ptr<const vine::graphics::ShaderProgram> program);

    /** @brief Gets the material manager used to obtain Phong resources.
     *
     * Falls back to the bridge-owned default manager when the renderer never
     * injected one (setMaterialManager). Both callers that need material
     * resources route through here so the fallback is decided once.
     *
     * @return The active material manager (always non-null).
     */
    VsgMaterialManager& materialManager();

    /** @brief Gets the slot's base shader set (the built-in default when unset).
     *
     * The built-in Phong set is created lazily on first use and cached, so
     * the bridge never pays for a fresh createPhongShaderSet() per geometry.
     * A user program path builds on top of this set's default pipeline states
     * (the baked viewport / blending), keeping both paths on one material
     * descriptor ABI.
     *
     * @return The base shader set (always non-null).
     */
    ::vsg::ref_ptr<::vsg::ShaderSet> baseShaderSet();

    ::vsg::ref_ptr<::vsg::ShaderSet> shader_set_;
    // Shares layout / pipeline / descriptor-set content across every geometry
    // this bridge builds: GraphicsPipelineConfigurator::copyTo() deduplicates
    // through SharedObjects (content equality), so geometry that resolves to
    // the same pipeline state and material registers ONE vsg::GraphicsPipeline
    // and descriptor set instead of one per geometry.
    ::vsg::ref_ptr<::vsg::SharedObjects> shared_objects_;
    // Distinct pipeline variants registered with shared_objects_ (diagnostic;
    // see pipelineVariantCount()).
    std::size_t pipeline_variants_ = 0;
    // Times buildGeometry reused a cached (program, material, state) template
    // instead of running a fresh configurator (diagnostic; see
    // variantReuseCount()).
    std::size_t variant_reuses_ = 0;
    vine::raw_ptr<VsgMaterialManager> material_manager_ = nullptr;
    // Default manager used when the renderer does not inject one.
    VsgMaterialManager default_manager_;
    // Retained per-geometry nodes, keyed by geometry pointer for O(1) lookup.
    std::unordered_map<const vine::graphics::Geometry*, std::unique_ptr<Item>> cache_;
    // Cached run-time compiled ShaderSet per user program (L1): every geometry
    // bound to the same program shares one glslang compile + ShaderSet instead
    // of recompiling per geometry. Keyed by raw pointer (program lifetime is
    // guaranteed by the scene while in use; released with the bridge); the
    // entry also stores the program content revision it was built from, so
    // editing a retained program's GLSL (ShaderProgram::revision) rebuilds the
    // compiled set instead of serving the stale one (D10).
    struct ProgramEntry
    {
        std::uint64_t revision = ~std::uint64_t{0};
        ::vsg::ref_ptr<::vsg::ShaderSet> shader_set;
    };
    std::unordered_map<const vine::graphics::ShaderProgram*, ProgramEntry>
        program_shader_sets_;
    // Pipeline-template cache (L2), keyed by a content hash of the (program,
    // material, resolved-state) variant; the full key lives in VariantEntry
    // for collision-safe equality. The first geometry of a variant builds its
    // pipeline through the configurator and captures the reusable bind
    // commands; later geometry of that variant reuse them and only attach
    // their own vertex data, keeping pipeline setup cost proportional to the
    // state-variant count rather than the geometry count.
    struct VariantEntry;
    std::unordered_map<std::uint64_t, std::unique_ptr<VariantEntry>> variant_cache_;
};

V_VSG_NS_END
