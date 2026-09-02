#include <vine/graphics/Camera.hpp>
#include <vine/graphics/CameraManipulator.hpp>
#include <vine/graphics/OrbitCameraManipulator.hpp>
#include <vine/graphics/Geometry.hpp>
#include <vine/graphics/Node.hpp>
#include <vine/graphics/Material.hpp>
#include <vine/graphics/AxisGizmo.hpp>
#include <vine/graphics/Overlay.hpp>
#include <vine/graphics/Ray.hpp>
#include <vine/graphics/RenderBackend.hpp>
#include <vine/graphics/RenderBackendRegistry.hpp>
#include <vine/graphics/RenderCommand.hpp>
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
using vine::math::Aabbd;
using vine::math::Aabbf;

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
 * @brief Builds a node holding a single triangle at a world-space position.
 *
 * @param position World-space translation of the node.
 * @param material Material to apply to the geometry (may be null).
 * @param name     Name assigned to the geometry drawable.
 * @return Node with one triangle drawable.
 */
intrusive_ptr<Node> makeTriangleNode(const Vec3d& position, Material* material,
                                     const vine::String& name = {})
{
    auto node = intrusive_ptr<Node>(new Node());
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = makeUnitTriangle();
    geom->setShape(mesh.get());
    geom->setMaterial(material);
    geom->setName(name);
    node->addDrawable(geom.get());
    node->setLocalTransform(vine::math::translate(position));
    return node;
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

    Aabbd box = scene.boundingBox();
    EXPECT_TRUE(box.isValid());
    EXPECT_NEAR(box.min().x, 0.0, 1e-9);
    EXPECT_NEAR(box.max().x, 1.0, 1e-9);
    EXPECT_NEAR(box.max().y, 1.0, 1e-9);
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

    Aabbd box = scene.boundingBox();
    EXPECT_TRUE(box.isEmpty());
}

// ============ RenderCommand collection / culling ============

/**
 * @brief Configures a camera at (0,0,5) looking at the origin.
 *
 * @param cam Camera to configure in place (avoids the deleted copy ctor).
 */
void setupLookAtCamera(Camera& cam)
{
    cam.setViewMatrixAsLookAt(Vec3d(0, 0, 5), Vec3d(0, 0, 0), Vec3d(0, 1, 0));
    cam.setProjectionMatrixAsPerspective(60.0, 1.0, 0.1, 1000.0);
}

TEST(SceneTest, CollectCommandsSortsOpaqueFrontToBack)
{
    Scene scene;
    // Nearer triangle (z=-2) and farther triangle (z=-5).
    auto near_node = makeTriangleNode(Vec3d(0, 0, -2), nullptr, u8"near");
    auto far_node = makeTriangleNode(Vec3d(0, 0, -5), nullptr, u8"far");
    scene.addNode(far_node.get());
    scene.addNode(near_node.get());

    Camera cam;
    setupLookAtCamera(cam);
    auto commands = scene.collectRenderCommands(&cam);

    ASSERT_EQ(commands.size(), 2u);
    EXPECT_EQ(commands[0].drawable->name(), u8"near");
    EXPECT_EQ(commands[1].drawable->name(), u8"far");
    EXPECT_FALSE(commands[0].isTransparent);
}

TEST(SceneTest, CollectCommandsSortsTransparentBackToFrontAfterOpaque)
{
    Scene scene;
    auto opaque_mat = intrusive_ptr<Material>(new Material());
    auto trans_mat = intrusive_ptr<Material>(new Material());
    trans_mat->setOpacity(0.5f);

    // Opaque far, transparent far, transparent near.
    auto opaque_far = makeTriangleNode(Vec3d(0, 0, -10), opaque_mat.get(), u8"opaque-far");
    auto trans_far = makeTriangleNode(Vec3d(0, 0, -6), trans_mat.get(), u8"trans-far");
    auto trans_near = makeTriangleNode(Vec3d(0, 0, -2), trans_mat.get(), u8"trans-near");
    scene.addNode(trans_near.get());
    scene.addNode(opaque_far.get());
    scene.addNode(trans_far.get());

    Camera cam;
    setupLookAtCamera(cam);
    auto commands = scene.collectRenderCommands(&cam);

    ASSERT_EQ(commands.size(), 3u);
    // Opaque batch first (regardless of depth).
    EXPECT_EQ(commands[0].drawable->name(), u8"opaque-far");
    EXPECT_FALSE(commands[0].isTransparent);
    // Transparent drawn back-to-front.
    EXPECT_EQ(commands[1].drawable->name(), u8"trans-far");
    EXPECT_TRUE(commands[1].isTransparent);
    EXPECT_EQ(commands[2].drawable->name(), u8"trans-near");
    EXPECT_TRUE(commands[2].isTransparent);
}

TEST(SceneTest, CollectCommandsCullsOutOfView)
{
    Scene scene;
    // A node far outside the frustum (behind the camera) must be culled.
    auto visible = makeTriangleNode(Vec3d(0, 0, -3), nullptr, u8"visible");
    auto behind = makeTriangleNode(Vec3d(0, 0, 100), nullptr, u8"behind");
    scene.addNode(behind.get());
    scene.addNode(visible.get());

    Camera cam;
    setupLookAtCamera(cam);
    auto commands = scene.collectRenderCommands(&cam);

    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].drawable->name(), u8"visible");
}

