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
#include <vsg/utils/GraphicsPipelineConfigurator.h>
#include <vsg/utils/ShaderCompiler.h>
#include <vsg/utils/ShaderSet.h>

#include "VsgUtils.hpp"

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
    shader_set->addPushConstantRange("pc", "", VK_SHADER_STAGE_VERTEX_BIT, 0, 128);
    shader_set->defaultGraphicsPipelineStates = base_states;
    return shader_set;
}

}  // namespace

SceneBridge::SceneBridge() = default;

SceneBridge::~SceneBridge() = default;

void SceneBridge::setShaderSet(::vsg::ref_ptr<::vsg::ShaderSet> shaderSet)
{
    shader_set_ = shaderSet;
}

void SceneBridge::setMaterialManager(vine::raw_ptr<VsgMaterialManager> manager)
{
    material_manager_ = manager;
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
    // Root of the retained subtree (matrix transform -> state group).
    ::vsg::ref_ptr<::vsg::MatrixTransform> transform;
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

void SceneBridge::clearCache()
{
    cache_.clear();
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

        // The geometry data, the material binding, the effective render
        // state or the user program changed: the retained subtree (geometry +
        // pipeline + descriptor) must be rebuilt. Any cached write state is
        // discarded with it.
        if (item->revision != geometry->revision() || item->material != cmd.material.get() ||
            item->render_state != cmd.renderState || item->program != cmd.program.get()) {
            item->revision = geometry->revision();
            item->material = cmd.material.get();
            item->render_state = cmd.renderState;
            item->program = cmd.program.get();
            item->transform = nullptr;
            item->colors = nullptr;
            item->matrix_valid = false;
            item->last_opacity = -1.0f;
            changed = true;
        }

        if (item->transform == nullptr) {
            auto geomNode = buildGeometry(geometry, item->material, item->colors,
                                          item->render_state, item->program);
            if (geomNode == nullptr) {
                // Unsupported shape / empty mesh: nothing drawable.
                cache_.erase(geometry);
                continue;
            }
            item->transform = ::vsg::MatrixTransform::create();
            item->transform->addChild(geomNode);
            // Fresh buffers: the first sync must write matrix and alpha.
            item->matrix_valid = false;
            item->last_opacity = -1.0f;
            // New/rebuild subtrees must be GPU-compiled before recording.
            if (created != nullptr) {
                created->emplace_back(item->transform);
            }
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
    // (the descriptor already points at these cached Phong values).
    if (!commands.empty()) {
        auto* manager = material_manager_ != nullptr ? material_manager_ : &default_manager_;
        for (const auto& cmd : commands) {
            if (cmd.material == nullptr) {
                continue;
            }
            auto value = manager->getOrCreate(cmd.material.get());
            auto& m = value->value();
            const auto diffuse  = cmd.material->diffuse();
            const auto specular = cmd.material->specular();
            const auto ambient  = cmd.material->ambient();
            m.ambient  = ::vsg::vec4(ambient.r, ambient.g, ambient.b, ambient.a);
            // Opacity is carried by the per-vertex alpha, not the shared
            // material, so per-geometry opacity stays independent.
            m.diffuse  = ::vsg::vec4(diffuse.r, diffuse.g, diffuse.b, 1.0f);
            m.specular = ::vsg::vec4(specular.r, specular.g, specular.b, specular.a);
            m.shininess = cmd.material->shininess();
        }
    }

    return changed;
}

::vsg::ref_ptr<::vsg::Node> SceneBridge::buildGeometry(
    vine::raw_ptr<const vine::graphics::Geometry> geometry,
    vine::raw_ptr<vine::graphics::Material> material,
    ::vsg::ref_ptr<::vsg::vec4Array>& out_colors,
    const vine::graphics::ResolvedRenderState& state,
    vine::raw_ptr<const vine::graphics::ShaderProgram> program)
{
    if (geometry == nullptr) {
        return ::vsg::ref_ptr<::vsg::Node>();
    }
    // Materialise the open attribute list into typed CPU arrays for the vsg
    // build: location 0 = positions, location 1 = (optional) normals.
    const auto* position_attr = geometry->buffer(0);
    if (position_attr == nullptr || position_attr->empty() ||
        position_attr->components < 3u) {
        return ::vsg::ref_ptr<::vsg::Node>();
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

    // A user program replaces the built-in pipeline: compile its stages and
    // assemble a custom ShaderSet following the official vsg contract
    // (vsg_Vertex + "pc" projection/modelView push constant). On any failure
    // fall back to the built-in default so a bad program cannot break a scene.
    bool program_path = false;
    ::vsg::ref_ptr<::vsg::ShaderSet> shaderSet;
    if (program != nullptr) {
        const auto base_states =
            (shader_set_ != nullptr ? shader_set_ : ::vsg::createPhongShaderSet())
                ->defaultGraphicsPipelineStates;
        shaderSet = buildProgramShaderSet(program, base_states);
        program_path = shaderSet != nullptr;
    }
    if (!shaderSet) {
        shaderSet = shader_set_ != nullptr ? shader_set_ : ::vsg::createPhongShaderSet();
    }
    auto config = ::vsg::GraphicsPipelineConfigurator::create(shaderSet);

    ::vsg::DataList arrays;
    config->assignArray(arrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, vertices);

    if (!program_path) {
        ::vsg::ref_ptr<::vsg::vec4Array> colors = makeWhiteColors(vertices->size());
        out_colors = colors;
        config->assignArray(arrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normals);
        config->assignArray(arrays, "vsg_Color", VK_VERTEX_INPUT_RATE_VERTEX, colors);

        // Material resources come from the material manager (converted +
        // cached), never built ad-hoc here.
        auto material_manager = material_manager_ != nullptr ? material_manager_ : &default_manager_;
        auto material_value = material_manager->getOrCreate(material);
        config->assignDescriptor("material", material_value);
    } else {
        // A user program draws with its own colors; the per-vertex-alpha
        // opacity path (default materials) does not apply here.
        out_colors = nullptr;
    }

    // Assemble the pipeline from the geometry's effective render state. The
    // mapped color blend keeps alpha blending enabled on every pipeline (the
    // per-vertex opacity alpha may drop below 1 at any time without a rebuild);
    // depth, culling, polygon mode, blend factors and topology come from the
    // StateNode fold carried by the command.
    const RenderStateObjects states = makeRenderStateObjects(state);
    applyRenderStateObjects(*config, states);

    config->init();

    auto stateGroup = ::vsg::StateGroup::create();
    config->copyTo(stateGroup, shared_objects_);

    // NOTE: manual geometry must use explicit bind/draw commands, NOT a
    // manually-assembled VertexIndexDraw, or nothing is rasterized (same
    // finding as VsgRenderer::makeRawDemoNode).
    auto drawCommands = ::vsg::Commands::create();
    drawCommands->addChild(
        ::vsg::BindVertexBuffers::create(config->baseAttributeBinding, arrays));
    drawCommands->addChild(::vsg::BindIndexBuffer::create(indices));
    drawCommands->addChild(::vsg::DrawIndexed::create(
        static_cast<uint32_t>(indices->size()), 1, 0, 0, 0));
    stateGroup->addChild(drawCommands);

    return stateGroup;
}

V_VSG_NS_END
