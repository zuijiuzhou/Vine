#include <vine/graphics/BoundingBox.hpp>
#include <vine/graphics/Camera.hpp>
#include <vine/graphics/CameraManipulator.hpp>
#include <vine/graphics/Geometry.hpp>
#include <vine/graphics/Node.hpp>
#include <vine/graphics/Material.hpp>
#include <vine/graphics/Ray.hpp>
#include <vine/graphics/RenderBackend.hpp>
#include <vine/graphics/RenderEngine.hpp>
#include <vine/graphics/RenderPass.hpp>
#include <vine/graphics/RayIntersection.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/geometry/TriangleMesh.hpp>
#include <vine/Colorf.hpp>
#include <vine/math/Transform3.hpp>

#include <gtest/gtest.h>

using namespace vine::graphics;
using vine::intrusive_ptr;
using vine::Colorf;
using vine::Color;
using vine::math::Vec2d;
using vine::math::Vec3d;
using vine::math::Mat4d;

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

}  // namespace

// ============ Scene ============

TEST(SceneTest, AddRemoveFind)
{
    Scene scene;
    scene.setName(u8"test-scene");
    EXPECT_EQ(scene.name(), u8"test-scene");

    auto node = intrusive_ptr<Node>(new Node());
    node->setName(u8"node1");
    scene.addNode(node.get());

    EXPECT_EQ(scene.nodes().size(), 1u);
    auto found = scene.findNode(u8"node1");
    EXPECT_EQ(found.get(), node.get());
    EXPECT_EQ(scene.findNode(u8"nope"), nullptr);

    scene.removeNode(node.get());
    EXPECT_EQ(scene.nodes().size(), 0u);
}

TEST(SceneTest, NullAddIsIgnored)
{
    Scene scene;
    scene.addNode(nullptr);
    EXPECT_EQ(scene.nodes().size(), 0u);
}

TEST(SceneTest, BoundingBoxAggregates)
{
    Scene scene;
    auto node = intrusive_ptr<Node>(new Node());
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = makeUnitTriangle();
    geom->setShape(mesh.get());
    node->addDrawable(geom.get());
    scene.addNode(node.get());

    BoundingBox box = scene.boundingBox();
    EXPECT_TRUE(box.isValid());
    EXPECT_NEAR(box.min.x, 0.0, 1e-9);
    EXPECT_NEAR(box.max.x, 1.0, 1e-9);
    EXPECT_NEAR(box.max.y, 1.0, 1e-9);
}

TEST(SceneTest, InvisibleNodeExcludedFromBoundingBox)
{
    Scene scene;
    auto node = intrusive_ptr<Node>(new Node());
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = makeUnitTriangle();
    geom->setShape(mesh.get());
    node->addDrawable(geom.get());
    node->setVisible(false);
    scene.addNode(node.get());

    BoundingBox box = scene.boundingBox();
    EXPECT_TRUE(box.isEmpty());
}

// ============ Camera ============

TEST(CameraTest, Defaults)
{
    Camera cam;
    EXPECT_EQ(cam.projectionType(), Camera::ProjectionType::Perspective);
    EXPECT_NEAR(cam.fieldOfView(), 60.0, 1e-9);
    EXPECT_NEAR(cam.nearPlane(), 0.1, 1e-9);
    EXPECT_NEAR(cam.farPlane(), 1000.0, 1e-9);
    EXPECT_EQ(cam.eye(), Vec3d(0, 0, 5));
    EXPECT_EQ(cam.target(), Vec3d(0, 0, 0));
    EXPECT_EQ(cam.up(), Vec3d(0, 1, 0));
}

TEST(CameraTest, ViewMatrixLooksAtTarget)
{
    Camera cam;
    cam.setViewMatrixAsLookAt(Vec3d(0, 0, 5), Vec3d(0, 0, 0), Vec3d(0, 1, 0));

    Mat4d view = cam.viewMatrix();
    // Camera looking down -Z: the origin should be at z = -5 in view space.
    const auto origin_in_view = view * vine::math::Point3d(0, 0, 0);
    EXPECT_NEAR(origin_in_view.x, 0.0, 1e-6);
    EXPECT_NEAR(origin_in_view.y, 0.0, 1e-6);
    EXPECT_NEAR(origin_in_view.z, -5.0, 1e-6);
}