TEST(SceneTest, CollectCommandsCullsOffToTheSide)
{
    Scene scene;
    // Far off to the side, outside the horizontal FOV.
    auto visible = makeTriangleNode(Vec3d(0, 0, -3), nullptr, u8"visible");
    auto side = makeTriangleNode(Vec3d(50, 0, -3), nullptr, u8"side");
    scene.addNode(side.get());
    scene.addNode(visible.get());

    Camera cam;
    setupLookAtCamera(cam);
    auto commands = scene.collectRenderCommands(&cam);

    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].drawable->name(), u8"visible");
}

TEST(SceneTest, CollectCommandsNullCameraYieldsEmpty)
{
    Scene scene;
    auto node = makeTriangleNode(Vec3d(0, 0, -3), nullptr);
    scene.addNode(node.get());

    auto commands = scene.collectRenderCommands(nullptr);
    EXPECT_TRUE(commands.empty());
}

TEST(SceneTest, CollectCommandsHidesWholeScene)
{
    Scene scene;
    scene.addNode(makeTriangleNode(Vec3d(0, 0, -3), nullptr, u8"tri").get());
    scene.setVisible(false);

    Camera cam;
    setupLookAtCamera(cam);
    auto commands = scene.collectRenderCommands(&cam);
    EXPECT_TRUE(commands.empty());
}

TEST(SceneTest, CollectCommandsHidesNode)
{
    Scene scene;
    auto hidden = makeTriangleNode(Vec3d(0, 0, -3), nullptr, u8"hidden");
    auto shown = makeTriangleNode(Vec3d(0, 0, -5), nullptr, u8"shown");
    hidden->setVisible(false);
    scene.addNode(hidden.get());
    scene.addNode(shown.get());

    Camera cam;
    setupLookAtCamera(cam);
    auto commands = scene.collectRenderCommands(&cam);
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].drawable->name(), u8"shown");
}

TEST(SceneTest, CollectCommandsHidesDrawable)
{
    Scene scene;
    auto node = intrusive_ptr<Node>(new Node());
    auto g1 = intrusive_ptr<Geometry>(new Geometry());
    g1->setShape(makeUnitTriangle().get());
    g1->setName(u8"g1");
    auto g2 = intrusive_ptr<Geometry>(new Geometry());
    g2->setShape(makeUnitTriangle().get());
    g2->setName(u8"g2");
    g1->setVisible(false);
    node->addDrawable(g1.get());
    node->addDrawable(g2.get());
    node->setLocalTransform(vine::math::translate(Vec3d(0, 0, -3)));
    scene.addNode(node.get());

    Camera cam;
    setupLookAtCamera(cam);
    auto commands = scene.collectRenderCommands(&cam);
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_EQ(commands[0].drawable->name(), u8"g2");
}

TEST(SceneTest, CollectCommandsEffectiveOpacity)
{
    Scene scene;
    auto node = makeTriangleNode(Vec3d(0, 0, -3), nullptr, u8"tri");
    scene.addNode(node.get());
    scene.setOpacity(0.5f);
    node->setOpacity(0.5f);
    node->drawables().front()->setOpacity(0.5f);

    Camera cam;
    setupLookAtCamera(cam);
    auto commands = scene.collectRenderCommands(&cam);
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_NEAR(commands[0].opacity, 0.5f * 0.5f * 0.5f, 1e-5f);
    EXPECT_TRUE(commands[0].isTransparent);
}

TEST(SceneTest, CollectCommandsOpacityIncludesMaterial)
{
    Scene scene;
    auto mat = intrusive_ptr<Material>(new Material());
    mat->setOpacity(0.5f);
    auto node = makeTriangleNode(Vec3d(0, 0, -3), mat.get(), u8"tri");
    node->setOpacity(0.5f);
    scene.addNode(node.get());

    Camera cam;
    setupLookAtCamera(cam);
    auto commands = scene.collectRenderCommands(&cam);
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_NEAR(commands[0].opacity, 0.5f * 0.5f, 1e-5f);
    EXPECT_TRUE(commands[0].isTransparent);
}

TEST(SceneTest, CollectCommandsOpacityMultipliesAlongHierarchy)
{
    Scene scene;
    auto parent = intrusive_ptr<Node>(new Node());
    auto child = makeTriangleNode(Vec3d(0, 0, -3), nullptr, u8"tri");
    parent->addChild(child.get());
    parent->setOpacity(0.5f);
    child->setOpacity(0.5f);
    scene.addNode(parent.get());

    Camera cam;
    setupLookAtCamera(cam);
    auto commands = scene.collectRenderCommands(&cam);
    ASSERT_EQ(commands.size(), 1u);
    EXPECT_NEAR(commands[0].opacity, 0.25f, 1e-5f);
    EXPECT_TRUE(commands[0].isTransparent);
}

// ============ Camera ============

TEST(CameraTest, Defaults)
{
    Camera cam;
    EXPECT_EQ(cam.projectionType(), Camera::ProjectionType::Perspective);
    EXPECT_NEAR(cam.fieldOfView(), 45.0, 1e-9);
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

    OrbitCameraManipulator manip(cam.get());
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

    OrbitCameraManipulator manip(cam.get());
    const double before = manip.orbitRadius();
    manip.zoom(0.5);
    EXPECT_LT(manip.orbitRadius(), before);
    const double distance = (cam->eye() - cam->target()).length();
    EXPECT_NEAR(distance, manip.orbitRadius(), 1e-6);
}

