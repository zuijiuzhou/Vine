#include "AppShellUi.hpp"

#include <utility>

#include <QTimer>

#include <vine/Colorf.hpp>
#include <vine/graphics/AxisGizmo.hpp>
#include <vine/graphics/Geometry.hpp>
#include <vine/graphics/Material.hpp>
#include <vine/graphics/Node.hpp>
#include <vine/graphics/RenderEngine.hpp>
#include <vine/graphics/Scene.hpp>
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
 * @brief Builds a flat-shaded cube node centred at a world-space position.
 *
 * The box is authored as an IndexedTriangleMesh (six faces, twelve triangles,
 * per-face vertex normals) so the vsg backend renders it directly.
 *
 * @param scene   Scene receiving the node.
 * @param diffuse Flat diffuse colour of the cube.
 * @param name    Node and geometry name.
 * @param centre  World-space centre of the cube.
 * @param half    Half edge length of the cube.
 * @return The created node (kept alive by the scene).
 */
vine::intrusive_ptr<vine::graphics::Node> addDemoCube(vine::graphics::Scene* scene,
                                                      const vine::Colorf& diffuse,
                                                      const vine::String& name,
                                                      const vine::math::Vec3d& centre,
                                                      float half)
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
        const float cu[4]       = { -1.0f, 1.0f, 1.0f, -1.0f };
        const float cv[4]       = { -1.0f, -1.0f, 1.0f, 1.0f };
        for (int k = 0; k < 4; ++k) {
            const Vec3f corner =
                normal * half + u * (cu[k] * half) + v * (cv[k] * half);
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
    geometry->setShape(mesh.get());

    auto material = vine::intrusive_ptr<vine::graphics::Material>(new vine::graphics::Material());
    material->setDiffuse(diffuse);
    geometry->setMaterial(material.get());

    auto node = vine::intrusive_ptr<vine::graphics::Node>(new vine::graphics::Node());
    node->setName(name);
    node->setLocalTransform(vine::math::translate(centre));
    node->addDrawable(geometry.get());

    scene->addNode(node.get());
    return node;
}

/**
 * @brief Adds the default demo content: four colour-coded cubes in a grid.
 *
 * Temporary demo content: the default scene is empty, so without this the
 * render view would only show the clear colour.
 *
 * @param scene Scene to receive the cubes.
 */
void addDemoCubes(vine::graphics::Scene* scene)
{
    const float half = 0.5f;
    addDemoCube(scene, vine::Colorf(0.85f, 0.20f, 0.20f, 1.0f), u8"cube_red",
                vine::math::Vec3d(-1.4, 1.4, 0.0), half);
    addDemoCube(scene, vine::Colorf(0.25f, 0.75f, 0.25f, 1.0f), u8"cube_green",
                vine::math::Vec3d(1.4, 1.4, 0.0), half);
    addDemoCube(scene, vine::Colorf(0.25f, 0.40f, 0.90f, 1.0f), u8"cube_blue",
                vine::math::Vec3d(-1.4, -1.4, 0.0), half);
    addDemoCube(scene, vine::Colorf(0.95f, 0.72f, 0.10f, 1.0f), u8"cube_amber",
                vine::math::Vec3d(1.4, -1.4, 0.0), half);
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
    gizmo->setSourceCamera(render_control->engine()->camera());
    // Qt reports logical sizes; the overlay is positioned in device pixels.
    gizmo->setPixelRatio(render_control->devicePixelRatio());
    render_control->engine()->addOverlay(gizmo);
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
