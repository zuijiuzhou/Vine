#include "AppShellUi.hpp"

#include <cstdlib>
#include <cmath>
#include <utility>

#include <QTimer>

#include <vine/Colorf.hpp>
#include <vine/graphics/AxisGizmo.hpp>
#include <vine/graphics/Camera.hpp>
#include <vine/graphics/Geometry.hpp>
#include <vine/graphics/MatrixTransform.hpp>
#include <vine/graphics/Material.hpp>
#include <vine/graphics/Node.hpp>
#include <vine/graphics/RenderEngine.hpp>
#include <vine/graphics/ShaderPreset.hpp>
#include <vine/graphics/RenderPass.hpp>
#include <vine/graphics/RenderPipelineBuilder.hpp>
#include <vine/graphics/RenderTarget.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/graphics/ScreenPass.hpp>
#include <vine/graphics/ShaderProgram.hpp>
#include <vine/graphics/StateNode.hpp>
#include <vine/graphics/Light.hpp>
#include <vine/math/Vector3.hpp>
#include <vine/geometry/IndexedTriangleMesh.hpp>
#include <vine/math/Transform3.hpp>
#include <vine/intrusive_ptr.hpp>

#include <vine/appfw/gui/ConsolePanel.hpp>
#include <vine/appfw/gui/DockPanel.hpp>
#include <vine/appfw/gui/DockPanelManager.hpp>
#include <vine/appfw/gui/GuiApplication.hpp>
#include <vine/appfw/gui/Icon.hpp>
#include <vine/appfw/gui/MainWindow.hpp>
#include <vine/appfw/gui/RenderControl.hpp>
#include <vine/appfw/gui/RibbonBar.hpp>
#include <vine/appfw/gui/RibbonButton.hpp>
#include <vine/appfw/gui/RibbonGroup.hpp>
#include <vine/appfw/gui/RibbonTab.hpp>

V_APPFW_NS_BEGIN

