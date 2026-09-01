#include <vine/vsg/SceneBridge.hpp>

#include <vine/graphics/Geometry.hpp>
#include <vine/graphics/Node.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/geometry/IndexedTriangleMesh.hpp>
#include <vine/geometry/TriangleMesh.hpp>
#include <vsg/core/Array.h>
#include <vsg/maths/mat4.h>
#include <vsg/nodes/Geometry.h>
#include <vsg/nodes/Group.h>
#include <vsg/nodes/MatrixTransform.h>
#include <vsg/nodes/StateGroup.h>
#include <vsg/nodes/VertexIndexDraw.h>

#include "VsgUtils.hpp"

V_VSG_NS_BEGIN

namespace
{

/**
 * @brief Builds a vsg drawable node from vertex and index arrays.
 *
 * @param vertices Vertex positions.
 * @param indices  Triangle indices.
 * @return vsg geometry node.
 */
::vsg::ref_ptr<::vsg::Node> makeDrawable(::vsg::ref_ptr<::vsg::vec3Array> vertices,
                                         ::vsg::ref_ptr<::vsg::uintArray> indices)
{
    auto vid = ::vsg::VertexIndexDraw::create();
    vid->assignArrays(::vsg::DataList{ vertices });
    vid->assignIndices(indices);
    vid->indexCount = static_cast<uint32_t>(indices->size());

    auto geometry = ::vsg::Geometry::create();
    geometry->commands.push_back(vid);
    return geometry;
}

}  // namespace

::vsg::ref_ptr<::vsg::Node> SceneBridge::build(vine::graphics::Scene* scene)
{
    if (scene == nullptr) {
        return ::vsg::ref_ptr<::vsg::Node>();
    }
    auto group = ::vsg::Group::create();
    for (const auto& node : scene->nodes()) {
        group->addChild(buildNode(node.get()));
    }
    return group;
}

::vsg::ref_ptr<::vsg::Node> SceneBridge::buildNode(vine::graphics::Node* node)
{
    auto transform = ::vsg::MatrixTransform::create();
    transform->matrix = detail::toVsg(node->localTransform());

    for (const auto& drawable : node->drawables()) {
        if (auto* geometry = dynamic_cast<vine::graphics::Geometry*>(drawable.get())) {
            ::vsg::ref_ptr<::vsg::Node> geomNode = buildGeometry(geometry);
            if (geomNode != nullptr) {
                // One Vine drawable maps to a transform -> state -> geometry
                // chain in VSG: the StateGroup will carry the material's
                // pipeline/descriptor state.
                auto stateGroup = ::vsg::StateGroup::create();
                stateGroup->addChild(geomNode);
                transform->addChild(stateGroup);
            }
        }
    }
    for (const auto& child : node->children()) {
        transform->addChild(buildNode(child.get()));
    }
    return transform;
}

::vsg::ref_ptr<::vsg::Node> SceneBridge::buildGeometry(vine::graphics::Geometry* geometry)
{
    const auto* shape = geometry->shape();
    if (shape == nullptr) {
        return ::vsg::ref_ptr<::vsg::Node>();
    }

    switch (shape->shapeType()) {
        case vine::geometry::ShapeType::TriangleMesh: {
            const auto* mesh = dynamic_cast<const vine::geometry::TriangleMesh*>(shape);
            if (mesh == nullptr) {
                return ::vsg::ref_ptr<::vsg::Node>();
            }
            auto vertices = ::vsg::vec3Array::create(static_cast<uint32_t>(mesh->positions().size()));
            for (std::size_t i = 0; i < mesh->positions().size(); ++i) {
                const auto& v = mesh->positions()[i];
                (*vertices)[i] = ::vsg::vec3(v.x, v.y, v.z);
            }
            auto indices = ::vsg::uintArray::create(static_cast<uint32_t>(vertices->size()));
            for (uint32_t i = 0; i < vertices->size(); ++i) {
                (*indices)[i] = i;
            }
            return makeDrawable(vertices, indices);
        }
        case vine::geometry::ShapeType::IndexedTriangleMesh: {
            const auto* mesh = dynamic_cast<const vine::geometry::IndexedTriangleMesh*>(shape);
            if (mesh == nullptr) {
                return ::vsg::ref_ptr<::vsg::Node>();
            }
            auto vertices = ::vsg::vec3Array::create(static_cast<uint32_t>(mesh->positions().size()));
            for (std::size_t i = 0; i < mesh->positions().size(); ++i) {
                const auto& v = mesh->positions()[i];
                (*vertices)[i] = ::vsg::vec3(v.x, v.y, v.z);
            }
            auto indices = ::vsg::uintArray::create(static_cast<uint32_t>(mesh->indices().size()));
            for (std::size_t i = 0; i < mesh->indices().size(); ++i) {
                (*indices)[i] = mesh->indices()[i];
            }
            return makeDrawable(vertices, indices);
        }
        default:
            return ::vsg::ref_ptr<::vsg::Node>();
    }
}

V_VSG_NS_END
