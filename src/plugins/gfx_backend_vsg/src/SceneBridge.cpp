#include <vine/vsg/SceneBridge.hpp>

#include <vine/graphics/Geometry.hpp>
#include <vine/graphics/Material.hpp>
#include <vine/graphics/Node.hpp>
#include <vine/graphics/RenderCommand.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/graphics/ShaderProgram.hpp>
#include <vine/vsg/RenderStateMapper.hpp>
#include <vine/vsg/VsgMaterialManager.hpp>
#include <vsg/commands/BindIndexBuffer.h>
#include <vsg/commands/BindVertexBuffers.h>
#include <vsg/commands/Commands.h>
#include <vsg/commands/DrawIndexed.h>
#include <vsg/core/Array.h>
#include <vsg/io/Options.h>
#include <vsg/maths/mat4.h>
#include <vsg/maths/vec4.h>
#include <vsg/nodes/Geometry.h>
#include <vsg/nodes/Group.h>
#include <vsg/nodes/MatrixTransform.h>
#include <vsg/nodes/StateGroup.h>
#include <vsg/state/ColorBlendState.h>
#include <vsg/state/material.h>
#include <vsg/state/ShaderStage.h>
#include <vsg/state/ArrayState.h>
#include <vsg/utils/GraphicsPipelineConfigurator.h>
#include <vsg/utils/ShaderCompiler.h>
#include <vsg/utils/ShaderSet.h>

#include "VsgUtils.hpp"

#include <algorithm>
#include <cstdint>
#include <unordered_set>

V_VSG_NS_BEGIN