TEST(CameraTest, ScreenToWorldRayCenter)
{
    Camera cam;
    cam.setViewMatrixAsLookAt(Vec3d(0, 0, 5), Vec3d(0, 0, 0), Vec3d(0, 1, 0));
    cam.setProjectionMatrixAsPerspective(60.0, 800.0 / 600.0, 0.1, 1000.0);

    // Screen center maps to a ray through the target.
    Ray ray = cam.screenToWorldRay(Vec2d(0.5, 0.5));
    EXPECT_NEAR(ray.origin.x, 0.0, 1e-6);
    EXPECT_NEAR(ray.origin.y, 0.0, 1e-6);
    EXPECT_NEAR(ray.origin.z, 5.0, 1e-6);
    // Direction should point towards -Z.
    EXPECT_NEAR(ray.direction.z, -1.0, 1e-6);
    EXPECT_NEAR(ray.direction.x, 0.0, 1e-6);
}

// ============ CameraManipulator ============

TEST(CameraManipulatorTest, OrbitKeepsTarget)
{
    auto cam = intrusive_ptr<Camera>(new Camera());
    cam->setViewMatrixAsLookAt(Vec3d(0, 0, 10), Vec3d(0, 0, 0), Vec3d(0, 1, 0));

    CameraManipulator manip(cam.get());
    manip.orbit(0.5, 0.0);

    // Target remains fixed at origin.
    EXPECT_NEAR(cam->target().x, 0.0, 1e-6);
    EXPECT_NEAR(cam->target().y, 0.0, 1e-6);
    EXPECT_NEAR(cam->target().z, 0.0, 1e-6);
    // Eye moved off the original axis.
    EXPECT_GT(std::abs(cam->eye().x), 1e-3);
}

TEST(CameraManipulatorTest, ZoomChangesRadius)
{
    auto cam = intrusive_ptr<Camera>(new Camera());
    cam->setViewMatrixAsLookAt(Vec3d(0, 0, 10), Vec3d(0, 0, 0), Vec3d(0, 1, 0));

    CameraManipulator manip(cam.get());
    const double before = manip.orbitRadius();
    manip.zoom(0.5);
    EXPECT_LT(manip.orbitRadius(), before);
    const double distance = (cam->eye() - cam->target()).length();
    EXPECT_NEAR(distance, manip.orbitRadius(), 1e-6);
}

// ============ Ray ============

TEST(RayTest, PointAt)
{
    Ray ray(Vec3d(1, 2, 3), Vec3d(1, 0, 0));
    Vec3d p = ray.pointAt(4.0);
    EXPECT_NEAR(p.x, 5.0, 1e-9);
    EXPECT_NEAR(p.y, 2.0, 1e-9);
    EXPECT_NEAR(p.z, 3.0, 1e-9);
}

TEST(RayTest, DistanceToPoint)
{
    Ray ray(Vec3d(0, 0, 0), Vec3d(1, 0, 0));
    EXPECT_NEAR(ray.distanceToPoint(Vec3d(5, 3, 0)), 3.0, 1e-9);
    // Point behind the ray origin: distance to origin.
    EXPECT_NEAR(ray.distanceToPoint(Vec3d(-5, 0, 0)), 5.0, 1e-9);
}

// ============ RayIntersection ============

TEST(RayIntersectionTest, HitsTriangle)
{
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = makeUnitTriangle();
    geom->setShape(mesh.get());

    Ray ray(Vec3d(0.25, 0.25, 1.0), Vec3d(0, 0, -1));
    RayIntersectionResult result = RayIntersection::intersect(ray, geom.get(), Mat4d());
    EXPECT_TRUE(result.hit);
    EXPECT_NEAR(result.distance, 1.0, 1e-6);
    EXPECT_NEAR(result.point.x, 0.25, 1e-6);
    EXPECT_NEAR(result.point.y, 0.25, 1e-6);
    // Normal of CCW triangle in XY plane points +Z.
    EXPECT_NEAR(result.normal.z, 1.0, 1e-6);
}

TEST(RayIntersectionTest, MissesTriangle)
{
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = makeUnitTriangle();
    geom->setShape(mesh.get());

    // Ray aimed outside the triangle.
    Ray ray(Vec3d(5, 5, 1), Vec3d(0, 0, -1));
    RayIntersectionResult result = RayIntersection::intersect(ray, geom.get(), Mat4d());
    EXPECT_FALSE(result.hit);
}