namespace
{

/**
 * @brief Adds a large Ribbon button executing the given command by name.
 *
 * @param group Target group.
 * @param text Button label.
 * @param icon Icon resource path.
 * @param command Registered command name.
 * @return The created button (owned by the group).
 */
gui::RibbonButton* addCommandButton(gui::RibbonGroup* group, const String& text, const String& icon, const String& command)
{
    auto* button = new gui::RibbonButton();
    button->setText(text);
    button->setIcon(gui::Icon(icon));
    button->setButtonSize(gui::RibbonItemSize::Large);
    button->setCommand(command);
    group->addButton(button);
    return button;
}

/**
 * @brief Builds a flat-shaded axis-aligned box node centred at a position.
 *
 * The box is authored as an IndexedTriangleMesh (six faces, twelve triangles,
 * per-face vertex normals) so the vsg backend renders it directly.
 *
 * @param scene   Scene receiving the node.
 * @param diffuse Flat diffuse colour of the box.
 * @param name    Node and geometry name.
 * @param centre  World-space centre of the box.
 * @param half    Half extents along X / Y / Z.
 * @return The created node (kept alive by the scene).
 */
vine::intrusive_ptr<vine::graphics::MatrixTransform> addBox(vine::graphics::Scene* scene,
                                                            const vine::Colorf& diffuse,
                                                            const vine::String& name,
                                                            const vine::math::Vec3d& centre,
                                                            const vine::math::Vec3d& half)
{
    using vine::math::Vec3f;

    vine::geometry::Vec3fArray positions;
    vine::geometry::Vec3fArray normals;
    vine::geometry::UInt32Array indices;

    // Build each face with a consistent outward (counter-clockwise, seen from
    // the outside) winding. For a face normal n, pick in-plane unit vectors
    // u and v with u x v = n; then the four corners ordered as
    //   -u-v, u-v, u+v, -u+v
    // are guaranteed to wind CCW around the outward normal, so the winding is
    // consistent across all faces by construction.
    const int face_table[6][3] = {
        // { normal axis, u axis, v axis } with implicit positive unit axes.
        { 0, 1, 2 },  // +X: u=Y, v=Z  -> YxZ = +X
        { 1, 2, 0 },  // +Y: u=Z, v=X  -> ZxX = +Y
        { 2, 0, 1 },  // +Z: u=X, v=Y  -> XxY = +Z
        { 0, 2, 1 },  // -X: u=Z, v=Y  -> ZxY = -X
        { 1, 0, 2 },  // -Y: u=X, v=Z  -> XxZ = -Y
        { 2, 1, 0 },  // -Z: u=Y, v=X  -> YxX = -Z
    };

    auto axisVector = [](int axis) {
        return Vec3f(axis == 0 ? 1.0f : 0.0f,
                     axis == 1 ? 1.0f : 0.0f,
                     axis == 2 ? 1.0f : 0.0f);
    };

    for (int f = 0; f < 6; ++f) {
        const int  n_axis = face_table[f][0];
        const int  u_axis = face_table[f][1];
        const int  v_axis = face_table[f][2];
        const bool neg    = f >= 3;  // the last three rows are the -axis faces

        const Vec3f normal = neg ? -axisVector(n_axis) : axisVector(n_axis);
        const Vec3f u = axisVector(u_axis);
        const Vec3f v = axisVector(v_axis);

        const std::uint32_t base = static_cast<std::uint32_t>(positions.size());
        const float hs[3]       = { static_cast<float>(half.x), static_cast<float>(half.y), static_cast<float>(half.z) };
        const float cu[4]       = { -1.0f, 1.0f, 1.0f, -1.0f };
        const float cv[4]       = { -1.0f, -1.0f, 1.0f, 1.0f };
        for (int k = 0; k < 4; ++k) {
            const Vec3f corner = normal * hs[n_axis] + u * (cu[k] * hs[u_axis]) + v * (cv[k] * hs[v_axis]);
            positions.push_back(corner);
            normals.push_back(normal);
        }
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }

    auto mesh = vine::intrusive_ptr<vine::geometry::IndexedTriangleMesh>(
        new vine::geometry::IndexedTriangleMesh());
    mesh->setPositions(std::move(positions));
    mesh->setNormals(std::move(normals));
    mesh->setIndices(std::move(indices));
    // Pre-compute the cached AABB so drawable/scene bounds reuse it.
    mesh->computeAabb();

    auto geometry = vine::intrusive_ptr<vine::graphics::Geometry>(new vine::graphics::Geometry());
    geometry->setName(name);
    geometry->setShape(mesh);

    auto material = vine::intrusive_ptr<vine::graphics::Material>(new vine::graphics::Material());
    material->setDiffuse(diffuse);
    geometry->setMaterial(material);

    // Place the box with a MatrixTransform: the transform node carries the
    // position and owns the leaf geometry.
    auto node = vine::intrusive_ptr<vine::graphics::MatrixTransform>(
        new vine::graphics::MatrixTransform());
    node->setName(name);
    node->setMatrix(vine::math::translate(centre));
    node->addChild(geometry);

    scene->addNode(node);
    return node;
}

/**
 * @brief Builds the default demo scene: a single-pass feature showcase.
 *
 * Exercises, in one window view, every recently landed graphics slice:
 *  - ground + lit box: MatrixTransform + Material (default Phong path)
 *  - StateNode{ PolygonMode::Line } box         -> per-subtree polygon state
 *  - StateNode{ CullMode::Back } box            -> per-subtree culling state
 *  - StateNode{ Topology::Points } + cyan user program point cloud
 *  - Geometry::setProgram magenta box (runtime-compiled GLSL, official vsg
 *    "pc" projection/modelView contract)
 *  - nested MatrixTransform (world-matrix chain)
 *  - StateNode blend + translucent material box
 *
 * @param scene Scene to receive the content.
 */
void addDemoCubes(vine::graphics::Scene* scene)
{
    using vine::intrusive_ptr;
    using vine::graphics::BlendState;
    using vine::graphics::Geometry;
    using vine::graphics::MatrixTransform;
    using vine::graphics::ShaderProgram;
    using vine::graphics::ShaderStage;
    using vine::graphics::ShaderStageType;
    using vine::graphics::StateNode;
    using vine::math::Vec3d;
    using vine::math::Vec3f;

    // Ground (wide, slightly below origin) + one plain lit box: the default
    // material / Phong path.
    addBox(scene, vine::Colorf(0.45f, 0.47f, 0.52f, 1.0f), u8"ground",
           Vec3d(0.0, -0.05, 0.0), Vec3d(3.0, 0.05, 3.0));
    addBox(scene, vine::Colorf(0.30f, 0.62f, 0.36f, 1.0f), u8"lit_box",
           Vec3d(0.0, 0.4, 0.0), Vec3d(0.5, 0.4, 0.5));

    // --- StateNode{ PolygonMode::Line }: wireframe box ----------------------
    {
        auto state = intrusive_ptr<StateNode>(new StateNode());
        state->setPolygonMode(vine::graphics::PolygonMode::Line);
        auto box = addBox(scene, vine::Colorf(1.0f, 0.68f, 0.12f, 1.0f), u8"wire_box",
                          Vec3d(-2.3, 0.35, 0.6), Vec3d(0.35, 0.35, 0.35));
        scene->removeNode(box.get());
        state->addChild(box);
        scene->addNode(state);
    }

    // --- StateNode{ CullMode::Back }: single-sided box ----------------------
    {
        auto state = intrusive_ptr<StateNode>(new StateNode());
        state->setCullMode(vine::graphics::CullMode::Back);
        auto box = addBox(scene, vine::Colorf(0.20f, 0.75f, 0.85f, 1.0f), u8"culled_box",
                          Vec3d(-1.6, 0.4, -0.8), Vec3d(0.4, 0.4, 0.4));
        scene->removeNode(box.get());
        state->addChild(box);
        scene->addNode(state);
    }

    // --- Custom magenta program on a box (Geometry::setProgram) -------------
    {
        auto program = intrusive_ptr<ShaderProgram>(new ShaderProgram());
        program->setName(u8"demo_magenta");
        ShaderStage vs;
        vs.type = ShaderStageType::Vertex;
        vs.source = u8"#version 450\n"
                    u8"layout(push_constant) uniform PushConstants { mat4 projection; mat4 modelView; } pc;\n"
                    u8"layout(location = 0) in vec3 vsg_Vertex;\n"
                    u8"void main(){ gl_Position = pc.projection * pc.modelView * vec4(vsg_Vertex, 1.0); }\n";
        program->addStage(vs);
        ShaderStage fs;
        fs.type = ShaderStageType::Fragment;
        fs.source = u8"#version 450\n"
                    u8"layout(location = 0) out vec4 outColor;\n"
                    u8"void main(){ outColor = vec4(0.9, 0.1, 0.85, 1.0); }\n";
        program->addStage(fs);

        auto box = addBox(scene, vine::Colorf(1.0f, 1.0f, 1.0f, 1.0f), u8"custom_program",
                          Vec3d(0.0, 0.85, 0.0), Vec3d(0.42, 0.45, 0.42));
        auto* geometry = dynamic_cast<Geometry*>(box->children().front().get());
        if (geometry != nullptr) {
            geometry->setProgram(program);
        }
    }

    // --- Point cloud: StateNode{ Topology::Points } + cyan program ----------
    {
        auto program = intrusive_ptr<ShaderProgram>(new ShaderProgram());
        program->setName(u8"demo_cyan_points");
        ShaderStage vs;
        vs.type = ShaderStageType::Vertex;
        vs.source = u8"#version 450\n"
                    u8"layout(push_constant) uniform PushConstants { mat4 projection; mat4 modelView; } pc;\n"
                    u8"layout(location = 0) in vec3 vsg_Vertex;\n"
                    u8"void main(){ gl_Position = pc.projection * pc.modelView * vec4(vsg_Vertex, 1.0); }\n";
        program->addStage(vs);
        ShaderStage fs;
        fs.type = ShaderStageType::Fragment;
        fs.source = u8"#version 450\n"
                    u8"layout(location = 0) out vec4 outColor;\n"
                    u8"void main(){ outColor = vec4(0.1, 0.9, 0.95, 1.0); }\n";
        program->addStage(fs);

        auto cloud = intrusive_ptr<Geometry>(new Geometry());
        cloud->setName(u8"point_cloud");
        vine::geometry::Vec3fArray points;
        for (int ring = 0; ring < 4; ++ring) {
            const float r = 0.35f + 0.12f * static_cast<float>(ring);
            const float y = -0.25f + 0.17f * static_cast<float>(ring);
            for (int k = 0; k < 36; ++k) {
                const float a = 6.2831853f * static_cast<float>(k) / 36.0f;
                points.emplace_back(r * std::cos(a), y, r * std::sin(a));
            }
        }
        cloud->setPositions(points);
        cloud->setProgram(program);

        auto state = intrusive_ptr<StateNode>(new StateNode());
        state->setTopology(vine::graphics::Topology::Points);
        auto mt = intrusive_ptr<MatrixTransform>(new MatrixTransform());
        mt->setName(u8"point_cloud_node");
        mt->setMatrix(vine::math::translate(Vec3d(-2.3, 0.75, -1.2)));
        mt->addChild(cloud);
        state->addChild(mt);
        scene->addNode(state);
    }

    // --- Nested MatrixTransform: world-matrix chain --------------------------
    {
        auto inner = addBox(scene, vine::Colorf(0.95f, 0.5f, 0.1f, 1.0f), u8"nested_inner",
                            Vec3d(0.0, 0.0, 0.0), Vec3d(0.28, 0.28, 0.28));
        inner->setMatrix(vine::math::translate(Vec3d(0.0, 0.32, 0.0)));
        scene->removeNode(inner.get());
        auto outer = intrusive_ptr<MatrixTransform>(new MatrixTransform());
        outer->setName(u8"nested_outer");
        outer->setMatrix(vine::math::translate(Vec3d(1.5, 0.0, 0.9)));
        outer->addChild(inner);
        scene->addNode(outer);
    }

    // --- StateNode blend + translucent box -----------------------------------
    {
        auto state = intrusive_ptr<StateNode>(new StateNode());
        BlendState blend;
        blend.enabled = true;
        blend.src = vine::graphics::BlendFactor::SrcAlpha;
        blend.dst = vine::graphics::BlendFactor::OneMinusSrcAlpha;
        state->setBlend(blend);

        auto box = addBox(scene, vine::Colorf(1.0f, 0.25f, 0.25f, 1.0f), u8"translucent",
                          Vec3d(0.9, 0.7, -1.1), Vec3d(0.55, 0.55, 0.55));
        auto* geometry = dynamic_cast<Geometry*>(box->children().front().get());
        if (geometry != nullptr) {
            geometry->setOpacity(0.5f);
        }
        scene->removeNode(box.get());
        state->addChild(box);
        scene->addNode(state);
    }
}

} // namespace