TEST(CameraManipulatorTest, PressOnGeometryKeepsCentre)
{
    Scene scene;
    scene.addNode(makeTriangleNode(Vec3d(0, 0, 0), nullptr, u8"tri").get());

    auto cam = intrusive_ptr<Camera>(new Camera());
    // Aim at a point BEHIND the triangle plane, so the centre-pixel ray hits
    // the triangle surface at a point different from the current target.
    const Vec3d eye(0, 0, 5);
    const Vec3d target(0.3, 0.3, -6.0);
    cam->setViewMatrixAsLookAt(eye, target, Vec3d(0, 1, 0));
    cam->setProjectionMatrixAsPerspective(60.0, 1.0, 0.1, 1000.0);

    OrbitCameraManipulator manip(cam.get(), &scene);
    manip.onResize(vine::window::ResizeEvent{ 800, 600 });

    Vec3d hit;
    ASSERT_TRUE(manip.pickAt(400.0, 300.0, hit));
    EXPECT_NEAR(hit.z, 0.0, 1e-6);  // on the triangle plane (in front of target)

    vine::window::MouseEvent press;
    press.button = vine::window::MouseButton::Left;
    press.x = 400.0;
    press.y = 300.0;
    press.pressed = true;
    manip.onMousePress(press);

    // A rotate press on the model must not re-aim or jump: the orbit centre
    // and the eye both stay put until the user actually drags (the pressed
    // point is then used as the rotation pivot).
    EXPECT_NEAR(manip.orbitCenter().x, target.x, 1e-6);
    EXPECT_NEAR(manip.orbitCenter().y, target.y, 1e-6);
    EXPECT_NEAR(manip.orbitCenter().z, target.z, 1e-6);
    EXPECT_NEAR(cam->eye().x, eye.x, 1e-6);
    EXPECT_NEAR(cam->eye().y, eye.y, 1e-6);
    EXPECT_NEAR(cam->eye().z, eye.z, 1e-6);
}

TEST(CameraManipulatorTest, PressOnEmptyKeepsCentre)
{
    Scene scene;
    scene.addNode(makeTriangleNode(Vec3d(2, 0, -3), nullptr, u8"tri").get());

    auto cam = intrusive_ptr<Camera>(new Camera());
    // Look at the origin; the triangle sits off to the right, so the centre
    // pixel ray misses it.
    const Vec3d eye(0, 0, 5);
    cam->setViewMatrixAsLookAt(eye, Vec3d(0, 0, 0), Vec3d(0, 1, 0));
    cam->setProjectionMatrixAsPerspective(60.0, 1.0, 0.1, 1000.0);

    OrbitCameraManipulator manip(cam.get(), &scene);
    manip.onResize(vine::window::ResizeEvent{ 800, 600 });

    Vec3d hit;
    EXPECT_FALSE(manip.pickAt(400.0, 300.0, hit));

    vine::window::MouseEvent press;
    press.button = vine::window::MouseButton::Left;
    press.x = 400.0;
    press.y = 300.0;
    press.pressed = true;
    manip.onMousePress(press);

    // Pressing empty space must not move the centre either.
    EXPECT_NEAR(manip.orbitCenter().x, 0.0, 1e-6);
    EXPECT_NEAR(manip.orbitCenter().y, 0.0, 1e-6);
    EXPECT_NEAR(manip.orbitCenter().z, 0.0, 1e-6);
    EXPECT_NEAR(cam->eye().x, eye.x, 1e-6);
    EXPECT_NEAR(cam->eye().y, eye.y, 1e-6);
    EXPECT_NEAR(cam->eye().z, eye.z, 1e-6);
}

