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
 * The gizmo is a generic graphics Overlay: it mirrors the main camera's
 * orientation and is drawn by the render backend into a small sub-viewport.
 *
 * @param render_control Render view whose engine receives the overlay.
 */
void addAxisGizmo(gui::RenderControl* render_control)
{
    auto gizmo = vine::intrusive_ptr<vine::graphics::AxisGizmo>(new vine::graphics::AxisGizmo());
    gizmo->setSourceCamera(render_control->engine()->masterCamera());
    // Qt reports logical sizes; the overlay is positioned in device pixels.
    gizmo->setPixelRatio(render_control->devicePixelRatio());
    render_control->engine()->addOverlay(gizmo);
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