/**
 * @brief Adds a world-orientation axis gizmo in the bottom-left corner.
 *
 * The gizmo is a self-contained HUD pass (AxisGizmo derives RenderPass): it
 * mirrors the main camera's orientation and is drawn by the render backend
 * into a small sub-viewport, registered on top of the (default) window pass.
 *
 * @param render_control Render view whose engine receives the gizmo pass.
 */
void addAxisGizmo(gui::RenderControl* render_control)
{
    auto gizmo = vine::intrusive_ptr<vine::graphics::AxisGizmo>(new vine::graphics::AxisGizmo());
    gizmo->setSourceCamera(render_control->engine()->masterCamera());
    // Qt reports logical sizes; the gizmo is positioned in device pixels.
    gizmo->setPixelRatio(render_control->devicePixelRatio());
    // A top / HUD pass is just an ordinary pass: register it above the window
    // pass (order 0) that RenderControl provisions for the main view.
    render_control->engine()->addPass(gizmo, 10);
}

/**
 * @brief Lights the demo scene (scene-level v4a rig) and frames it.
 *
 * Runs unconditionally so the default demo (ground + box stack) is lit by an
 * ambient fill plus a sun-like directional light, viewed from an elevated 3/4
 * angle. The off-screen/PiP validation renders the same engine scene and
 * camera, so the PiP matches the main view.
 *
 * Shadow mapping is deferred until the custom-shader / multi-pass slice is
 * mature: the demo sun never sets castShadow() yet, so no half-built shadow
 * path runs.
 *
 * @param render_control Render view whose engine receives the lights.
 */
void addDemoLighting(gui::RenderControl* render_control)
{
    auto* engine = render_control->engine();
    if (engine == nullptr) {
        return;
    }
    auto* scene = engine->scene();
    auto ambient = vine::graphics::Light::createAmbient();
    ambient->setName(u8"scene_ambient");
    ambient->setIntensity(0.25f);
    scene->addLight(ambient);

    // A sun-like directional light from behind-left-top (warm side-light).
    auto sun = vine::graphics::Light::createDirectional(vine::math::Vec3d(0.35, -0.75, 0.35));
    sun->setName(u8"scene_sun");
    sun->setIntensity(1.0f);
    scene->addLight(sun);

    // Dev switch: VINE_VSG_EXTRA_SUNS adds two more directional lights so the
    // scene exercises the deferred light pass' full three-light capacity.
    if (std::getenv("VINE_VSG_EXTRA_SUNS") != nullptr) {
        auto sun2 = vine::graphics::Light::createDirectional(vine::math::Vec3d(-0.9, -0.5, -0.3));
        sun2->setName(u8"scene_sun2");
        sun2->setColor(vine::Colorf(0.6f, 0.8f, 1.0f, 1.0f));
        sun2->setIntensity(0.7f);
        scene->addLight(sun2);

        auto sun3 = vine::graphics::Light::createDirectional(vine::math::Vec3d(-0.2, -0.3, 0.95));
        sun3->setName(u8"scene_sun3");
        sun3->setColor(vine::Colorf(1.0f, 0.55f, 0.35f, 1.0f));
        sun3->setIntensity(0.5f);
        scene->addLight(sun3);
    }

    // Raise the camera to an elevated 3/4 view of the stack. Runs before
    // RenderControl::init() wires the orbit manipulator, so the manipulator
    // home syncs to this vantage.
    auto* camera = engine->masterCamera();
    if (camera != nullptr) {
        camera->setViewMatrixAsLookAt(vine::math::Vec3d(6.5, 5.0, 6.5), // eye: front-right, elevated
                                      vine::math::Vec3d(0.0, 0.6, 0.0), // target: mid-stack
                                      vine::math::Vec3d(0.0, 1.0, 0.0));
    }
}

/**
 * @brief TEMP GPU-validation hook for the v3 render-to-texture chain.
 *
 * When the environment variable VINE_VSG_OFFSCREEN is set, registers:
 *   1. an order < 0 pass that renders the engine scene into an off-screen
 *      RGBA8 + depth RenderTarget each frame (publishing it as "SceneColor"),
 *      exercising the vsg render-to-texture path, and
 *   2. an order > 0 ScreenPass that samples "SceneColor" and composites it
 *      into a picture-in-picture sub-viewport of the window, so the off-screen
 *      result is actually visible (render-to-texture -> sample -> present).
 *
 * Run it to check for Vulkan validation errors and the log lines
 * "[VsgRenderer] EXPERIMENTAL off-screen target ... attached" and
 * "[VsgRenderer] EXPERIMENTAL screen PiP ... attached".
 *
 * @param render_control Render view whose engine receives the passes.
 */
