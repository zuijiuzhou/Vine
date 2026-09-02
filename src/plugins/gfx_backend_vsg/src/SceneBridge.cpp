#include <vine/vsg/SceneBridge.hpp>

#include <vine/graphics/Geometry.hpp>
#include <vine/graphics/Material.hpp>
#include <vine/graphics/Node.hpp>
#include <vine/graphics/RenderCommand.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/geometry/IndexedTriangleMesh.hpp>
#include <vine/geometry/TriangleMesh.hpp>
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
#include <vsg/utils/GraphicsPipelineConfigurator.h>
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
 * @brief Enables alpha blending on a pipeline configurator.
 *
 * Required for transparent materials: without blending the fragment's alpha
 * is ignored and the surface renders opaque.
 *
 * @param config Pipeline configurator to modify.
 */
void enableBlending(::vsg::ref_ptr<::vsg::GraphicsPipelineConfigurator> config)
{
    for (auto& state : config->pipelineStates) {
        if (auto* cbs = state->cast<::vsg::ColorBlendState>()) {
            cbs->configureAttachments(true);
            return;
        }
    }
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

}  // namespace

SceneBridge::SceneBridge() = default;

SceneBridge::~SceneBridge() = default;

void SceneBridge::setShaderSet(::vsg::ref_ptr<::vsg::ShaderSet> shaderSet)
{
    shader_set_ = shaderSet;
}

void SceneBridge::setMaterialManager(VsgMaterialManager* manager)
{
    material_manager_ = manager;
}

/** @brief Retained vsg node for one drawn geometry. */
struct SceneBridge::Item {
    // Last translated identity, used to detect geometry/material changes.
    vine::graphics::Material* material = nullptr;
    const vine::geometry::Shape* shape = nullptr;
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
        const auto* geometry =
            dynamic_cast<const vine::graphics::Geometry*>(cmd.drawable.get());
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

        // The mesh buffers or the material binding changed: the retained
        // subtree (geometry + pipeline + descriptor) must be rebuilt. Any
        // cached write state is discarded with it.
        if (item->shape != geometry->shape() || item->material != cmd.material.get()) {
            item->shape = geometry->shape();
            item->material = cmd.material.get();
            item->transform = nullptr;
            item->colors = nullptr;
            item->matrix_valid = false;
            item->last_opacity = -1.0f;
            changed = true;
        }

        if (item->transform == nullptr) {
            auto geomNode = buildGeometry(geometry, item->material, item->colors);
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

        // Effective opacity (scene x node x drawable x material) rides the
        // per-vertex alpha. Rewriting O(vertices) only when it actually
        // changed keeps the steady-state per-frame cost independent of mesh
        // size, while 3-tier transparency edits still apply live.
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
            // material, so per-drawable 3-tier opacity stays independent.
            m.diffuse  = ::vsg::vec4(diffuse.r, diffuse.g, diffuse.b, 1.0f);
            m.specular = ::vsg::vec4(specular.r, specular.g, specular.b, specular.a);
            m.shininess = cmd.material->shininess();
        }
    }

    return changed;
}

::vsg::ref_ptr<::vsg::Node> SceneBridge::buildGeometry(
    const vine::graphics::Geometry* geometry,
    vine::graphics::Material* material,
    ::vsg::ref_ptr<::vsg::vec4Array>& out_colors)
{
    if (geometry == nullptr) {
        return ::vsg::ref_ptr<::vsg::Node>();
    }
    const auto* shape = geometry->shape();
    if (shape == nullptr) {
        return ::vsg::ref_ptr<::vsg::Node>();
    }

    ::vsg::ref_ptr<::vsg::vec3Array> vertices;
    ::vsg::ref_ptr<::vsg::vec3Array> normals;
    ::vsg::ref_ptr<::vsg::uintArray> indices;

    switch (shape->shapeType()) {
        case vine::geometry::ShapeType::TriangleMesh: {
            const auto* mesh = dynamic_cast<const vine::geometry::TriangleMesh*>(shape);
            if (mesh == nullptr) {
                return ::vsg::ref_ptr<::vsg::Node>();
            }
            vertices = ::vsg::vec3Array::create(static_cast<uint32_t>(mesh->positions().size()));
            for (std::size_t i = 0; i < mesh->positions().size(); ++i) {
                const auto& v = mesh->positions()[i];
                (*vertices)[i] = ::vsg::vec3(v.x, v.y, v.z);
            }
            normals = makeNormals(mesh->positions(), mesh->normals());
            indices = ::vsg::uintArray::create(static_cast<uint32_t>(vertices->size()));
            for (uint32_t i = 0; i < vertices->size(); ++i) {
                (*indices)[i] = i;
            }
            break;
        }
        case vine::geometry::ShapeType::IndexedTriangleMesh: {
            const auto* mesh = dynamic_cast<const vine::geometry::IndexedTriangleMesh*>(shape);
            if (mesh == nullptr) {
                return ::vsg::ref_ptr<::vsg::Node>();
            }
            vertices = ::vsg::vec3Array::create(static_cast<uint32_t>(mesh->positions().size()));
            for (std::size_t i = 0; i < mesh->positions().size(); ++i) {
                const auto& v = mesh->positions()[i];
                (*vertices)[i] = ::vsg::vec3(v.x, v.y, v.z);
            }
            normals = makeIndexedNormals(mesh->positions(), mesh->normals(), mesh->indices());
            indices = ::vsg::uintArray::create(static_cast<uint32_t>(mesh->indices().size()));
            for (std::size_t i = 0; i < mesh->indices().size(); ++i) {
                (*indices)[i] = mesh->indices()[i];
            }
            break;
        }
        default:
            return ::vsg::ref_ptr<::vsg::Node>();
    }

    // Per-geometry pipeline: carries the Phong material descriptor so each
    // drawable renders with its own material properties.
    auto shaderSet = shader_set_ != nullptr ? shader_set_ : ::vsg::createPhongShaderSet();
    auto config = ::vsg::GraphicsPipelineConfigurator::create(shaderSet);

    ::vsg::ref_ptr<::vsg::vec4Array> colors = makeWhiteColors(vertices->size());
    out_colors = colors;

    ::vsg::DataList arrays;
    config->assignArray(arrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, vertices);
    config->assignArray(arrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normals);
    config->assignArray(arrays, "vsg_Color", VK_VERTEX_INPUT_RATE_VERTEX, colors);

    // Material resources come from the material manager (converted + cached),
    // never built ad-hoc here.
    auto material_manager = material_manager_ != nullptr ? material_manager_ : &default_manager_;
    auto material_value = material_manager->getOrCreate(material);
    config->assignDescriptor("material", material_value);
    // Blending stays enabled for every pipeline: the effective per-drawable
    // opacity (scene x node x drawable x material) is carried by the
    // per-vertex alpha and may drop below 1 at any time without a pipeline
    // change, so transparency toggles never rebuild or recompile.
    enableBlending(config);

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