TEST(RayIntersectionTest, SceneQuery)
{
    auto scene = intrusive_ptr<Scene>(new Scene());
    auto node = intrusive_ptr<Node>(new Node());
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = makeUnitTriangle();
    geom->setName(u8"tri");
    geom->setShape(mesh.get());
    node->addDrawable(geom.get());
    scene->addNode(node.get());

    Ray ray(Vec3d(0.25, 0.25, 1.0), Vec3d(0, 0, -1));
    RayIntersectionResult result = RayIntersection::intersectScene(ray, scene.get());
    EXPECT_TRUE(result.hit);
    EXPECT_EQ(result.geometry.get(), geom.get());
}

// ============ Material ============

TEST(MaterialTest, Defaults)
{
    Material mat;
    EXPECT_NEAR(mat.opacity(), 1.0f, 1e-6f);
    EXPECT_NEAR(mat.shininess(), 32.0f, 1e-6f);
    EXPECT_NEAR(mat.diffuse().r, 0.8f, 1e-6f);
}

TEST(MaterialTest, Setters)
{
    Material mat;
    mat.setName(u8"red");
    mat.setDiffuse(Colorf(1.0f, 0.0f, 0.0f, 1.0f));
    mat.setOpacity(0.5f);
    mat.setShininess(64.0f);
    mat.setTextureFile(u8"tex.png");

    EXPECT_EQ(mat.name(), u8"red");
    EXPECT_NEAR(mat.diffuse().r, 1.0f, 1e-6f);
    EXPECT_NEAR(mat.opacity(), 0.5f, 1e-6f);
    EXPECT_NEAR(mat.shininess(), 64.0f, 1e-6f);
    EXPECT_EQ(mat.textureFile(), u8"tex.png");
}

// ============ Geometry ============

TEST(GeometryTest, MeshCounts)
{
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = makeUnitTriangle();
    geom->setShape(mesh.get());

    EXPECT_EQ(geom->triangleCount(), 1u);
    EXPECT_EQ(geom->vertexCount(), 3u);
}

TEST(GeometryTest, NoShapeYieldsZero)
{
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    EXPECT_EQ(geom->triangleCount(), 0u);
    EXPECT_EQ(geom->vertexCount(), 0u);
    EXPECT_EQ(geom->shape(), nullptr);
}

// ============ Node ============

TEST(NodeTest, DrawableBinding)
{
    Node node;
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    node.addDrawable(geom.get());
    EXPECT_EQ(node.drawables().size(), 1u);
    EXPECT_EQ(node.drawables()[0].get(), geom.get());
    node.removeDrawable(geom.get());
    EXPECT_EQ(node.drawables().size(), 0u);
}

TEST(NodeTest, MultipleDrawables)
{
    Node node;
    auto geom1 = intrusive_ptr<Geometry>(new Geometry());
    auto geom2 = intrusive_ptr<Geometry>(new Geometry());
    node.addDrawable(geom1.get());
    node.addDrawable(geom2.get());

    EXPECT_EQ(node.drawables().size(), 2u);
    EXPECT_EQ(node.drawables()[0].get(), geom1.get());
    EXPECT_EQ(node.drawables()[1].get(), geom2.get());
}

TEST(NodeTest, ChildHierarchy)
{
    auto parent = intrusive_ptr<Node>(new Node());
    auto child = intrusive_ptr<Node>(new Node());
    parent->addChild(child.get());

    EXPECT_EQ(parent->children().size(), 1u);
    EXPECT_EQ(child->parent(), parent.get());

    parent->removeChild(child.get());
    EXPECT_EQ(parent->children().size(), 0u);
    EXPECT_EQ(child->parent(), nullptr);
}

TEST(NodeTest, WorldTransformCascades)
{
    auto parent = intrusive_ptr<Node>(new Node());
    auto child = intrusive_ptr<Node>(new Node());
    parent->addChild(child.get());

    // Parent translates by (1, 0, 0), child by (2, 0, 0).
    parent->setLocalTransform(vine::math::translate(Vec3d(1, 0, 0)));
    child->setLocalTransform(vine::math::translate(Vec3d(2, 0, 0)));

    // Child world origin = parent translation * child translation = (3, 0, 0).
    const auto origin = child->worldTransform() * vine::math::Point3d(0, 0, 0);
    EXPECT_NEAR(origin.x, 3.0, 1e-9);
    EXPECT_NEAR(origin.y, 0.0, 1e-9);
    EXPECT_NEAR(origin.z, 0.0, 1e-9);
}