void addOffscreenValidationPass(gui::RenderControl* render_control)
{
    const char* enabled = std::getenv("VINE_VSG_OFFSCREEN");
    if (enabled == nullptr || enabled[0] == '\0') {
        return;
    }
    auto* engine = render_control->engine();
    if (engine == nullptr) {
        return;
    }

    // Recipe #1 (RenderPipelineBuilder): compose the "off-screen RT -> PiP
    // ScreenPass" wiring instead of hand-building the two passes.
    const double dpr    = render_control->devicePixelRatio();
    const int    margin = static_cast<int>(10.0 * dpr);
    const int    pip_w  = static_cast<int>(320.0 * dpr);
    const int    pip_h  = static_cast<int>(180.0 * dpr);

    // Bottom-right anchoring against the (current or default) surface size.
    const auto anchorRect = [render_control, margin](int w, int h, int& out_x, int& out_y) {
        auto* engine_ptr = render_control->engine();
        int sw = (engine_ptr != nullptr) ? engine_ptr->frameContext().surface_width : 0;
        int sh = (engine_ptr != nullptr) ? engine_ptr->frameContext().surface_height : 0;
        if (sw <= 0 || sh <= 0) {
            // Surface not realized yet: anchor against the default viewport so
            // the PiP never briefly covers the whole surface.
            sw = 1280;
            sh = 720;
        }
        if (w > sw / 2) {
            w = sw / 2;
            h = static_cast<int>(w * 9 / 16);
        }
        if (h > sh / 2) {
            h = sh / 2;
            w = static_cast<int>(h * 16 / 9);
        }
        out_x = sw - w - margin;
        out_y = sh - h - margin;
    };
    int px = 0, py = 0;
    anchorRect(pip_w, pip_h, px, py);

    vine::graphics::RenderPipelineBuilder builder(engine);
    auto* screen = builder.addOffscreenToScreen(
        u8"SceneColor", 640, 360,
        vine::graphics::RenderTarget::ColorFormat::RGBA8,
        vine::graphics::RenderTarget::DepthFormat::D24,
        px, py, pip_w, pip_h);
    if (screen == nullptr) {
        return;
    }

    // Re-anchor once the backend surface is realized and sized.
    QTimer::singleShot(600, [render_control, screen, pip_w, pip_h, margin] {
        auto* engine_ptr = render_control->engine();
        int sw = (engine_ptr != nullptr) ? engine_ptr->frameContext().surface_width : 0;
        int sh = (engine_ptr != nullptr) ? engine_ptr->frameContext().surface_height : 0;
        if (sw <= 0 || sh <= 0) {
            sw = 1280;
            sh = 720;
        }
        int w = pip_w;
        int h = pip_h;
        if (w > sw / 2) {
            w = sw / 2;
            h = static_cast<int>(w * 9 / 16);
        }
        if (h > sh / 2) {
            h = sh / 2;
            w = static_cast<int>(h * 16 / 9);
        }
        screen->setViewport(sw - w - margin, sh - h - margin, w, h);
    });
}

void buildAppShellRibbon(gui::MainWindow* wnd)
{
    auto* bar = wnd->ribbonBar();

    auto* plugin_tab = new gui::RibbonTab();
    plugin_tab->setTitle(u8"插件");
    bar->addTab(plugin_tab);
    auto* plugin_group = new gui::RibbonGroup();
    plugin_group->setTitle(u8"插件管理");
    plugin_tab->addGroup(plugin_group);

    auto* help_tab = new gui::RibbonTab();
    help_tab->setTitle(u8"帮助");
    bar->addTab(help_tab);
    auto* help_group = new gui::RibbonGroup();
    help_group->setTitle(u8"帮助");
    help_tab->addGroup(help_group);

    addCommandButton(plugin_group, u8"插件信息", u8":/icons/show_plugins.svg", u8"show_plugins");
    addCommandButton(plugin_group, u8"渲染后端", u8":/icons/show_plugins.svg", u8"show_render_backends");
    addCommandButton(plugin_group, u8"配置管理", u8":/icons/show_config.svg", u8"show_config");
    addCommandButton(help_group, u8"命令管理器", u8":/icons/show_commands.svg", u8"show_commands");
    addCommandButton(help_group, u8"帮助", u8":/icons/show_help.svg", u8"show_help");
    addCommandButton(help_group, u8"关于", u8":/icons/about.svg", u8"about");
}

/**
 * @brief Adds a second content slot on the main camera (env VINE_VSG_SLOT_DEMO).
 *
 * Demonstrates same-camera stacking: a pass that shares the MAIN camera but
 * runs at a higher order (20, after the order-0 window pass) draws its own
 * retained content into the same window buffer (no clear -> on-top, depth-off
 * style), so the extra boxes stay screen-aligned with the main view while the
 * camera orbits. The backend keys a camera's content slots by the pass order,
 * so the higher-order pass is its own slot stacked on top of the main one.
 *
 * @param render_control Render view whose engine receives the slot pass.
 */
void addSlotOverlayDemo(gui::RenderControl* render_control)
{
    if (std::getenv("VINE_VSG_SLOT_DEMO") == nullptr) {
        return;
    }
    auto* engine = render_control->engine();
    if (engine == nullptr || engine->masterCamera() == nullptr) {
        return;
    }

    // Register after RenderControl::init() has provisioned the default window
    // pass (order 0), so the extra slot stacks on top of the real main view.
    QTimer::singleShot(300, [render_control] {
        auto* engine = render_control->engine();
        if (engine == nullptr || engine->masterCamera() == nullptr) {
            return;
        }
        auto overlay = vine::intrusive_ptr<vine::graphics::Scene>(new vine::graphics::Scene());
        const auto add_overlay_box = [&overlay](const vine::Colorf& color,
                                                const vine::math::Vec3d& centre,
                                                double half) {
            auto box = addBox(overlay.get(), color, u8"slot_overlay", centre,
                              vine::math::Vec3d(half, half, half));
            // Top (on-top) layers are lit by a pure ambient light: a WHITE
            // ambient material makes ambientColor == diffuse == the box color.
            if (auto* geometry = dynamic_cast<vine::graphics::Geometry*>(box->children().front().get())) {
                if (auto* material = geometry->material(); material != nullptr) {
                    material->setAmbient(vine::Colorf(1.0f, 1.0f, 1.0f, 1.0f));
                    material->setSpecular(vine::Colorf(0.0f, 0.0f, 0.0f, 1.0f));
                }
            }
        };
        // Sparse, vivid boxes offset from the main cubes so the overlay is
        // clearly visible on top while tracking the main camera.
        add_overlay_box(vine::Colorf(1.0f, 0.30f, 0.10f, 1.0f), vine::math::Vec3d(-2.9, 0.8, -2.4), 0.35);
        add_overlay_box(vine::Colorf(1.0f, 0.95f, 0.10f, 1.0f), vine::math::Vec3d(2.6, 1.2, 1.9), 0.28);

        auto pass = vine::intrusive_ptr<vine::graphics::RenderPass>(new vine::graphics::RenderPass());
        pass->setName(u8"slot_overlay");
        pass->setCamera(engine->masterCamera());
        pass->setClearEnabled(false); // overlay: no clear -> on-top (depth-off)
        engine->addPass(pass, overlay, 20); // its own (master camera, order 20) slot
    });
}

