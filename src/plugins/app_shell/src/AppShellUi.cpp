#include "AppShellUi.hpp"

#include <QWidget>

#include <vine/appfw/gui/ConsolePanel.hpp>
#include <vine/appfw/gui/Control.hpp>
#include <vine/appfw/gui/DockPanel.hpp>
#include <vine/appfw/gui/DockPanelManager.hpp>
#include <vine/appfw/gui/GuiApplication.hpp>
#include <vine/appfw/gui/Icon.hpp>
#include <vine/appfw/gui/MainWindow.hpp>
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

} // namespace

void buildAppShellRibbon(gui::MainWindow* wnd)
{
    auto* bar = wnd->ribbonBar();

    auto* pluginTab = new gui::RibbonTab();
    pluginTab->setTitle(u8"插件");
    bar->addTab(pluginTab);
    auto* pluginGroup = new gui::RibbonGroup();
    pluginGroup->setTitle(u8"插件管理");
    pluginTab->addGroup(pluginGroup);

    auto* helpTab = new gui::RibbonTab();
    helpTab->setTitle(u8"帮助");
    bar->addTab(helpTab);
    auto* helpGroup = new gui::RibbonGroup();
    helpGroup->setTitle(u8"帮助");
    helpTab->addGroup(helpGroup);

    addCommandButton(pluginGroup, u8"插件信息", u8":/icons/show_plugins.svg", u8"show_plugins");
    addCommandButton(pluginGroup, u8"配置管理", u8":/icons/show_config.svg", u8"show_config");
    addCommandButton(helpGroup, u8"命令管理器", u8":/icons/show_commands.svg", u8"show_commands");
    addCommandButton(helpGroup, u8"帮助", u8":/icons/show_help.svg", u8"show_help");
    addCommandButton(helpGroup, u8"关于", u8":/icons/about.svg", u8"about");
}

AppShellDock buildAppShellDock(gui::MainWindow* wnd)
{
    AppShellDock result;

    auto* manager = wnd->dockPanelManager();

    auto* leftPanel = manager->createDockPanel(u8"项目", gui::DockAreas::Left);
    leftPanel->setId(u8"dock_project");

    manager->setCentralWidget(new gui::Control(new QWidget()));

    auto* rightPanel = manager->createDockPanel(u8"属性", gui::DockAreas::Right);
    rightPanel->setId(u8"dock_properties");

    auto* consolePanel = new gui::ConsolePanel();
    auto* consoleDock  = manager->createDockPanel(u8"控制台", consolePanel, gui::DockAreas::Bottom);
    consoleDock->setId(u8"dock_console");

    if (auto* app = ::vine::obj_cast<gui::GuiApplication>(Application::current())) {
        app->setConsolePanel(consolePanel);
    }

    result.console_panel = consolePanel;
    return result;
}

V_APPFW_NS_END