namespace
{

/**
 * @brief Builds a white per-vertex color array.
 *
 * The Phong fragment shader multiplies the vertex color by the material
 * diffuse color. Since Vine's material is carried by the material descriptor
 * (uniform), a white per-vertex color keeps the final color driven solely by
 * the material without double modulation.
 *
 * @param count Number of vertices.
 * @return White color array.
 */
::vsg::ref_ptr<::vsg::vec4Array> makeWhiteColors(std::size_t count)
{
    auto colors = ::vsg::vec4Array::create(static_cast<uint32_t>(count));
    for (auto& v : *colors) {
        v = ::vsg::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    return colors;
}

/**
 * @brief Collects the vertex Data bound by a data node's BindVertexBuffers.
 *
 * Used to register the same arrays with a GraphicsPipelineConfigurator when a
 * fresh state wrapper is built over an existing (retained) data node.
 *
 * @param node Data node (a vsg::Commands) to inspect.
 * @return The bound Data in binding order, or an empty list.
 */
::vsg::DataList boundArraysOf(const ::vsg::ref_ptr<::vsg::Node>& node)
{
    if (node == nullptr) {
        return {};
    }
    if (auto bvb = node->cast<::vsg::BindVertexBuffers>()) {
        ::vsg::DataList out;
        out.reserve(bvb->arrays.size());
        for (const auto& buffer_info : bvb->arrays) {
            if (buffer_info != nullptr && buffer_info->data != nullptr) {
                out.emplace_back(buffer_info->data);
            }
        }
        return out;
    }
    if (auto commands = node->cast<::vsg::Commands>()) {
        for (const auto& child : commands->children) {
            if (auto r = boundArraysOf(child); !r.empty()) {
                return r;
            }
        }
    }
    if (auto group = node->cast<::vsg::Group>()) {
        for (const auto& child : group->children) {
            if (auto r = boundArraysOf(child); !r.empty()) {
                return r;
            }
        }
    }
    return {};
}

/**
 * @brief Builds a per-vertex normal array for a non-indexed mesh.
 *
 * When the mesh provides normals they are copied; otherwise face normals are
 * computed per triangle.
 *
 * @param positions Mesh positions (three vertices per triangle).
 * @param meshNormals Optional mesh normals (may be empty).
 * @return Normal array.
 */
::vsg::ref_ptr<::vsg::vec3Array> makeNormals(
    const vine::geometry::Vec3fArray& positions,
    const vine::geometry::Vec3fArray& meshNormals)
{
    auto normals = ::vsg::vec3Array::create(static_cast<uint32_t>(positions.size()));
    if (meshNormals.size() == positions.size()) {
        for (std::size_t i = 0; i < positions.size(); ++i) {
            const auto& n = meshNormals[i];
            (*normals)[i] = ::vsg::vec3(n.x, n.y, n.z);
        }
        return normals;
    }
    for (std::size_t i = 0; i + 2 < positions.size(); i += 3) {
        const vine::math::Vec3f a = positions[i];
        const vine::math::Vec3f b = positions[i + 1];
        const vine::math::Vec3f c = positions[i + 2];
        const vine::math::Vec3f n = (b - a).cross(c - a).normalized();
        for (std::size_t k = 0; k < 3; ++k) {
            (*normals)[i + k] = ::vsg::vec3(n.x, n.y, n.z);
        }
    }
    return normals;
}

/**
 * @brief Builds a per-vertex normal array for an indexed mesh.
 *
 * When the mesh provides normals they are copied; otherwise face normals are
 * computed per triangle and assigned to the referenced vertices.
 *
 * @param positions Shared vertex positions.
 * @param meshNormals Optional mesh normals (may be empty).
 * @param indices  Triangle indices (three per triangle).
 * @return Normal array.
 */
::vsg::ref_ptr<::vsg::vec3Array> makeIndexedNormals(
    const vine::geometry::Vec3fArray& positions,
    const vine::geometry::Vec3fArray& meshNormals,
    const vine::geometry::UInt32Array& indices)
{
    auto normals = ::vsg::vec3Array::create(static_cast<uint32_t>(positions.size()));
    if (meshNormals.size() == positions.size()) {
        for (std::size_t i = 0; i < positions.size(); ++i) {
            const auto& n = meshNormals[i];
            (*normals)[i] = ::vsg::vec3(n.x, n.y, n.z);
        }
        return normals;
    }
    // Accumulate face normals per vertex for a smoother result.
    for (std::size_t tri = 0; tri + 2 < indices.size(); tri += 3) {
        const vine::math::Vec3f a = positions[indices[tri]];
        const vine::math::Vec3f b = positions[indices[tri + 1]];
        const vine::math::Vec3f c = positions[indices[tri + 2]];
        const vine::math::Vec3f n = (b - a).cross(c - a);
        (*normals)[indices[tri]] += ::vsg::vec3(n.x, n.y, n.z);
        (*normals)[indices[tri + 1]] += ::vsg::vec3(n.x, n.y, n.z);
        (*normals)[indices[tri + 2]] += ::vsg::vec3(n.x, n.y, n.z);
    }
    for (auto& n : *normals) {
        n = ::vsg::normalize(n);
    }
    return normals;
}

/**
 * @brief Maps an SDK shader-stage kind onto the matching Vulkan stage flag.
 *
 * @param type SDK stage kind.
 * @return Vulkan shader-stage flag.
 */
VkShaderStageFlagBits stageFlag(vine::graphics::ShaderStageType type)
{
    switch (type) {
        case vine::graphics::ShaderStageType::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case vine::graphics::ShaderStageType::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        case vine::graphics::ShaderStageType::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
    }
    return VK_SHADER_STAGE_VERTEX_BIT;
}

/**
 * @brief Gets a process-wide vsg shader compiler (glslang).
 *
 * @return Compiler, or null when this vsg build has no glslang.
 */
::vsg::ref_ptr<::vsg::ShaderCompiler> shaderCompiler()
{
    static ::vsg::ref_ptr<::vsg::ShaderCompiler> compiler;
    if (!compiler) {
        compiler = ::vsg::ShaderCompiler::create();
    }
    return compiler;
}

/**
 * @brief Assembles the custom vsg::ShaderSet for a user program.
 *
 * Compiles the program's GLSL stages to SPIR-V at run time and wraps them in
 * a hand-built ShaderSet following the official vsg contract (see
 * vsgExamples/utils/vsgcustomshaderset): one vsg_Vertex attribute binding at
 * location 0 and the canonical "pc" push-constant range that vsg fills per
 * drawable with { mat4 projection; mat4 modelView; }. The default pipeline
 * states are borrowed from the built-in shader set so the pipeline keeps the
 * baked viewport / multisampling; the per-geometry render state is applied
 * afterwards by the caller.
 *
 * @param program    User program (non-null).
 * @param base_states Default pipeline states to inherit (viewport etc.).
 * @return Shader set, or null when compilation/assembly failed.
 */
::vsg::ref_ptr<::vsg::ShaderSet> buildProgramShaderSet(
    vine::raw_ptr<const vine::graphics::ShaderProgram> program,
    const ::vsg::GraphicsPipelineStates& base_states)
{
    if (program == nullptr) {
        return ::vsg::ref_ptr<::vsg::ShaderSet>();
    }
    auto compiler = shaderCompiler();
    if (!compiler || !compiler->supported()) {
        return ::vsg::ref_ptr<::vsg::ShaderSet>();
    }

    ::vsg::ShaderStages stages;
    for (const auto& stage_spec : program->stages()) {
        auto stage = ::vsg::ShaderStage::create(
            stageFlag(stage_spec.type), stage_spec.entryPoint.stdstr(),
            stage_spec.source.stdstr());
        if (!compiler->compile(stage) || !stage->module || stage->module->code.empty()) {
            return ::vsg::ref_ptr<::vsg::ShaderSet>();
        }
        stages.push_back(stage);
    }
    if (stages.empty()) {
        return ::vsg::ref_ptr<::vsg::ShaderSet>();
    }

    auto shader_set = ::vsg::ShaderSet::create(stages);
    shader_set->addAttributeBinding("vsg_Vertex", "", 0, VK_FORMAT_R32G32B32_SFLOAT,
                                    ::vsg::vec3Array::create(1));
    // Normal / colour follow the canonical locations (1 / 2) so a user
    // program can shade with per-vertex normals / colours; the SceneBridge
    // program path feeds these arrays and the material descriptor below.
    shader_set->addAttributeBinding("vsg_Normal", "", 1, VK_FORMAT_R32G32B32_SFLOAT,
                                    ::vsg::vec3Array::create(1));
    shader_set->addAttributeBinding("vsg_Color", "", 2, VK_FORMAT_R32G32B32A32_SFLOAT,
                                    ::vsg::vec4Array::create(1));
    // Material: the same vsg::PhongMaterialValue uniform the default path
    // binds (SceneBridge assigns the cached value), so a program can read the
    // Vine material's diffuse/specular etc. Unused when the program does not
    // read it; harmless in that case.
    shader_set->addDescriptorBinding("material", "", 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
                                     VK_SHADER_STAGE_FRAGMENT_BIT, ::vsg::PhongMaterialValue::create());
    shader_set->addPushConstantRange("pc", "", VK_SHADER_STAGE_VERTEX_BIT, 0, 128);
    shader_set->defaultGraphicsPipelineStates = base_states;
    return shader_set;
}

/**
 * @brief Hashes the identity of one (program, material, render-state) pipeline
 * variant into a cache key for the L2 variant template cache.
 *
 * Pointer identities mix in the raw (program, material) pointers — their
 * lifetime is guaranteed by the scene while the bridge uses them — plus the
 * program's content revision, so editing a retained program's GLSL yields a
 * fresh key and pipeline (D10), and every folded render-state field the
 * pipeline must honour. Collisions with a different variant are safe: they
 * only displace a template entry, which rebuilds on its next use.
 *
 * @param program  User shader program (null = built-in default).
 * @param material Bound material (may be null).
 * @param state    Resolved render state the pipeline honours.
 * @return The content hash used as the variant cache key.
 */
std::uint64_t hashStateVariant(const vine::graphics::ShaderProgram* program,
                               const vine::graphics::Material*     material,
                               const vine::graphics::ResolvedRenderState& state)
{
    const auto combine = [](std::uint64_t h, std::uint64_t v) {
        return h ^ (v + 0x9e3779b97f4a7c15ull + (h << 6u) + (h >> 2u));
    };
    std::uint64_t h = 0xcbf29ce484222325ull;
    const auto   mix_ptr = [&](const void* p) {
        h = combine(h, static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p)));
    };
    mix_ptr(program);
    if (program != nullptr) {
        // Program content is part of the variant identity: editing a retained
        // program's GLSL bumps its revision, which yields a new template key
        // and a fresh pipeline (D10).
        h = combine(h, program->revision());
    }
    mix_ptr(material);
    h = combine(h, static_cast<std::uint64_t>(state.depth.test));
    h = combine(h, static_cast<std::uint64_t>(state.depth.write));
    h = combine(h, static_cast<std::uint64_t>(state.depth.compare));
    h = combine(h, static_cast<std::uint64_t>(state.cullMode));
    h = combine(h, static_cast<std::uint64_t>(state.blend.enabled));
    h = combine(h, static_cast<std::uint64_t>(state.blend.src));
    h = combine(h, static_cast<std::uint64_t>(state.blend.dst));
    h = combine(h, static_cast<std::uint64_t>(state.polygonMode));
    h = combine(h, static_cast<std::uint64_t>(state.topology));
    return h;
}

}  // namespace