/**
 * @brief Bakes ONE off-screen target with two content slots (env
 * VINE_VSG_OFFSCREEN_MULTISLOT).
 *
 * Demonstrates C6.4: the same RenderTarget is rendered by two passes that
 * share the master camera but run at distinct orders (-2 and -1), so each is
 * its own content slot (camera, order) under the target's render graph, drawn
 * in ascending order into the same buffer. The first pass clears and draws
 * the main scene (depth-on); the second draws a few extra boxes from a
 * DIFFERENT scene as an on-top (depth-off) slot, so they composite over the
 * first bake. A ScreenPass then shows the baked texture in a picture-in-
 * picture sub-viewport so the result is visible.
 *
 * @param render_control Render view whose engine receives the passes.
 */
void addOffscreenMultiSlotDemo(gui::RenderControl* render_control)
{
    if (std::getenv("VINE_VSG_OFFSCREEN_MULTISLOT") == nullptr) {
        return;
    }
    auto* engine = render_control->engine();
    if (engine == nullptr || engine->masterCamera() == nullptr) {
        return;
    }

    // Shared off-screen target: two content slots bake into it.
    auto target = vine::intrusive_ptr<vine::graphics::RenderTarget>(new vine::graphics::RenderTarget());
    target->setSize(640, 360);
    target->attachColor(vine::graphics::RenderTarget::ColorFormat::RGBA8);
    target->attachDepth(vine::graphics::RenderTarget::DepthFormat::D24);

    // Slot 0: the main engine scene, cleared (depth-on).
    auto pass_main = vine::intrusive_ptr<vine::graphics::RenderPass>(new vine::graphics::RenderPass());
    pass_main->setName(u8"multislot_main");
    pass_main->setCamera(engine->masterCamera());
    pass_main->setRenderTarget(target);
    pass_main->setOutputName(u8"MultiColor");
    engine->addPass(pass_main, -2); // content = engine scene; slot = (master, -2)

    // Slot 1: an extra scene drawn on top (no clear -> on-top, depth-off).
    auto overlay = vine::intrusive_ptr<vine::graphics::Scene>(new vine::graphics::Scene());
    const auto add_overlay_box = [&overlay](const vine::Colorf& color,
                                            const vine::math::Vec3d& centre,
                                            const vine::math::Vec3d& half) {
        auto box = addBox(overlay.get(), color, u8"mslot_overlay", centre, half);
        // On-top (depth-off) slots are lit by a pure ambient light: a WHITE
        // ambient material makes ambientColor == diffuse == the box colour.
        if (auto* geometry = dynamic_cast<vine::graphics::Geometry*>(box->children().front().get())) {
            if (auto* material = geometry->material(); material != nullptr) {
                material->setAmbient(vine::Colorf(1.0f, 1.0f, 1.0f, 1.0f));
                material->setSpecular(vine::Colorf(0.0f, 0.0f, 0.0f, 1.0f));
            }
        }
    };
    add_overlay_box(vine::Colorf(1.0f, 0.30f, 0.10f, 1.0f), vine::math::Vec3d(-2.4, 0.9, -1.8), vine::math::Vec3d(0.55, 0.55, 0.55));
    add_overlay_box(vine::Colorf(0.30f, 0.90f, 0.30f, 1.0f), vine::math::Vec3d(2.2, 1.3, 1.6), vine::math::Vec3d(0.45, 0.45, 0.45));

    auto pass_top = vine::intrusive_ptr<vine::graphics::RenderPass>(new vine::graphics::RenderPass());
    pass_top->setName(u8"multislot_top");
    pass_top->setCamera(engine->masterCamera());
    pass_top->setRenderTarget(target);
    pass_top->setOutputName(u8"MultiColor"); // publishes the same baked target
    pass_top->setClearEnabled(false); // no clear -> on-top (depth-off) slot
    engine->addPass(pass_top, overlay, -1); // slot = (master, -1)

    // PiP screen pass sampling the baked texture into the window.
    const double dpr   = render_control->devicePixelRatio();
    const int    pip_w = static_cast<int>(300.0 * dpr);
    const int    pip_h = static_cast<int>(169.0 * dpr);
    int          sx    = 0;
    int          sy    = 0;
    {
        int sw = engine->frameContext().surface_width;
        int sh = engine->frameContext().surface_height;
        if (sw <= 0 || sh <= 0) {
            sw = 1280;
            sh = 720;
        }
        const int margin = 8;
        sx = sw - pip_w - margin;
        sy = sh - pip_h - margin;
    }
    auto screen = vine::intrusive_ptr<vine::graphics::ScreenPass>(new vine::graphics::ScreenPass());
    screen->setName(u8"multislot_pip");
    screen->addInputName(u8"MultiColor");
    screen->setViewport(sx, sy, pip_w, pip_h);
    engine->addPass(screen, 100);
}

/**
 * @brief Renders the engine scene into an MRT G-buffer target and previews
 * each colour attachment (env VINE_VSG_GBUFFER).
 *
 * Demonstrates MRT / G-buffer (one geometry traversal producing several
 * textures): a custom three-output program writes
 *   att 0 = albedo (unlit material diffuse, RGBA8),
 *   att 1 = view-space normal (RGBA16F),
 *   att 2 = view-space position (RGBA16F)
 * into one off-screen target in a single pass (order -3). Three ScreenPasses
 * then sample colour attachments 0/1/2 of the same published target as small
 * picture-in-picture previews, so each buffer is visible and independently
 * sampleable.
 *
 * @param render_control Render view whose engine receives the passes.
 */