TEST(NodeTest, BoundingBoxWithTransform)
{
    auto node = intrusive_ptr<Node>(new Node());
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = makeUnitTriangle();
    geom->setShape(mesh.get());
    node->addDrawable(geom.get());
    node->setLocalTransform(vine::math::translate(Vec3d(10, 0, 0)));

    BoundingBox box = node->boundingBox();
    // Local box [0,1]x[0,1] translated by +10 on X.
    EXPECT_NEAR(box.min.x, 10.0, 1e-9);
    EXPECT_NEAR(box.max.x, 11.0, 1e-9);
    EXPECT_NEAR(box.max.y, 1.0, 1e-9);
}


// ============ RenderEngine ============

/**
 * @brief Recording backend that counts calls for verification.
 */
class MockBackend : public RenderBackend {
  public:
    bool ok = false;
    int begin_calls = 0;
    int end_calls = 0;
    int swap_calls = 0;
    int render_calls = 0;
    int clear_calls = 0;

    bool initialize() override { ok = true; return true; }
    void shutdown() override { ok = false; }
    void beginFrame() override { ++begin_calls; }
    void endFrame() override { ++end_calls; }
    void executePass(const RenderPass*, const std::vector<RenderCommand>&) override {}
    void setRenderTarget(RenderTarget*) override {}
    void render(const std::vector<RenderCommand>&, const Camera*) override { ++render_calls; }
    void clear(const Color&, bool) override { ++clear_calls; }
    void swapBuffers() override { ++swap_calls; }
};

TEST(RenderEngineTest, InitializeCallsBackend)
{
    MockBackend backend;
    auto engine = intrusive_ptr<RenderEngine>(new RenderEngine(&backend));

    EXPECT_TRUE(engine->initialize());
    EXPECT_TRUE(backend.ok);
    EXPECT_EQ(engine->backend(), &backend);
}

TEST(RenderEngineTest, FrameRunsPipeline)
{
    MockBackend backend;
    auto engine = intrusive_ptr<RenderEngine>(new RenderEngine(&backend));
    engine->initialize();

    engine->frame();

    EXPECT_EQ(backend.begin_calls, 1);
    EXPECT_EQ(backend.end_calls, 1);
    EXPECT_EQ(backend.swap_calls, 1);
    EXPECT_GE(backend.clear_calls, 1);
}

TEST(RenderEngineTest, FrameBeforeInitializeIsNoOp)
{
    MockBackend backend;
    auto engine = intrusive_ptr<RenderEngine>(new RenderEngine(&backend));

    // Not initialized: frame should do nothing.
    engine->frame();
    EXPECT_EQ(backend.begin_calls, 0);
    EXPECT_EQ(backend.swap_calls, 0);
}

TEST(RenderEngineTest, DefaultObjectsCreated)
{
    MockBackend backend;
    auto engine = intrusive_ptr<RenderEngine>(new RenderEngine(&backend));

    EXPECT_NE(engine->scene(), nullptr);
    EXPECT_NE(engine->camera(), nullptr);
    EXPECT_NE(engine->mainPass(), nullptr);
    EXPECT_EQ(engine->mainPass()->camera(), engine->camera());
}

TEST(RenderEngineTest, SetSceneAndCamera)
{
    MockBackend backend;
    auto engine = intrusive_ptr<RenderEngine>(new RenderEngine(&backend));

    auto scene = intrusive_ptr<Scene>(new Scene());
    auto camera = intrusive_ptr<Camera>(new Camera());
    camera->setName(u8"cam");

    engine->setScene(scene.get());
    engine->setCamera(camera.get());

    EXPECT_EQ(engine->scene(), scene.get());
    EXPECT_EQ(engine->camera(), camera.get());
    EXPECT_EQ(engine->mainPass()->camera(), camera.get());
}

TEST(RenderEngineTest, ShutdownReleasesBackend)
{
    MockBackend backend;
    auto engine = intrusive_ptr<RenderEngine>(new RenderEngine(&backend));
    engine->initialize();
    EXPECT_TRUE(backend.ok);

    engine->shutdown();
    EXPECT_FALSE(backend.ok);
}