SceneBridge::SceneBridge()
{
    // Share layout / pipeline / descriptor-set content across the bridge's
    // geometry so identical (shader, render state, material) resolve to ONE
    // VkPipeline: the pipeline count follows state variants, not the geometry
    // count, which is what keeps hundreds/thousands of drawables cheap to load
    // and run (see GraphicsPipelineConfigurator::copyTo's SharedObjects path).
    shared_objects_ = ::vsg::SharedObjects::create();
}

SceneBridge::~SceneBridge() = default;

void SceneBridge::setShaderSet(::vsg::ref_ptr<::vsg::ShaderSet> shaderSet)
{
    shader_set_ = shaderSet;
}

void SceneBridge::setMaterialManager(vine::raw_ptr<VsgMaterialManager> manager)
{
    material_manager_ = manager;
}

VsgMaterialManager& SceneBridge::materialManager()
{
    return material_manager_ != nullptr ? *material_manager_ : default_manager_;
}

::vsg::ref_ptr<::vsg::ShaderSet> SceneBridge::baseShaderSet()
{
    if (shader_set_ == nullptr) {
        shader_set_ = ::vsg::createPhongShaderSet();
    }
    return shader_set_;
}

::vsg::ref_ptr<::vsg::ShaderSet> SceneBridge::getProgramShaderSet(
    vine::raw_ptr<const vine::graphics::ShaderProgram> program)
{
    if (program == nullptr) {
        return ::vsg::ref_ptr<::vsg::ShaderSet>();
    }
    const auto program_rev = program->revision();
    auto it = program_shader_sets_.find(program);
    if (it != program_shader_sets_.end() && it->second.revision == program_rev) {
        return it->second.shader_set;
    }
    // Compile once per (program content, slot). The base default pipeline
    // states (the baked viewport / blend) come from the slot's shader set;
    // both shader sets bind the same "material" Phong uniform, so the compiled
    // set stays compatible with the built-in path's descriptor assignment.
    const auto base_states = baseShaderSet()->defaultGraphicsPipelineStates;
    auto shaderSet = buildProgramShaderSet(program, base_states);
    // A failed compile is cached too (null) so later geometry does not retry
    // the expensive glslang pass every frame (it falls back to the built-in);
    // editing the program bumps its revision and forces a recompile (D10).
    auto& entry       = program_shader_sets_[program];
    entry.revision    = program_rev;
    entry.shader_set  = shaderSet;
    // D16: bound slot-lifetime growth. Dropping the whole cache only loses the
    // fast path (a program recompiles once on its next use); it never breaks
    // correctness — retained geometry keeps its already-built pipelines.
    constexpr std::size_t kMaxProgramCacheEntries = 64;
    if (program_shader_sets_.size() > kMaxProgramCacheEntries) {
        program_shader_sets_.clear();
    }
    return shaderSet;
}