void addGbufferDemo(gui::RenderControl* render_control)
{
    if (std::getenv("VINE_VSG_GBUFFER") == nullptr) {
        return;
    }
    auto* engine = render_control->engine();
    if (engine == nullptr || engine->masterCamera() == nullptr) {
        return;
    }

    // G-buffer program: unlit geometry pass writing three fragment outputs.
    auto program = vine::intrusive_ptr<vine::graphics::ShaderProgram>(new vine::graphics::ShaderProgram());
    program->setName(u8"gbuffer_geometry");
    vine::graphics::ShaderStage vs;
    vs.type   = vine::graphics::ShaderStageType::Vertex;
    vs.source = u8"#version 450\n"
                u8"layout(push_constant) uniform PushConstants { mat4 projection; mat4 modelView; } pc;\n"
                u8"layout(location = 0) in vec3 vsg_Vertex;\n"
                u8"layout(location = 1) in vec3 vsg_Normal;\n"
                u8"layout(location = 0) out vec3 v_view_pos;\n"
                u8"layout(location = 1) out vec3 v_view_normal;\n"
                u8"void main()\n"
                u8"{\n"
                u8"    vec4 view_pos = pc.modelView * vec4(vsg_Vertex, 1.0);\n"
                u8"    v_view_pos = view_pos.xyz;\n"
                u8"    v_view_normal = mat3(pc.modelView) * vsg_Normal;\n"
                u8"    gl_Position = pc.projection * view_pos;\n"
                u8"}\n";
    program->addStage(vs);
    vine::graphics::ShaderStage fs;
    fs.type   = vine::graphics::ShaderStageType::Fragment;
    fs.source = u8"#version 450\n"
                u8"layout(location = 0) in vec3 v_view_pos;\n"
                u8"layout(location = 1) in vec3 v_view_normal;\n"
                u8"layout(location = 0) out vec4 out_albedo;\n"
                u8"layout(location = 1) out vec4 out_normal;\n"
                u8"layout(location = 2) out vec4 out_position;\n"
                u8"layout(set = 0, binding = 0, std140) uniform MaterialBlock\n"
                u8"{\n"
                u8"    vec4 ambient;\n"
                u8"    vec4 diffuse;\n"
                u8"    vec4 specular;\n"
                u8"    vec4 emissive;\n"
                u8"    float shininess;\n"
                u8"    float alphaMask;\n"
                u8"    float alphaMaskCutoff;\n"
                u8"} material;\n"
                u8"void main()\n"
                u8"{\n"
                u8"    out_albedo   = vec4(material.diffuse.rgb, 1.0);\n"
                u8"    out_normal   = vec4(normalize(v_view_normal), 1.0);\n"
                u8"    out_position = vec4(v_view_pos, 1.0);\n"
                u8"}\n";
    program->addStage(fs);

    // One off-screen MRT target with three colour attachments (+ depth).
    auto target = vine::intrusive_ptr<vine::graphics::RenderTarget>(new vine::graphics::RenderTarget());
    target->setSize(640, 360);
    target->attachColor(vine::graphics::RenderTarget::ColorFormat::RGBA8);   // att 0: albedo
    target->attachColor(vine::graphics::RenderTarget::ColorFormat::RGBA16F); // att 1: normal
    target->attachColor(vine::graphics::RenderTarget::ColorFormat::RGBA16F); // att 2: view pos
    target->attachDepth(vine::graphics::RenderTarget::DepthFormat::D24);

    // Geometry pass: the engine scene through the multi-output program into
    // the MRT target. The whole target is published as one named output;
    // consumers pick which colour attachment to sample.
    auto pass = vine::intrusive_ptr<vine::graphics::RenderPass>(new vine::graphics::RenderPass());
    pass->setName(u8"gbuffer_pass");
    pass->setCamera(engine->masterCamera());
    pass->setRenderTarget(target);
    pass->setProgramOverride(program);
    pass->setOutputName(u8"GBuffer");
    engine->addPass(pass, -3);

    // Preview each colour attachment of the same published target.
    const double dpr   = render_control->devicePixelRatio();
    const int    pip_w = static_cast<int>(108.0 * dpr);
    const int    pip_h = static_cast<int>(61.0 * dpr);
    for (int attachment = 0; attachment < 3; ++attachment) {
        auto screen = vine::intrusive_ptr<vine::graphics::ScreenPass>(new vine::graphics::ScreenPass());
        screen->setName(u8"gbuffer_preview");
        screen->addInputName(u8"GBuffer");
        screen->setSourceAttachment(attachment);
        const int x = 8 + attachment * (pip_w + 8);
        screen->setViewport(x, 8, pip_w, pip_h);
        engine->addPass(screen, 120 + attachment);
    }
}

// Forward declarations (the shared helpers are defined further down).
vine::intrusive_ptr<vine::graphics::ShaderProgram> makeGbufferGeometryProgram();
vine::intrusive_ptr<vine::graphics::RenderTarget>  makeGbufferTarget();
vine::intrusive_ptr<vine::graphics::ShaderProgram> makeDeferredLightProgram();

/**
 * @brief Renders the engine scene into a G-buffer and lights it in a
 * fullscreen deferred pass (env VINE_VSG_DEFERRED).
 *
 * S2a vertical slice: the G-buffer geometry pass (three colour attachments:
 * albedo / view normal / view position) runs off-screen; a fullscreen
 * deferred-lighting ScreenPass (a fragment program sampling every G-buffer
 * attachment) re-lights it in one draw and shows the result in a preview
 * sub-viewport. The main window keeps its forward-lit view, so the two can be
 * compared side by side (A/B). The lights are the content scene's own
 * (ambient + directional), pushed to the lighting shader in view space.
 *
 * @param render_control Render view whose engine receives the passes.
 */
void addDeferredDemo(gui::RenderControl* render_control)
{
    if (std::getenv("VINE_VSG_DEFERRED") == nullptr) {
        return;
    }
    auto* engine = render_control->engine();
    if (engine == nullptr || engine->masterCamera() == nullptr) {
        return;
    }

    // Shared G-buffer: engine scene through the multi-output program.
    auto target = makeGbufferTarget();    auto gbuf   = vine::intrusive_ptr<vine::graphics::RenderPass>(new vine::graphics::RenderPass());
    gbuf->setName(u8"deferred_gbuffer");
    gbuf->setCamera(engine->masterCamera());
    gbuf->setRenderTarget(target);
    gbuf->setProgramOverride(makeGbufferGeometryProgram());
    gbuf->setOutputName(u8"GBuffer");
    engine->addPass(gbuf, -3);

    // Deferred-lighting preview: a fullscreen program sampling the G-buffer.
    const double dpr = render_control->devicePixelRatio();
    const int    pw  = static_cast<int>(340.0 * dpr);
    const int    ph  = static_cast<int>(191.0 * dpr);
    auto light = vine::intrusive_ptr<vine::graphics::ScreenPass>(new vine::graphics::ScreenPass());
    light->setName(u8"deferred_light");
    light->addInputName(u8"GBuffer");
    light->setCamera(engine->masterCamera());
    light->setProgram(makeDeferredLightProgram());
    light->setViewport(8, 8, pw, ph);
    engine->addPass(light, 130);
}

/**
 * @brief Builds the G-buffer geometry program (four fragment outputs).
 *
 * One traversal writes albedo (attachment 0), view-space normal (1, alpha =
 * shininess / 256), specular colour (2) and view-space position (3) into an
 * MRT target. Unlit: the deferred lighting pass shades from these buffers
 * (position is stored, not reconstructed from depth — exact under the
 * backend's depth convention).
 *
 * @return The shared geometry program.
 */
