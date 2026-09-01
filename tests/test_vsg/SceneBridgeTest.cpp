#include <vine/vsg/SceneBridge.hpp>
#include <vine/vsg/CameraBridge.hpp>
#include <vine/vsg/VsgRenderer.hpp>
#include <vine/graphics/Camera.hpp>

#include <vine/graphics/Geometry.hpp>
#include <vine/graphics/Node.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/geometry/IndexedTriangleMesh.hpp>
#include <vine/geometry/TriangleMesh.hpp>
#include <vine/math/Transform3.hpp>
#include <vine/math/Math.hpp>
#include <cmath>

#include <vsg/core/Array.h>
#include <vsg/maths/mat4.h>
#include <vsg/nodes/Geometry.h>
#include <vsg/nodes/Group.h>
#include <vsg/nodes/MatrixTransform.h>
#include <vsg/nodes/StateGroup.h>
#include <vsg/nodes/VertexIndexDraw.h>

#include <gtest/gtest.h>

using vine::graphics::Geometry;
using vine::graphics::Node;
using vine::graphics::Scene;
using vine::intrusive_ptr;

namespace
{

/**
 * @brief Builds a unit triangle mesh in the XY plane.
 *
 * Vertices: (0,0,0), (1,0,0), (0,1,0).
 */
intrusive_ptr<vine::geometry::TriangleMesh> makeUnitTriangle()
{
    auto mesh = intrusive_ptr<vine::geometry::TriangleMesh>(new vine::geometry::TriangleMesh());
    mesh->addTriangle(vine::math::Vec3f(0.0f, 0.0f, 0.0f),
                      vine::math::Vec3f(1.0f, 0.0f, 0.0f),
                      vine::math::Vec3f(0.0f, 1.0f, 0.0f));
    return mesh;
}

/**
 * @brief Builds an indexed quad mesh in the XY plane.
 */
intrusive_ptr<vine::geometry::IndexedTriangleMesh> makeIndexedQuad()
{
    auto mesh = intrusive_ptr<vine::geometry::IndexedTriangleMesh>(new vine::geometry::IndexedTriangleMesh());
    mesh->setPositions({ vine::math::Vec3f(0, 0, 0), vine::math::Vec3f(1, 0, 0),
                         vine::math::Vec3f(1, 1, 0), vine::math::Vec3f(0, 1, 0) });
    mesh->setIndices({ 0, 1, 2, 0, 2, 3 });
    return mesh;
}

}  // namespace

// ============ SceneBridge ============

TEST(SceneBridgeTest, BuildEmptyScene)
{
    Scene scene;
    vine::vsg::SceneBridge bridge;
    auto root = bridge.build(&scene);

    ASSERT_NE(root, nullptr);
    auto group = root->cast<vsg::Group>();
    ASSERT_NE(group, nullptr);
    EXPECT_EQ(group->children.size(), 0u);
}

TEST(SceneBridgeTest, BuildSingleGeometry)
{
    Scene scene;
    auto node = intrusive_ptr<Node>(new Node());
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = makeUnitTriangle();
    geom->setShape(mesh.get());
    node->addDrawable(geom.get());
    scene.addNode(node.get());

    vine::vsg::SceneBridge bridge;
    auto root = bridge.build(&scene);

    ASSERT_NE(root, nullptr);
    auto group = root->cast<vsg::Group>();
    ASSERT_NE(group, nullptr);
    ASSERT_EQ(group->children.size(), 1u);

    // Node -> MatrixTransform -> StateGroup -> Geometry
    auto transform = group->children[0]->cast<vsg::MatrixTransform>();
    ASSERT_NE(transform, nullptr);
    ASSERT_EQ(transform->children.size(), 1u);

    auto stateGroup = transform->children[0]->cast<vsg::StateGroup>();
    ASSERT_NE(stateGroup, nullptr);
    ASSERT_EQ(stateGroup->children.size(), 1u);

    // Geometry with a VertexIndexDraw
    auto geomNode = stateGroup->children[0]->cast<vsg::Geometry>();
    ASSERT_NE(geomNode, nullptr);
    ASSERT_EQ(geomNode->commands.size(), 1u);
    auto vid = geomNode->commands[0]->cast<vsg::VertexIndexDraw>();
    ASSERT_NE(vid, nullptr);

    EXPECT_EQ(vid->indexCount, 3u);
    EXPECT_EQ(vid->arrays.size(), 1u);
    auto verts = vid->arrays[0]->data.cast<vsg::vec3Array>();
    ASSERT_NE(verts, nullptr);
    EXPECT_EQ(verts->size(), 3u);
    EXPECT_FLOAT_EQ((*verts)[0].x, 0.0f);
    EXPECT_FLOAT_EQ((*verts)[1].x, 1.0f);
}