/** @brief Retained vsg node for one drawn geometry. */
struct SceneBridge::Item {
    // Last translated identity, used to detect geometry/material/state changes.
    vine::graphics::Material* material = nullptr;
    std::uint64_t revision = ~std::uint64_t{0};
    // Last resolved render state the retained pipeline was built with.
    vine::graphics::ResolvedRenderState render_state;
    // Last user program the retained pipeline was built with (null = built-in).
    vine::graphics::ShaderProgram* program = nullptr;
    // Content revision of @ref program the retained pipeline was built with
    // (D10: editing a retained program's GLSL must invalidate it).
    std::uint64_t program_revision = ~std::uint64_t{0};
    // Root of the retained subtree: matrix transform -> state_node -> data_node.
    ::vsg::ref_ptr<::vsg::MatrixTransform> transform;
    // Pipeline/descriptor wrapper (state group) for the current
    // (material, state, program) variant; its child is @ref data_node.
    ::vsg::ref_ptr<::vsg::StateGroup> state_node;
    // Geometry vertex/index draw commands. Kept stable across state-only
    // rebuilds so a material/state/program edit never re-materialises or
    // re-uploads the mesh data.
    ::vsg::ref_ptr<::vsg::Commands> data_node;
    // Per-vertex color array; its alpha carries the effective per-drawable
    // opacity and is rewritten only when the opacity actually changed.
    ::vsg::ref_ptr<::vsg::vec4Array> colors;
    // Cached write state so steady-state frames skip redundant work.
    ::vsg::dmat4 last_matrix;
    bool matrix_valid = false;
    float last_opacity = -1.0f;  // sentinel forces the first write
    // Consecutive frames this geometry was absent (hidden/culled/removed).
    std::uint32_t absent_frames = 0;
};

/**
 * @brief Reusable bind commands for one (program, material, render-state)
 * pipeline variant.
 *
 * Captured from the first geometry that built the variant (see buildGeometry):
 * the canonical shared BindGraphicsPipeline + BindDescriptorSet command list
 * copyTo produced (already deduplicated through the bridge's SharedObjects),
 * the vertex-binding start index and the prototype array state. Later geometry
 * of the same variant reuse these instead of running another configurator, and
 * only attach their own vertex/index data.
 */
struct SceneBridge::VariantEntry {
    const vine::graphics::ShaderProgram* program = nullptr;
    vine::graphics::Material*            material = nullptr;
    vine::graphics::ResolvedRenderState  state;
    ::vsg::StateCommands state_commands;
    ::vsg::ref_ptr<::vsg::ArrayState> prototype_array_state;
    std::uint32_t base_binding = 0;
};