vine::intrusive_ptr<vine::graphics::ShaderProgram> makeGbufferGeometryProgram()
{
    auto program = vine::intrusive_ptr<vine::graphics::ShaderProgram>(new vine::graphics::ShaderProgram());
    program->setName(u8"gbuffer_geometry");
    vine::graphics::ShaderStage vs;
    vs.type   = vine::graphics::ShaderStageType::Vertex;
    vs.source = u8"#version 450\n"
                u8"layout(push_constant) uniform PushConstants { mat4 projection; mat4 modelView; } pc;\n"
                u8"layout(location = 0) in vec3 vsg_Vertex;\n"
                u8"layout(location = 1) in vec3 vsg_Normal;\n"
                u8"layout(location = 0) out vec3 v_view_pos;\n"
                u8"layout(location = 1) out vec3 v_view_normal;\n"
                u8"void main()\n"
                u8"{\n"
                u8"    vec4 view_pos = pc.modelView * vec4(vsg_Vertex, 1.0);\n"
                u8"    v_view_pos = view_pos.xyz;\n"
                u8"    v_view_normal = mat3(pc.modelView) * vsg_Normal;\n"
                u8"    gl_Position = pc.projection * view_pos;\n"
                u8"}\n";
    program->addStage(vs);
    vine::graphics::ShaderStage fs;
    fs.type   = vine::graphics::ShaderStageType::Fragment;
    fs.source = u8"#version 450\n"
                u8"layout(location = 0) in vec3 v_view_pos;\n"
                u8"layout(location = 1) in vec3 v_view_normal;\n"
                u8"layout(location = 0) out vec4 out_albedo;\n"
                u8"layout(location = 1) out vec4 out_normal;\n"
                u8"layout(location = 2) out vec4 out_specular;\n"
                u8"layout(location = 3) out vec4 out_position;\n"
                u8"layout(set = 0, binding = 0, std140) uniform MaterialBlock\n"
                u8"{\n"
                u8"    vec4 ambient;\n"
                u8"    vec4 diffuse;\n"
                u8"    vec4 specular;\n"
                u8"    vec4 emissive;\n"
                u8"    float shininess;\n"
                u8"    float alphaMask;\n"
                u8"    float alphaMaskCutoff;\n"
                u8"} material;\n"
                u8"void main()\n"
                u8"{\n"
                u8"    out_albedo = vec4(material.diffuse.rgb, 1.0);\n"
                u8"    out_normal = vec4(normalize(v_view_normal), clamp(material.shininess / 256.0, 0.0, 1.0));\n"
                u8"    out_specular = vec4(clamp(material.specular.rgb, 0.0, 1.0), 1.0);\n"
                u8"    out_position = vec4(v_view_pos, 1.0);\n"
                u8"}\n";
    program->addStage(fs);
    return program;
}

/**
 * @brief Builds a shared G-buffer MRT target (albedo / normal / spec / pos).
 *
 * Four colour attachments (albedo RGBA8, view-space normal RGBA16F with alpha
 * = shininess / 256, specular colour RGBA8, view-space position RGBA16F) plus
 * a depth attachment. Position is stored so the deferred lighting pass is
 * exact under the backend's depth convention.
 *
 * @return The target with four colour attachments + depth.
 */
vine::intrusive_ptr<vine::graphics::RenderTarget> makeGbufferTarget()
{
    auto target = vine::intrusive_ptr<vine::graphics::RenderTarget>(new vine::graphics::RenderTarget());
    target->setSize(640, 360);
    target->attachColor(vine::graphics::RenderTarget::ColorFormat::RGBA8);   // att 0: albedo
    target->attachColor(vine::graphics::RenderTarget::ColorFormat::RGBA16F); // att 1: view normal (+ shininess in alpha)
    target->attachColor(vine::graphics::RenderTarget::ColorFormat::RGBA8);   // att 2: specular colour
    target->attachColor(vine::graphics::RenderTarget::ColorFormat::RGBA16F); // att 3: view position
    target->attachDepth(vine::graphics::RenderTarget::DepthFormat::D24);
    return target;
}

/**
 * @brief Builds the deferred-lighting fragment program (fullscreen).
 *
 * Samples the G-buffer's albedo / normal / specular / view-position
 * attachments (binding 0..3) and shades ambient + up to three directional
 * lights whose parameters arrive in the view-space push block (see the
 * backend ABI).
 *
 * @return The lighting program (fragment stage only).
 */
vine::intrusive_ptr<vine::graphics::ShaderProgram> makeDeferredLightProgram()
{
    auto program = vine::intrusive_ptr<vine::graphics::ShaderProgram>(new vine::graphics::ShaderProgram());
    program->setName(u8"deferred_light");
    vine::graphics::ShaderStage fs;
    fs.type   = vine::graphics::ShaderStageType::Fragment;
    fs.source = u8"#version 450\n"
                u8"layout(location = 0) in vec2 v_uv;\n"
                u8"layout(location = 0) out vec4 out_color;\n"
                u8"layout(binding = 0) uniform sampler2D albedo_tex;\n"
                u8"layout(binding = 1) uniform sampler2D normal_tex;\n"
                u8"layout(binding = 2) uniform sampler2D spec_tex;\n"
                u8"layout(binding = 3) uniform sampler2D pos_tex;\n"
                u8"layout(push_constant) uniform PushConstants\n"
                u8"{\n"
                u8"    vec4 ambient;\n"
                u8"    vec4 projparms;\n"
                u8"    vec4 sun_dir[3];\n"
                u8"    vec4 sun_color[3];\n"
                u8"} pc;\n"
                u8"void main()\n"
                u8"{\n"
                u8"    // vsg projects world-up to the top G-buffer row (reverse-Y\n"
                u8"    // perspective) and v_uv.y == 0 is the top of the screen,\n"
                u8"    // so v_uv samples the buffers upright (no Y flip).\n"
                u8"    vec2 uv = v_uv;\n"
                u8"    vec3 albedo = texture(albedo_tex, uv).rgb;\n"
                u8"    vec4 n4 = texture(normal_tex, uv);\n"
                u8"    vec3 n = n4.xyz;\n"
                u8"    vec3 pos = texture(pos_tex, uv).xyz;\n"
                u8"    // Background (stored position ~0 where nothing was drawn).\n"
                u8"    if (dot(pos, pos) < 1e-6) { out_color = vec4(vec3(0.06), 1.0); return; }\n"
                u8"    n = normalize(n);\n"
                u8"    // Per-pixel material from the G-buffer: specular colour rides\n"
                u8"    // attachment 2, shininess rides the normal attachment's alpha.\n"
                u8"    vec3 spec_col = clamp(texture(spec_tex, uv).rgb, 0.0, 1.0);\n"
                u8"    float shininess = max(n4.a * 256.0, 1.0);\n"
                u8"    vec3 view_dir = normalize(-pos);\n"
                u8"    vec3 color = albedo * (pc.ambient.rgb * pc.ambient.a);\n"
                u8"    for (int i = 0; i < 3; ++i)\n"
                u8"    {\n"
                u8"        vec3 d = pc.sun_dir[i].xyz;\n"
                u8"        vec3 c = pc.sun_color[i].rgb;\n"
                u8"        float a = pc.sun_color[i].a;\n"
                u8"        if (dot(d, d) < 1e-6) continue;\n"
                u8"        vec3 L = normalize(-d);\n"
                u8"        float ndl = max(dot(n, L), 0.0);\n"
                u8"        color += albedo * c * a * ndl;\n"
                u8"        vec3 H = normalize(L + view_dir);\n"
                u8"        float spec = pow(max(dot(n, H), 0.0), shininess);\n"
                u8"        color += c * a * spec * spec_col;\n"
                u8"    }\n"
                u8"    out_color = vec4(color, 1.0);\n"
                u8"}\n";
    program->addStage(fs);
    return program;
}