TEST(CameraManipulatorTest, RotateOnEmptySpaceKeepsModelCentrePinned)
{
    Scene scene;
    scene.addNode(makeTriangleNode(Vec3d(2, 0, -3), nullptr, u8"tri").get());

    auto cam = intrusive_ptr<Camera>(new Camera());
    // Look at the origin; the triangle sits off to the right, so the centre
    // pixel ray misses it (the rotate press happens on empty space).
    const Vec3d eye(0, 0, 5);
    cam->setViewMatrixAsLookAt(eye, Vec3d(0, 0, 0), Vec3d(0, 1, 0));
    cam->setProjectionMatrixAsPerspective(60.0, 1.0, 0.1, 1000.0);

    OrbitCameraManipulator manip(cam.get(), &scene);
    manip.onResize(vine::window::ResizeEvent{ 800, 600 });

    Vec3d hit;
    EXPECT_FALSE(manip.pickAt(400.0, 300.0, hit));

    // Scene (model) centre: the rotate pivot used for an empty-space press.
    const Vec3d centre(2.5, 0.5, -3.0);

    // Project a world point to viewport pixels using the camera.
    const auto project = [&](const Vec3d& p) {
        const double w = 800.0;
        const double h = 600.0;
        const Vec3d fwd = (cam->target() - cam->eye()).normalized();
        const Vec3d right = fwd.cross(cam->up()).normalized();
        const Vec3d up = right.cross(fwd).normalized();
        const Vec3d d = p - cam->eye();
        const double lz = d.dot(fwd);
        const double lx = d.dot(right);
        const double ly = d.dot(up);
        const double tan_half = std::tan(cam->fieldOfView() * vine::math::DEG_TO_RAD * 0.5);
        const double aspect = cam->aspectRatio();
        const double ndc_x = lx / (lz * tan_half * aspect);
        const double ndc_y = ly / (lz * tan_half);
        return std::make_pair((ndc_x * 0.5 + 0.5) * w, (0.5 - ndc_y * 0.5) * h);
    };

    vine::window::MouseEvent press;
    press.button = vine::window::MouseButton::Left;
    press.x = 400.0;
    press.y = 300.0;
    press.pressed = true;
    manip.onMousePress(press);

    const auto before = project(centre);
    const double dist0 = (cam->eye() - centre).length();

    // A long, same-direction rotate drag on the empty area.
    const std::pair<double, double> steps[] = {
        { 410.0, 300.0 }, { 420.0, 301.0 }, { 432.0, 303.0 }, { 446.0, 305.0 },
        { 462.0, 308.0 }, { 480.0, 310.0 }, { 498.0, 312.0 }, { 520.0, 315.0 },
    };
    for (const auto& [x, y] : steps) {
        vine::window::MouseEvent move;
        move.x = x;
        move.y = y;
        move.pressed = true;
        manip.onMouseMove(move);
    }

    // The model centre must stay pinned to its original screen pixel, so the
    // model turns in place instead of swinging out of view.
    const auto after = project(centre);
    EXPECT_NEAR(after.first, before.first, 2.0);
    EXPECT_NEAR(after.second, before.second, 2.0);
    // The eye-to-model distance must not drift, so the model neither shrinks
    // nor enlarges no matter how long the drag continues.
    EXPECT_NEAR((cam->eye() - centre).length(), dist0, 1e-6);
}

TEST(CameraManipulatorTest, SetCenterFromScreenRecentersOnPick)
{
    Scene scene;
    scene.addNode(makeTriangleNode(Vec3d(0, 0, 0), nullptr, u8"tri").get());

    auto cam = intrusive_ptr<Camera>(new Camera());
    // Camera target is behind the triangle; explicit re-centring should move
    // the orbit centre onto the picked surface point while the eye stays.
    const Vec3d eye(0, 0, 5);
    cam->setViewMatrixAsLookAt(eye, Vec3d(0.3, 0.3, -6.0), Vec3d(0, 1, 0));
    cam->setProjectionMatrixAsPerspective(60.0, 1.0, 0.1, 1000.0);

    OrbitCameraManipulator manip(cam.get(), &scene);
    manip.onResize(vine::window::ResizeEvent{ 800, 600 });

    EXPECT_TRUE(manip.setCenterFromScreen(400.0, 300.0));
    // The centre snapped to the surface hit (on the z=0 plane), not the target.
    EXPECT_NEAR(manip.orbitCenter().z, 0.0, 1e-6);
    EXPECT_GT(manip.orbitCenter().x, 0.05);
    EXPECT_NEAR(cam->eye().x, eye.x, 1e-6);
    EXPECT_NEAR(cam->eye().y, eye.y, 1e-6);
    EXPECT_NEAR(cam->eye().z, eye.z, 1e-6);
}

TEST(CameraManipulatorTest, ZoomClampsAtMinimumDistance)
{
    auto cam = intrusive_ptr<Camera>(new Camera());
    cam->setViewMatrixAsLookAt(Vec3d(0, 0, 10), Vec3d(0, 0, 0), Vec3d(0, 1, 0));

    OrbitCameraManipulator manip(cam.get());
    manip.setMinDistance(2.0);
    manip.zoom(1e-6);
    EXPECT_NEAR(manip.orbitRadius(), 2.0, 1e-6);
    EXPECT_NEAR((cam->eye() - cam->target()).length(), manip.orbitRadius(), 1e-6);
}

TEST(CameraManipulatorTest, FitToScreenFramesScene)
{
    Scene scene;
    scene.addNode(makeTriangleNode(Vec3d(0, 0, -3), nullptr, u8"tri").get());
    const Aabbd box = scene.boundingBox();

    auto cam = intrusive_ptr<Camera>(new Camera());
    cam->setViewMatrixAsLookAt(Vec3d(0, 0, 10), Vec3d(0, 0, 0), Vec3d(0, 1, 0));
    cam->setProjectionMatrixAsPerspective(60.0, 1.0, 0.1, 1000.0);

    OrbitCameraManipulator manip(cam.get(), &scene);
    EXPECT_TRUE(manip.fitToScreen());
    EXPECT_NEAR(manip.orbitCenter().x, box.center().x, 1e-6);
    EXPECT_NEAR(manip.orbitCenter().y, box.center().y, 1e-6);
    EXPECT_NEAR(manip.orbitCenter().z, box.center().z, 1e-6);
    const double half_diag = (box.size() * 0.5).length();
    EXPECT_GT(manip.orbitRadius(), half_diag);
    EXPECT_NEAR((cam->eye() - cam->target()).length(), manip.orbitRadius(), 1e-6);
}