void SceneBridge::clearCache()
{
    cache_.clear();
    program_shader_sets_.clear();
    variant_cache_.clear();
    // Forget the shared-object registry (releases the registered pipeline /
    // layout / descriptor-set objects) when the slot's content is released.
    // Retained geometry nodes still hold ref_ptr to the shared objects until
    // they are destroyed, so clearing only drops the registry, never leaves a
    // dangling reference.
    if (shared_objects_ != nullptr) {
        shared_objects_->clear();
    }
    pipeline_variants_ = 0;
    variant_reuses_ = 0;
}

bool SceneBridge::syncRenderCommands(
    const std::vector<vine::graphics::RenderCommand>& commands,
    ::vsg::Group* root,
    std::vector<::vsg::ref_ptr<::vsg::Node>>* created)
{
    if (root == nullptr) {
        return false;
    }
    bool changed = false;
    std::vector<::vsg::ref_ptr<::vsg::Node>> visible;
    visible.reserve(commands.size());
    std::unordered_set<const vine::graphics::Geometry*> seen;
    seen.reserve(commands.size());

    for (const auto& cmd : commands) {
        const auto* geometry = cmd.geometry.get();
        if (geometry == nullptr) {
            continue;
        }
        seen.insert(geometry);

        Item* item = nullptr;
        auto it = cache_.find(geometry);
        if (it == cache_.end()) {
            auto entry = std::make_unique<Item>();
            item = entry.get();
            cache_.emplace(geometry, std::move(entry));
            changed = true;
        } else {
            item = it->second.get();
        }

        // Rebuild the retained subtree when any of its inputs changed. The
        // mesh DATA and the STATE (pipeline + descriptor) are decoupled: a
        // revision (vertex/index data) change rebuilds only the data node; a
        // material / resolved-state / program change rebuilds only the state
        // wrapper and reuses the retained data node, so material/state edits
        // never re-materialise or re-upload the mesh.
        const bool had_node = item->transform != nullptr;
        const auto program_rev =
            cmd.program.get() != nullptr ? cmd.program.get()->revision() : std::uint64_t{0};
        const bool data_dirty  = !had_node || item->revision != geometry->revision();
        const bool state_dirty = !had_node || item->material != cmd.material.get() ||
                                 item->render_state != cmd.renderState ||
                                 item->program != cmd.program.get() ||
                                 item->program_revision != program_rev;
        if (data_dirty || state_dirty) {
            item->revision         = geometry->revision();
            item->material         = cmd.material.get();
            item->render_state     = cmd.renderState;
            item->program          = cmd.program.get();
            item->program_revision = program_rev;
            changed                = true;
        }

        if (data_dirty) {
            // Fresh vertex data: rebuild the data node; the previous opacity
            // carrier is dropped with it and rewritten on the next frames.
            item->data_node = buildGeometryData(geometry, item->program == nullptr,
                                                item->colors);
            if (item->data_node == nullptr) {
                // Unsupported shape / empty mesh: nothing drawable.
                cache_.erase(geometry);
                continue;
            }
            item->matrix_valid = false;
            item->last_opacity = -1.0f;
        }

        if (state_dirty || item->state_node == nullptr) {
            item->state_node = buildStateGroup(item->data_node, item->material,
                                               item->render_state, item->program);
            if (item->state_node == nullptr) {
                cache_.erase(geometry);
                continue;
            }
        }

        // Attach: the wrapper's child is the current data node and the
        // retained transform's child is the current wrapper.
        if (item->state_node->children.empty() ||
            item->state_node->children.front().get() != item->data_node.get()) {
            item->state_node->children.clear();
            item->state_node->addChild(item->data_node);
        }
        if (!had_node) {
            item->transform = ::vsg::MatrixTransform::create();
            item->transform->addChild(item->state_node);
        }
        else if (item->transform->children.empty() ||
                 item->transform->children.front().get() != item->state_node.get()) {
            item->transform->children.clear();
            item->transform->addChild(item->state_node);
        }

        if ((data_dirty || state_dirty) && created != nullptr) {
            // New/rebuild subtrees must be GPU-compiled before recording.
            created->emplace_back(item->transform);
        }

        // Effective opacity (scene x nodes x leaf geometry) rides the
        // per-vertex alpha. Rewriting O(vertices) only when it actually
        // changed keeps the steady-state per-frame cost independent of mesh
        // size, while opacity edits still apply live.
        if (item->colors != nullptr && item->last_opacity != cmd.opacity) {
            const float opacity = cmd.opacity;
            for (auto& color : *item->colors) {
                color.a = opacity;
            }
            // The colour array is DYNAMIC (see buildGeometry): mark it dirty so
            // vsg's per-frame TransferTask re-copies it this frame; unchanged
            // frames issue no transfer.
            item->colors->dirty();
            item->last_opacity = opacity;
        }

        // World-space placement comes from the command stream; the matrix
        // write is skipped when the node did not move this frame.
        const ::vsg::dmat4 world = detail::toVsg(cmd.modelMatrix);
        if (!item->matrix_valid || item->last_matrix != world) {
            item->transform->matrix = world;
            item->last_matrix = world;
            item->matrix_valid = true;
        }
        visible.emplace_back(item->transform);
    }

    // A geometry missing from the frame is not dropped immediately: hiding a
    // node/drawable or a frustum-culled object must stay cheap (its compiled
    // node is simply detached from the root and reused when it reappears, with
    // no rebuild or recompile). Only a long-running absence — a drawable truly
    // removed from the scene — evicts the retained node.
    constexpr std::uint32_t kAbsentEvictFrames = 600;
    for (auto it = cache_.begin(); it != cache_.end();) {
        if (seen.count(it->first) != 0) {
            it->second->absent_frames = 0;
            ++it;
        } else if (++it->second->absent_frames > kAbsentEvictFrames) {
            changed = true;
            it = cache_.erase(it);
        } else {
            ++it;
        }
    }

    // Reparent the retained children to match the (already sorted) command
    // stream; a no-op when the order did not change.
    const bool same = [&] {
        if (root->children.size() != visible.size()) {
            return false;
        }
        for (std::size_t i = 0; i < visible.size(); ++i) {
            if (root->children[i] != visible[i]) {
                return false;
            }
        }
        return true;
    }();
    if (!same) {
        root->children.clear();
        for (auto& node : visible) {
            root->children.emplace_back(node);
        }
    }

    // Refresh bound material values in place so property edits show up live
    // (the descriptor already points at these cached Phong values). Each
    // distinct material is visited once per frame and its UBO is rewritten
    // only when the Vine material's parameters actually changed, so a steady
    // state costs O(distinct materials) compares instead of O(commands)
    // unconditional writes (D19).
    if (!commands.empty()) {
        auto& manager = materialManager();
        const auto same4 = [](const ::vsg::vec4& a, const ::vsg::vec4& b) {
            return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
        };
        std::unordered_set<const vine::graphics::Material*> refreshed;
        refreshed.reserve(commands.size());
        for (const auto& cmd : commands) {
            if (cmd.material == nullptr || !refreshed.insert(cmd.material.get()).second) {
                continue;
            }
            auto  value = manager.getOrCreate(cmd.material.get());
            auto& m     = value->value();
            const auto diffuse  = cmd.material->diffuse();
            const auto specular = cmd.material->specular();
            const auto ambient  = cmd.material->ambient();
            // Opacity is carried by the per-vertex alpha, not the shared
            // material, so per-geometry opacity stays independent (the diffuse
            // alpha is pinned to 1 here).
            const ::vsg::vec4 want_ambient(ambient.r, ambient.g, ambient.b, ambient.a);
            const ::vsg::vec4 want_diffuse(diffuse.r, diffuse.g, diffuse.b, 1.0f);
            const ::vsg::vec4 want_specular(specular.r, specular.g, specular.b, specular.a);
            const float       want_shininess = cmd.material->shininess();
            if (same4(m.ambient, want_ambient) && same4(m.diffuse, want_diffuse) &&
                same4(m.specular, want_specular) && m.shininess == want_shininess) {
                continue;
            }
            m.ambient   = want_ambient;
            m.diffuse   = want_diffuse;
            m.specular  = want_specular;
            m.shininess = want_shininess;
            // The material uniform is DYNAMIC (see VsgMaterialManager): mark it
            // dirty so the per-frame TransferTask re-copies it; materials that
            // did not change are never dirtied, so steady state transfers
            // nothing.
            value->dirty();
        }
    }

    return changed;
}