/**
 * @brief Makes the DEFERRED pipeline the main window (S2b/S3, default).
 *
 * Instead of a forward window scene pass, the engine scene is baked into an
 * off-screen G-buffer (order -3) and a fullscreen deferred-lighting pass
 * (order 0, camera = master camera, window target) shades it into the whole
 * window. The lighting pass presents the master view to the window, so
 * RenderControl does not add its default forward window pass; HUD overlays
 * (order 10+) still stack above the lit result.
 *
 * Dev switch (env VINE_VSG_DEFERRED_FULL): the CLASSIC FORWARD window remains
 * the default (RenderControl provisions the window pass when no deferred
 * pass claims the master view); run this deferred main only when the env is
 * set, for A/B / debugging.
 *
 * @param render_control Render view whose engine receives the passes.
 */
void addDeferredMain(gui::RenderControl* render_control)
{
    if (std::getenv("VINE_VSG_DEFERRED_FULL") == nullptr) {
        return;
    }
    auto* engine = render_control->engine();
    if (engine == nullptr || engine->masterCamera() == nullptr) {
        return;
    }

    // G-buffer: engine scene through the multi-output program.
    auto target = makeGbufferTarget();
    auto gbuf   = vine::intrusive_ptr<vine::graphics::RenderPass>(new vine::graphics::RenderPass());
    gbuf->setName(u8"deferred_gbuffer");
    gbuf->setCamera(engine->masterCamera());
    gbuf->setRenderTarget(target);
    gbuf->setProgramOverride(makeGbufferGeometryProgram());
    gbuf->setOutputName(u8"GBuffer");
    engine->addPass(gbuf, -3);

    // Deferred lighting to the WHOLE window at order 0 (the new "main pass").
    auto light = vine::intrusive_ptr<vine::graphics::ScreenPass>(new vine::graphics::ScreenPass());
    light->setName(u8"deferred_light");
    light->addInputName(u8"GBuffer");
    light->setCamera(engine->masterCamera());
    light->setProgram(makeDeferredLightProgram());
    engine->addPass(light, 0);
}

AppShellDock buildAppShellDock(gui::MainWindow* wnd)
{
    AppShellDock result;

    auto* manager = wnd->dockPanelManager();

    auto* left_panel = manager->createDockPanel(u8"项目", gui::DockAreas::Left);
    left_panel->setId(u8"dock_project");

    // Render view in the central client area; init() picks the first
    // registered render backend (e.g. "vsg") by default.
    auto* render_control = new gui::RenderControl();
    manager->setCentralWidget(render_control);
    addDemoCubes(render_control->engine()->scene());
    addAxisGizmo(render_control);
    addDemoLighting(render_control);
    addOffscreenValidationPass(render_control);
    // Dev switch: VINE_VSG_SLOT_DEMO stacks a second (camera, content slot)
    // overlay pass on the main camera to validate same-view multi-slot drawing.
    addSlotOverlayDemo(render_control);
    // Dev switch: VINE_VSG_OFFSCREEN_MULTISLOT bakes ONE off-screen target with
    // two content slots (main scene + on-top overlay) shown via PiP (C6.4).
    addOffscreenMultiSlotDemo(render_control);
    // Dev switch: VINE_VSG_GBUFFER renders the engine scene into an MRT
    // G-buffer target (albedo / normal / view position) and previews each
    // colour attachment via PiP (Slice 1 of the MRT / deferred work).
    addGbufferDemo(render_control);
    // Dev switch: VINE_VSG_DEFERRED adds a fullscreen deferred-lighting pass
    // that reads the G-buffer and shows the lit result (S2a, A/B preview).
    addDeferredDemo(render_control);
    // Default main pipeline: the CLASSIC FORWARD window (RenderControl
    // provisions it). Dev switch VINE_VSG_DEFERRED_FULL swaps the main window
    // to the deferred G-buffer + fullscreen lighting pipeline (S2b/S3).
    addDeferredMain(render_control);
    // Dev switch: setting VINE_SHADER_PRESET exercises the FlatShaded preset
    // through the whole engine/backend path (default = StandardPhong).
    if (std::getenv("VINE_SHADER_PRESET") != nullptr) {
        render_control->engine()->setShaderPreset(vine::graphics::ShaderPreset::FlatShaded);
    }
    // Register the 3D view so other plugins (tests/editors) can reach the
    // render engine/scene without depending on app shell internals.
    wnd->setPrimaryRenderControl(render_control);
    result.render_control = render_control;

    // Initialize once the window is shown and the central widget is laid out:
    // the backend attaches to the realized native surface. Deferred so the
    // first layout pass has happened by the time init() runs.
    QTimer::singleShot(100, [render_control] { render_control->init(); });

    auto* right_panel = manager->createDockPanel(u8"属性", gui::DockAreas::Right);
    right_panel->setId(u8"dock_properties");

    auto* console_panel = new gui::ConsolePanel();
    auto* console_dock  = manager->createDockPanel(u8"控制台", console_panel, gui::DockAreas::Bottom);
    console_dock->setId(u8"dock_console");

    if (auto* app = ::vine::obj_cast<gui::GuiApplication>(Application::current())) {
        app->setConsolePanel(console_panel);
    }

    result.console_panel = console_panel;
    return result;
}

V_APPFW_NS_END