TEST(SceneBridgeTest, BuildHierarchy)
{
    Scene scene;
    auto parent = intrusive_ptr<Node>(new Node());
    auto child = intrusive_ptr<Node>(new Node());
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = makeUnitTriangle();
    geom->setShape(mesh.get());
    child->addDrawable(geom.get());
    parent->addChild(child.get());
    scene.addNode(parent.get());

    vine::vsg::SceneBridge bridge;
    auto root = bridge.build(&scene);
    auto group = root->cast<vsg::Group>();
    ASSERT_NE(group, nullptr);
    ASSERT_EQ(group->children.size(), 1u);

    auto parentTransform = group->children[0]->cast<vsg::MatrixTransform>();
    ASSERT_NE(parentTransform, nullptr);
    ASSERT_EQ(parentTransform->children.size(), 1u);
    auto childTransform = parentTransform->children[0]->cast<vsg::MatrixTransform>();
    ASSERT_NE(childTransform, nullptr);
    ASSERT_EQ(childTransform->children.size(), 1u);
    auto stateGroup = childTransform->children[0]->cast<vsg::StateGroup>();
    ASSERT_NE(stateGroup, nullptr);
    ASSERT_EQ(stateGroup->children.size(), 1u);
}

TEST(SceneBridgeTest, BuildIndexedMesh)
{
    Scene scene;
    auto node = intrusive_ptr<Node>(new Node());
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = makeIndexedQuad();
    geom->setShape(mesh.get());
    node->addDrawable(geom.get());
    scene.addNode(node.get());

    vine::vsg::SceneBridge bridge;
    auto root = bridge.build(&scene);
    auto group = root->cast<vsg::Group>();
    ASSERT_NE(group, nullptr);
    auto transform = group->children[0]->cast<vsg::MatrixTransform>();
    ASSERT_NE(transform, nullptr);
    auto stateGroup = transform->children[0]->cast<vsg::StateGroup>();
    ASSERT_NE(stateGroup, nullptr);
    auto geomNode = stateGroup->children[0]->cast<vsg::Geometry>();
    ASSERT_NE(geomNode, nullptr);
    auto vid = geomNode->commands[0]->cast<vsg::VertexIndexDraw>();
    ASSERT_NE(vid, nullptr);

    EXPECT_EQ(vid->indexCount, 6u);
    auto verts = vid->arrays[0]->data.cast<vsg::vec3Array>();
    ASSERT_NE(verts, nullptr);
    EXPECT_EQ(verts->size(), 4u);
}

// ============ CameraBridge ============

TEST(CameraBridgeTest, CreateFromVineCamera)
{
    vine::graphics::Camera vineCam;
    vineCam.setViewMatrixAsLookAt(vine::math::Vec3d(0, 0, 5), vine::math::Vec3d(0, 0, 0),
                                  vine::math::Vec3d(0, 1, 0));
    vineCam.setProjectionMatrixAsPerspective(60.0, 16.0 / 9.0, 0.1, 1000.0);

    vine::vsg::CameraBridge bridge;
    auto vsgCam = bridge.create(&vineCam);

    ASSERT_NE(vsgCam, nullptr);

    // View: should be a LookAt mirroring eye (0,0,5) looking at origin.
    auto lookAt = vsgCam->viewMatrix.cast<vsg::LookAt>();
    ASSERT_NE(lookAt, nullptr);
    EXPECT_NEAR(lookAt->eye.z, 5.0, 1e-9);
    EXPECT_NEAR(lookAt->center.x, 0.0, 1e-9);

    // Projection: should be a Perspective with fieldOfViewY in degrees.
    auto persp = vsgCam->projectionMatrix.cast<vsg::Perspective>();
    ASSERT_NE(persp, nullptr);
    EXPECT_NEAR(persp->fieldOfViewY, 60.0, 1e-9);
    EXPECT_NEAR(persp->aspectRatio, 16.0 / 9.0, 1e-9);
    EXPECT_NEAR(persp->nearDistance, 0.1, 1e-9);
    EXPECT_NEAR(persp->farDistance, 1000.0, 1e-9);
}