::vsg::ref_ptr<::vsg::Commands> SceneBridge::buildGeometryData(
    vine::raw_ptr<const vine::graphics::Geometry> geometry,
    bool opacity_carrier,
    ::vsg::ref_ptr<::vsg::vec4Array>& out_colors)
{
    if (geometry == nullptr) {
        return ::vsg::ref_ptr<::vsg::Commands>();
    }
    // Materialise the open attribute list into typed CPU arrays for the vsg
    // build: location 0 = positions, location 1 = (optional) normals.
    const auto* position_attr = geometry->buffer(0);
    if (position_attr == nullptr || position_attr->empty() ||
        position_attr->components < 3u) {
        return ::vsg::ref_ptr<::vsg::Commands>();
    }
    vine::geometry::Vec3fArray positions;
    {
        const auto& data = *position_attr->data;
        positions.reserve(data.size() / 3u);
        for (std::size_t i = 0; i + 2 < data.size(); i += 3) {
            positions.emplace_back(data[i], data[i + 1], data[i + 2]);
        }
    }

    ::vsg::ref_ptr<::vsg::vec3Array> vertices =
        ::vsg::vec3Array::create(static_cast<uint32_t>(positions.size()));
    for (std::size_t i = 0; i < positions.size(); ++i) {
        const auto& v = positions[i];
        (*vertices)[i] = ::vsg::vec3(v.x, v.y, v.z);
    }

    vine::geometry::Vec3fArray src_normals;
    if (const auto* normal_attr = geometry->buffer(1);
        normal_attr != nullptr && !normal_attr->empty() &&
        normal_attr->components >= 3u) {
        const auto& data = *normal_attr->data;
        src_normals.reserve(data.size() / 3u);
        for (std::size_t i = 0; i + 2 < data.size(); i += 3) {
            src_normals.emplace_back(data[i], data[i + 1], data[i + 2]);
        }
    }

    ::vsg::ref_ptr<::vsg::vec3Array> normals;
    ::vsg::ref_ptr<::vsg::uintArray> indices;
    if (geometry->hasIndices()) {
        const auto& src_indices = *geometry->indices();
        indices = ::vsg::uintArray::create(static_cast<uint32_t>(src_indices.size()));
        for (std::size_t i = 0; i < src_indices.size(); ++i) {
            (*indices)[i] = src_indices[i];
        }
        normals = makeIndexedNormals(positions, src_normals, src_indices);
    } else {
        indices = ::vsg::uintArray::create(static_cast<uint32_t>(positions.size()));
        for (uint32_t i = 0; i < positions.size(); ++i) {
            (*indices)[i] = i;
        }
        normals = makeNormals(positions, src_normals);
    }

    // White per-vertex colour array bound on every geometry; its alpha is
    // rewritten in place to drive per-drawable opacity. When this node is the
    // built-in opacity carrier the array is marked DYNAMIC so vsg's per-frame
    // TransferTask re-copies it after a dirty() (unchanged frames transfer
    // nothing); a user-program node keeps it static (D8: the program owns
    // opacity), only binding it for programs that read vsg_Color.
    auto colors = makeWhiteColors(vertices->size());
    if (opacity_carrier) {
        colors->properties.dataVariance = ::vsg::DYNAMIC_DATA;
        out_colors = colors;
    }
    else {
        out_colors = ::vsg::ref_ptr<::vsg::vec4Array>();
    }
    // The bound vertex data follows the shader set's attribute-binding order
    // (vertex, normal, colour) starting at binding 0.
    ::vsg::DataList arrays;
    arrays.emplace_back(vertices);
    arrays.emplace_back(normals);
    arrays.emplace_back(colors);

    // NOTE: manual geometry must use explicit bind/draw commands, NOT a
    // manually-assembled VertexIndexDraw, or nothing is rasterized (same
    // finding as VsgRenderer::makeRawDemoNode).
    auto drawCommands = ::vsg::Commands::create();
    drawCommands->addChild(::vsg::BindVertexBuffers::create(0u, arrays));
    drawCommands->addChild(::vsg::BindIndexBuffer::create(indices));
    drawCommands->addChild(::vsg::DrawIndexed::create(
        static_cast<uint32_t>(indices->size()), 1, 0, 0, 0));
    return drawCommands;
}