TEST(CameraManipulatorTest, HomeRestoresInitialView)
{
    auto cam = intrusive_ptr<Camera>(new Camera());
    cam->setViewMatrixAsLookAt(Vec3d(0, 0, 10), Vec3d(0, 0, 0), Vec3d(0, 1, 0));

    OrbitCameraManipulator manip(cam.get());
    manip.orbit(1.1, -0.4);
    manip.zoom(0.4);
    manip.home();

    EXPECT_NEAR(cam->target().x, 0.0, 1e-6);
    EXPECT_NEAR(cam->target().y, 0.0, 1e-6);
    EXPECT_NEAR(cam->target().z, 0.0, 1e-6);
    EXPECT_NEAR(manip.orbitRadius(), 10.0, 1e-6);
    EXPECT_NEAR(cam->eye().x, 0.0, 1e-6);
    EXPECT_NEAR(cam->eye().y, 0.0, 1e-6);
    EXPECT_NEAR(cam->eye().z, 10.0, 1e-6);
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

TEST(RayIntersectionTest, AllHitsCollectsEveryTriangleSortedByDepth)
{
    // One geometry with two front-facing triangles stacked along -Z: a ray
    // through their centres pierces both, so AllHits returns both hits sorted
    // near to far while Nearest returns only the closer one.
    auto scene = intrusive_ptr<Scene>(new Scene());
    auto node = intrusive_ptr<Node>(new Node());
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = intrusive_ptr<vine::geometry::TriangleMesh>(new vine::geometry::TriangleMesh());
    mesh->addTriangle(vine::math::Vec3f(0.0f, 0.0f, 0.0f),
                      vine::math::Vec3f(1.0f, 0.0f, 0.0f),
                      vine::math::Vec3f(0.0f, 1.0f, 0.0f));
    mesh->addTriangle(vine::math::Vec3f(0.0f, 0.0f, -2.0f),
                      vine::math::Vec3f(1.0f, 0.0f, -2.0f),
                      vine::math::Vec3f(0.0f, 1.0f, -2.0f));
    geom->setShape(mesh.get());
    node->addDrawable(geom.get());
    scene->addNode(node.get());

    Ray ray(Vec3d(0.25, 0.25, 1.0), Vec3d(0, 0, -1));

    // Default AllHits: every triangle hit, sorted by depth.
    const auto all = RayIntersection::intersectSceneAll(ray, scene.get());
    ASSERT_EQ(all.size(), 2u);
    EXPECT_NEAR(all[0].distance, 1.0, 1e-6);
    EXPECT_NEAR(all[0].point.z, 0.0, 1e-6);
    EXPECT_NEAR(all[1].distance, 3.0, 1e-6);
    EXPECT_NEAR(all[1].point.z, -2.0, 1e-6);
    EXPECT_EQ(all[0].geometry.get(), geom.get());
    EXPECT_EQ(all[1].geometry.get(), geom.get());

    // Nearest mode keeps only the closest hit per geometry.
    const auto nearest = RayIntersection::intersectSceneAll(ray, scene.get(),
                                                            RayIntersection::Mode::Nearest);
    ASSERT_EQ(nearest.size(), 1u);
    EXPECT_NEAR(nearest[0].distance, 1.0, 1e-6);

    RayIntersectionResult single = RayIntersection::intersectScene(ray, scene.get());
    EXPECT_TRUE(single.hit);
    EXPECT_NEAR(single.distance, 1.0, 1e-6);
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

    Aabbd box = node->boundingBox();
    // Local box [0,1]x[0,1] translated by +10 on X.
    EXPECT_NEAR(box.min().x, 10.0, 1e-9);
    EXPECT_NEAR(box.max().x, 11.0, 1e-9);
    EXPECT_NEAR(box.max().y, 1.0, 1e-9);
}


TEST(MeshTest, AabbCacheFromPositions)
{
    auto mesh = makeUnitTriangle();
    // Cache starts empty (inverted sentinel) until set or computed.
    EXPECT_FALSE(mesh->aabb().isValid());
    EXPECT_TRUE(mesh->aabb().isEmpty());

    const Aabbf box = mesh->computeAabb();
    EXPECT_TRUE(box.isValid());
    EXPECT_FLOAT_EQ(box.min().x, 0.0f);
    EXPECT_FLOAT_EQ(box.min().y, 0.0f);
    EXPECT_FLOAT_EQ(box.min().z, 0.0f);
    EXPECT_FLOAT_EQ(box.max().x, 1.0f);
    EXPECT_FLOAT_EQ(box.max().y, 1.0f);
    EXPECT_FLOAT_EQ(box.max().z, 0.0f);
    // computeAabb caches the result for aabb().
    EXPECT_TRUE(mesh->aabb() == box);
}

TEST(MeshTest, AabbCacheManualSet)
{
    auto mesh = makeUnitTriangle();
    const Aabbf custom(-2.0f, -3.0f, -4.0f, 5.0f, 6.0f, 7.0f);
    mesh->setAabb(custom);
    EXPECT_TRUE(mesh->aabb() == custom);

    // Geometry edits do not touch the manually cached box.
    mesh->setPositions({ vine::math::Vec3f(9.0f, 9.0f, 9.0f) });
    EXPECT_TRUE(mesh->aabb() == custom);
}

TEST(MeshTest, AttributesSharedOnBase)
{
    auto mesh = makeUnitTriangle();
    EXPECT_EQ(mesh->vertexCount(), 3u);
    EXPECT_EQ(mesh->positions().size(), 3u);
    EXPECT_EQ(mesh->triangleCount(), 1u);

    // Position/normal/texcoord storage lives on the Mesh base.
    mesh->setPositions({ vine::math::Vec3f(0, 0, 0), vine::math::Vec3f(2, 0, 0),
                         vine::math::Vec3f(0, 2, 0) });
    EXPECT_EQ(mesh->vertexCount(), 3u);
    EXPECT_EQ(mesh->positions()[1].x, 2.0f);

    mesh->setNormals({ vine::math::Vec3f(0, 0, 1), vine::math::Vec3f(0, 0, 1),
                      vine::math::Vec3f(0, 0, 1) });
    EXPECT_EQ(mesh->normals().size(), 3u);
}

TEST(GeometryTest, BoundingBoxUsesMeshAabbCache)
{
    auto geom = intrusive_ptr<Geometry>(new Geometry());
    auto mesh = makeUnitTriangle();

    // No cache yet: bounding box falls back to scanning positions.
    geom->setShape(mesh.get());
    Aabbd box = geom->boundingBox();
    EXPECT_TRUE(box.isValid());
    EXPECT_NEAR(box.min().x, 0.0, 1e-9);
    EXPECT_NEAR(box.max().x, 1.0, 1e-9);
    EXPECT_NEAR(box.max().y, 1.0, 1e-9);

    // A computed cache is reused instead of re-scanning.
    mesh->computeAabb();
    box = geom->boundingBox();
    EXPECT_TRUE(box.isValid());
    EXPECT_NEAR(box.max().x, 1.0, 1e-6);

    // A manually set cache wins over the geometry.
    mesh->setAabb(vine::math::Aabbf(-5.0f, -5.0f, -5.0f, 5.0f, 5.0f, 5.0f));
    box = geom->boundingBox();
    EXPECT_NEAR(box.min().x, -5.0, 1e-6);
    EXPECT_NEAR(box.max().z, 5.0, 1e-6);
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
    int viewport_sets = 0;
    const Camera* last_camera = nullptr;
    int last_viewport[4] = { 0, 0, 0, 0 };

    bool initialize() override { ok = true; return true; }
    void shutdown() override { ok = false; }
    void beginFrame() override { ++begin_calls; }
    void endFrame() override { ++end_calls; }
    void executePass(const RenderPass*, const std::vector<RenderCommand>&) override {}
    void setRenderTarget(RenderTarget*) override {}
    void render(const std::vector<RenderCommand>&, const Camera* camera) override
    {
        ++render_calls;
        last_camera = camera;
    }
    void setViewport(int x, int y, int width, int height) override
    {
        ++viewport_sets;
        last_viewport[0] = x;
        last_viewport[1] = y;
        last_viewport[2] = width;
        last_viewport[3] = height;
    }
    void clear(const Color&, bool) override { ++clear_calls; }
    void swapBuffers() override { ++swap_calls; }
};

TEST(RenderEngineTest, InitializeCallsBackend)
{
    auto backend = intrusive_ptr<MockBackend>(new MockBackend());
    auto engine  = intrusive_ptr<RenderEngine>(new RenderEngine());
    engine->setBackend(backend);

    EXPECT_TRUE(engine->initialize());
    EXPECT_TRUE(backend->ok);
    EXPECT_EQ(engine->backend(), backend.get());
}

TEST(RenderEngineTest, FrameRunsPipeline)
{
    auto backend = intrusive_ptr<MockBackend>(new MockBackend());
    auto engine  = intrusive_ptr<RenderEngine>(new RenderEngine());
    engine->setBackend(backend);
    engine->initialize();

    engine->frame();

    EXPECT_EQ(backend->begin_calls, 1);
    EXPECT_EQ(backend->end_calls, 1);
    EXPECT_EQ(backend->swap_calls, 1);
    EXPECT_GE(backend->clear_calls, 1);
}

TEST(RenderEngineTest, FrameBeforeInitializeIsNoOp)
{
    auto backend = intrusive_ptr<MockBackend>(new MockBackend());
    auto engine  = intrusive_ptr<RenderEngine>(new RenderEngine());
    engine->setBackend(backend);

    // Not initialized: frame should do nothing.
    engine->frame();
    EXPECT_EQ(backend->begin_calls, 0);
    EXPECT_EQ(backend->swap_calls, 0);
}

TEST(RenderEngineTest, DefaultObjectsCreated)
{
    auto backend = intrusive_ptr<MockBackend>(new MockBackend());
    auto engine  = intrusive_ptr<RenderEngine>(new RenderEngine());
    engine->setBackend(backend);

    EXPECT_NE(engine->scene(), nullptr);
    EXPECT_NE(engine->camera(), nullptr);
    EXPECT_NE(engine->mainPass(), nullptr);
    EXPECT_EQ(engine->mainPass()->camera(), engine->camera());
}

TEST(RenderEngineTest, SetSceneAndCamera)
{
    auto backend = intrusive_ptr<MockBackend>(new MockBackend());
    auto engine  = intrusive_ptr<RenderEngine>(new RenderEngine());
    engine->setBackend(backend);

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
    auto backend = intrusive_ptr<MockBackend>(new MockBackend());
    auto engine  = intrusive_ptr<RenderEngine>(new RenderEngine());
    engine->setBackend(backend);
    engine->initialize();
    EXPECT_TRUE(backend->ok);

    engine->shutdown();
    EXPECT_FALSE(backend->ok);
}

// ============ Overlay ============

namespace
{

/**
 * @brief Overlay that records the order in which update() is called.
 */
class SeqOverlay : public Overlay {
  public:
    explicit SeqOverlay(int* next)
      : next_(next)
    {
    }

    int seq = -1;

    void update(double dt) override
    {
        (void)dt;
        seq = (*next_)++;
    }

  private:
    int* next_;
};

}  // namespace

TEST(OverlayTest, EngineDrawsVisibleOverlaysSortedByZOrder)
{
    auto backend = intrusive_ptr<MockBackend>(new MockBackend());
    auto engine  = intrusive_ptr<RenderEngine>(new RenderEngine());
    engine->setBackend(backend);
    engine->initialize();

    auto overlay_cam = intrusive_ptr<Camera>(new Camera());
    overlay_cam->setViewMatrixAsLookAt(Vec3d(0, 0, 5), Vec3d(0, 0, 0), Vec3d(0, 1, 0));
    auto scene = intrusive_ptr<Scene>(new Scene());

    int next = 0;
    auto z10 = intrusive_ptr<SeqOverlay>(new SeqOverlay(&next));
    z10->setContent(scene.get());
    z10->pass()->setCamera(overlay_cam.get());
    z10->setZOrder(10);

    auto z5 = intrusive_ptr<SeqOverlay>(new SeqOverlay(&next));
    z5->setContent(scene.get());
    z5->pass()->setCamera(overlay_cam.get());
    z5->setZOrder(5);

    auto hidden = intrusive_ptr<SeqOverlay>(new SeqOverlay(&next));
    hidden->setContent(scene.get());
    hidden->pass()->setCamera(overlay_cam.get());
    hidden->setZOrder(0);
    hidden->setVisible(false);

    engine->addOverlay(z10);
    engine->addOverlay(hidden);
    engine->addOverlay(z5);

    const int before = backend->render_calls;
    engine->frame();
    // Main pass + the two visible overlays.
    EXPECT_EQ(backend->render_calls - before, 3);
    // update() ran in ascending zOrder; the hidden overlay was skipped.
    EXPECT_EQ(z5->seq, 0);
    EXPECT_EQ(z10->seq, 1);
    EXPECT_EQ(hidden->seq, -1);
}

TEST(OverlayTest, PassViewportAndClearPolicy)
{
    auto backend = intrusive_ptr<MockBackend>(new MockBackend());
    auto engine  = intrusive_ptr<RenderEngine>(new RenderEngine());
    engine->setBackend(backend);
    engine->initialize();

    auto overlay_cam = intrusive_ptr<Camera>(new Camera());
    overlay_cam->setViewMatrixAsLookAt(Vec3d(0, 0, 5), Vec3d(0, 0, 0), Vec3d(0, 1, 0));
    auto scene = intrusive_ptr<Scene>(new Scene());

    auto overlay = intrusive_ptr<Overlay>(new Overlay());
    overlay->setContent(scene.get());
    overlay->pass()->setCamera(overlay_cam.get());
    // An overlay never clears the surface it draws over.
    EXPECT_FALSE(overlay->pass()->clearEnabled());
    overlay->pass()->setViewport(4, 5, 96, 96);
    engine->addOverlay(overlay);

    const int before = backend->clear_calls;
    engine->frame();
    // The overlay sub-viewport was forwarded to the backend.
    EXPECT_EQ(backend->viewport_sets, 1);
    EXPECT_EQ(backend->last_viewport[0], 4);
    EXPECT_EQ(backend->last_viewport[1], 5);
    EXPECT_EQ(backend->last_viewport[2], 96);
    EXPECT_EQ(backend->last_viewport[3], 96);
    // Only the main pass cleared (overlay clear is disabled), so exactly one
    // clear happens for this frame.
    EXPECT_EQ(backend->clear_calls - before, 1);
}

TEST(OverlayTest, OrientationMirrorFacesSourceKeepingFraming)
{
    auto src = intrusive_ptr<Camera>(new Camera());
    src->setViewMatrixAsLookAt(Vec3d(3, 4, 5), Vec3d(0, 0, 0), Vec3d(0, 1, 0));

    auto cam = intrusive_ptr<Camera>(new Camera());
    cam->setViewMatrixAsLookAt(Vec3d(0, 0, 4), Vec3d(0, 0, 0), Vec3d(0, 1, 0));

    auto overlay = intrusive_ptr<Overlay>(new Overlay());
    overlay->pass()->setCamera(cam.get());
    overlay->setSourceCamera(src.get());
    overlay->setMirrorMode(Overlay::MirrorMode::Orientation);
    overlay->update(0.0);

    const Vec3d sf = (src->target() - src->eye()).normalized();
    const Vec3d of = (cam->target() - cam->eye()).normalized();
    EXPECT_NEAR(of.x, sf.x, 1e-6);
    EXPECT_NEAR(of.y, sf.y, 1e-6);
    EXPECT_NEAR(of.z, sf.z, 1e-6);
    // The overlay kept its own framing distance.
    EXPECT_NEAR((cam->target() - cam->eye()).length(), 4.0, 1e-6);
}

// ============ AxisGizmo ============

TEST(AxisGizmoTest, BuildsThreeColouredSticks)
{
    auto gizmo = intrusive_ptr<AxisGizmo>(new AxisGizmo());
    ASSERT_NE(gizmo->content(), nullptr);
    ASSERT_EQ(gizmo->content()->nodes().size(), 3u);
    for (const auto& node : gizmo->content()->nodes()) {
        ASSERT_EQ(node->drawables().size(), 1u);
    }
    EXPECT_NE(gizmo->pass()->camera(), nullptr);
    EXPECT_EQ(gizmo->mirrorMode(), Overlay::MirrorMode::Orientation);
    // Overlays never clear the surface they draw over.
    EXPECT_FALSE(gizmo->pass()->clearEnabled());
}

TEST(AxisGizmoTest, ViewportPlacedBottomLeft)
{
    auto gizmo = intrusive_ptr<AxisGizmo>(new AxisGizmo());
    gizmo->onSurfaceResized(800, 600);

    int x = 0, y = 0, w = 0, h = 0;
    gizmo->pass()->getViewport(x, y, w, h);
    EXPECT_EQ(w, 96);
    EXPECT_EQ(h, 96);
    EXPECT_EQ(x, 16);
    EXPECT_EQ(y, 600 - 16 - 96);
}

TEST(AxisGizmoTest, OrientationMirrorTracksSource)
{
    auto gizmo = intrusive_ptr<AxisGizmo>(new AxisGizmo());
    auto src = intrusive_ptr<Camera>(new Camera());
    src->setViewMatrixAsLookAt(Vec3d(2, 3, 6), Vec3d(0, 0, 0), Vec3d(0, 1, 0));
    gizmo->setSourceCamera(src.get());
    gizmo->update(0.0);

    Camera* cam = gizmo->pass()->camera();
    const Vec3d sf = (src->target() - src->eye()).normalized();
    const Vec3d cf = (cam->target() - cam->eye()).normalized();
    EXPECT_NEAR(cf.x, sf.x, 1e-6);
    EXPECT_NEAR(cf.y, sf.y, 1e-6);
    EXPECT_NEAR(cf.z, sf.z, 1e-6);
}

// ============ RenderBackendRegistry ============

namespace
{

/**
 * @brief A named factory producing a MockBackend for registry tests.
 */
class MockBackendFactory : public RenderBackendFactory {
  public:
    MockBackendFactory() = default;

    explicit MockBackendFactory(const vine::String& name)
      : name_(name)
    {
    }

    RenderBackendInfo info() const override
    {
        const auto name = name_.empty() ? u8"mock3" : name_;
        return RenderBackendInfo{ name, name, u8"mock backend", u8"1.0.0", u8"test",
                                  RenderApi::Vulkan | RenderApi::OpenGL3 };
    }

    vine::intrusive_ptr<RenderBackend> create(Scene*, Camera*) override
    {
        return vine::intrusive_ptr<RenderBackend>(new MockBackend());
    }

  private:
    vine::String name_;
};

}  // namespace

TEST(RenderBackendRegistryTest, RegisterAndCreateByName)
{
    auto& registry = RenderBackendRegistry::instance();
    static MockBackendFactory factory(u8"mock");

    registry.registerFactory(&factory);
    EXPECT_TRUE(registry.has(u8"mock"));

    auto backend = registry.create(u8"mock", nullptr, nullptr);
    ASSERT_NE(backend, nullptr);

    // Unknown names return null.
    EXPECT_EQ(registry.create(u8"nope", nullptr, nullptr), nullptr);
    EXPECT_FALSE(registry.has(u8"nope"));
}

TEST(RenderBackendRegistryTest, EnumerateNames)
{
    auto& registry = RenderBackendRegistry::instance();
    static MockBackendFactory factory(u8"mock2");
    registry.registerFactory(&factory);

    const auto names = registry.names();
    EXPECT_FALSE(names.empty());
    const bool found = std::find(names.begin(), names.end(), u8"mock2") != names.end();
    EXPECT_TRUE(found);
}

TEST(RenderBackendRegistryTest, EnumerateEntries)
{
    auto& registry = RenderBackendRegistry::instance();
    static MockBackendFactory factory(u8"mock4");
    registry.registerFactory(&factory);

    const auto entries = registry.entries();
    EXPECT_FALSE(entries.empty());

    // Iterate and query each registered backend by name/metadata.
    bool found = false;
    for (const auto& entry : entries) {
        if (entry.info.name == u8"mock4") {
            found = true;
            EXPECT_NE(entry.factory, nullptr);
            EXPECT_EQ(entry.factory->name(), u8"mock4");
            EXPECT_FALSE(entry.info.description.empty());
            EXPECT_TRUE(vine::testFlag(entry.info.api_flags, RenderApi::Vulkan));
            EXPECT_TRUE(vine::testFlag(entry.info.api_flags, RenderApi::OpenGL3));
            EXPECT_EQ(renderApiToString(entry.info.api_flags), u8"vulkan | opengl3");
        }
    }
    EXPECT_TRUE(found);
}

TEST(RenderBackendRegistryTest, RegistrarSelfRegisters)
{
    auto& registry = RenderBackendRegistry::instance();
    // Static: the registrar's embedded factory must outlive the process, as
    // the registry keeps a raw pointer to it.
    static RenderBackendRegistry::Registrar<MockBackendFactory> registrar;
    // The registrar's constructor self-registers; the factory is embedded.
    EXPECT_TRUE(registry.has(u8"mock3"));
}