TEST(CameraBridgeTest, CreateOrthographic)
{
    vine::graphics::Camera vineCam;
    vineCam.setViewMatrixAsLookAt(vine::math::Vec3d(0, 0, 10), vine::math::Vec3d(0, 0, 0),
                                  vine::math::Vec3d(0, 1, 0));
    vineCam.setProjectionMatrixAsOrtho(-5, 5, -5, 5, 0.1, 1000.0);

    vine::vsg::CameraBridge bridge;
    auto vsgCam = bridge.create(&vineCam);

    auto ortho = vsgCam->projectionMatrix.cast<vsg::Orthographic>();
    ASSERT_NE(ortho, nullptr);
    EXPECT_NEAR(ortho->left, -5.0, 1e-9);
    EXPECT_NEAR(ortho->right, 5.0, 1e-9);
    EXPECT_NEAR(ortho->bottom, -5.0, 1e-9);
    EXPECT_NEAR(ortho->top, 5.0, 1e-9);
}

TEST(CameraBridgeTest, ApplyUpdatesInPlace)
{
    vine::graphics::Camera vineCam;
    vineCam.setViewMatrixAsLookAt(vine::math::Vec3d(0, 0, 5), vine::math::Vec3d(0, 0, 0),
                                  vine::math::Vec3d(0, 1, 0));
    vineCam.setProjectionMatrixAsPerspective(60.0, 16.0 / 9.0, 0.1, 1000.0);

    vine::vsg::CameraBridge bridge;
    auto vsgCam = bridge.create(&vineCam);

    // Move the eye and sync.
    vineCam.setViewMatrixAsLookAt(vine::math::Vec3d(0, 0, 20), vine::math::Vec3d(0, 0, 0),
                                  vine::math::Vec3d(0, 1, 0));
    bridge.apply(&vineCam, vsgCam);

    auto lookAt = vsgCam->viewMatrix.cast<vsg::LookAt>();
    ASSERT_NE(lookAt, nullptr);
    EXPECT_NEAR(lookAt->eye.z, 20.0, 1e-9);
}



// ============ VsgRenderer (POC) ============

TEST(VsgRendererTest, InitializeAndRenderFrames)
{
    // Build a small Vine scene: one node with a triangle geometry.
    auto scene = intrusive_ptr<Scene>(new Scene());
    auto node = intrusive_ptr<Node>(new Node());
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = makeUnitTriangle();
    geom->setShape(mesh.get());
    node->addDrawable(geom.get());
    scene->addNode(node.get());

    // Camera looking at the triangle from +Z.
    auto camera = intrusive_ptr<vine::graphics::Camera>(new vine::graphics::Camera());
    camera->setViewMatrixAsLookAt(vine::math::Vec3d(0, 0, 3), vine::math::Vec3d(0, 0, 0),
                                  vine::math::Vec3d(0, 1, 0));
    camera->setProjectionMatrixAsPerspective(60.0, 16.0 / 9.0, 0.1, 100.0);

    vine::vsg::VsgRenderer renderer(scene.get(), camera.get());
    bool ok = renderer.initialize();
    ASSERT_TRUE(ok);

    EXPECT_NE(renderer.viewer(), nullptr);
    EXPECT_NE(renderer.vsgCamera(), nullptr);
    EXPECT_NE(renderer.vsgScene(), nullptr);

    // Render a couple of frames to exercise the pipeline.
    for (int i = 0; i < 3; ++i) {
        renderer.frame();
    }

    renderer.shutdown();
    EXPECT_EQ(renderer.viewer(), nullptr);
}