::vsg::ref_ptr<::vsg::StateGroup> SceneBridge::buildStateGroup(
    ::vsg::ref_ptr<::vsg::Node> data,
    vine::raw_ptr<vine::graphics::Material> material,
    const vine::graphics::ResolvedRenderState& state,
    vine::raw_ptr<const vine::graphics::ShaderProgram> program)
{
    if (data == nullptr) {
        return ::vsg::ref_ptr<::vsg::StateGroup>();
    }

    // A user program replaces the built-in pipeline: compile its stages and
    // assemble a custom ShaderSet following the official vsg contract
    // (vsg_Vertex + "pc" projection/modelView push constant). The compiled set
    // is cached per (program, slot) — L1 — so N geometry bound to one program
    // share a single glslang compile. On any failure fall back to the built-in
    // default so a bad program cannot break a scene.
    ::vsg::ref_ptr<::vsg::ShaderSet> shaderSet;
    if (program != nullptr) {
        shaderSet = getProgramShaderSet(program);
    }
    if (!shaderSet) {
        shaderSet = baseShaderSet();
    }

    // L2 variant reuse: an identical (program, material, resolved-state)
    // variant built earlier contributes its reusable bind commands (the shared
    // pipeline bind + the per-material descriptor bind). Reuse skips the
    // configurator entirely.
    const auto hash_key = hashStateVariant(program, material, state);
    const auto variant_it = variant_cache_.find(hash_key);
    if (variant_it != variant_cache_.end() && variant_it->second != nullptr &&
        variant_it->second->program == program &&
        variant_it->second->material == material &&
        variant_it->second->state == state) {
        ++variant_reuses_;
        auto stateGroup = ::vsg::StateGroup::create();
        for (const auto& sc : variant_it->second->state_commands) {
            stateGroup->stateCommands.push_back(sc);
        }
        stateGroup->prototypeArrayState = variant_it->second->prototype_array_state;
        return stateGroup;
    }

    auto config = ::vsg::GraphicsPipelineConfigurator::create(shaderSet);

    // Material resources come from the material manager (converted + cached),
    // never built ad-hoc here. The same attributes and the shared "material"
    // uniform are registered on both paths; the actual vertex data is already
    // bound by the retained data node, so only the bindings are re-declared.
    {
        auto& material_manager = materialManager();
        auto  material_value   = material_manager.getOrCreate(material);
        const auto arrays      = boundArraysOf(data);
        ::vsg::DataList scratch;
        if (arrays.size() > 0u) {
            config->assignArray(scratch, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, arrays[0]);
        }
        if (arrays.size() > 1u) {
            config->assignArray(scratch, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, arrays[1]);
        }
        if (arrays.size() > 2u) {
            config->assignArray(scratch, "vsg_Color", VK_VERTEX_INPUT_RATE_VERTEX, arrays[2]);
        }
        config->assignDescriptor("material", material_value);
    }

    // Assemble the pipeline from the geometry's effective render state. The
    // mapped color blend keeps alpha blending enabled on every pipeline (the
    // per-vertex opacity alpha may drop below 1 at any time without a rebuild);
    // depth, culling, polygon mode, blend factors and topology come from the
    // StateNode fold carried by the command.
    RenderStateObjects states = makeRenderStateObjects(state);

    // MRT: the mapped blend is a single attachment (the renderer's default),
    // but a pipeline recorded into a multi-colour-attachment subpass must
    // carry one blend entry PER attachment, or Vulkan only writes attachment 0
    // (the rest stay cleared -> black). Size it to the slot shader set's
    // colour count (built from the target's colour attachments): attachment 0
    // keeps the mapped opacity blend, the extra G-buffer attachments write
    // opaque, unblended.
    int mrt = 1;
    if (shader_set_ != nullptr) {
        for (const auto& s : shader_set_->defaultGraphicsPipelineStates) {
            if (auto cb = s.cast<::vsg::ColorBlendState>()) {
                mrt = std::max(1, static_cast<int>(cb->attachments.size()));
                break;
            }
        }
    }
    if (mrt > 1) {
        const auto first = states.colorBlend->attachments.front();
        states.colorBlend->attachments.clear();
        for (int i = 0; i < mrt; ++i) {
            auto attachment = first;
            if (i > 0) {
                attachment.blendEnable = VK_FALSE;
            }
            states.colorBlend->attachments.push_back(attachment);
        }
    }
    applyRenderStateObjects(*config, states);

    config->init();

    // Register this pipeline with the shared-object cache: when an identical
    // variant is already registered, SharedObjects returns the existing object
    // (content-equal dedup) and the fresh duplicate is dropped. Count only
    // genuinely new variants so pipelineVariantCount() reflects distinct
    // pipeline states, not the geometry count.
    const auto local_bind = config->bindGraphicsPipeline;
    auto stateGroup       = ::vsg::StateGroup::create();
    config->copyTo(stateGroup, shared_objects_);
    if (shared_objects_ != nullptr && config->bindGraphicsPipeline == local_bind) {
        ++pipeline_variants_;
    }

    // Cache this variant's reusable pieces for later identical geometry. A
    // hash collision with a different variant simply overwrites the entry —
    // the displaced variant rebuilds fresh on its next appearance (still
    // correct, just uncached).
    {
        auto entry = std::make_unique<VariantEntry>();
        entry->program               = program;
        entry->material              = material;
        entry->state                 = state;
        entry->state_commands        = stateGroup->stateCommands;
        entry->prototype_array_state = stateGroup->prototypeArrayState;
        entry->base_binding          = config->baseAttributeBinding;
        variant_cache_[hash_key]     = std::move(entry);
        // D16: bound slot-lifetime growth. Dropping the template cache only
        // loses the fast path (variants rebuild on next use); retained state
        // nodes keep their built pipelines, so correctness is unaffected.
        constexpr std::size_t kMaxVariantCacheEntries = 256;
        if (variant_cache_.size() > kMaxVariantCacheEntries) {
            variant_cache_.clear();
        }
    }

    return stateGroup;
}

V_VSG_NS_END
