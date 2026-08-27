#include "AppShellPlugin.hpp"

#include <vine/appfw/Application.hpp>
#include <vine/appfw/CommandManager.hpp>
#include <vine/appfw/gui/MainWindow.hpp>
#include <vine/appfw/gui/RibbonBar.hpp>
#include <vine/appfw/gui/RibbonButton.hpp>
#include <vine/appfw/gui/RibbonGroup.hpp>
#include <vine/appfw/gui/RibbonTab.hpp>

#include <vine/appfw/plugin_export.hpp>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(AppShellPlugin, Plugin)

AppShellPlugin::AppShellPlugin() = default;

PluginInfo AppShellPlugin::info() const
{
    return PluginInfo{ u8"app_shell", u8"1.0.0", u8"应用外壳：基础界面（插件信息与配置管理）", u8"Vine" };
}

void AppShellPlugin::load(PluginLoadContext* context)
{
    (void)context;

    // Add two Ribbon buttons on the main window; each triggers one command.
    auto* wnd = gui::MainWindow::current();
    if (!wnd) {
        return;
    }
    auto* bar = wnd->ribbonBar();
    auto* tab = new gui::RibbonTab();
    tab->setTitle(u8"插件");
    bar->addTab(tab);
    auto* grp = new gui::RibbonGroup();
    grp->setTitle(u8"插件管理");
    tab->addGroup(grp);

    auto* btn1 = new gui::RibbonButton();
    btn1->setText(u8"插件信息");
    btn1->clicked.addHandler([](gui::RibbonButton&, EventArgs&) {
        if (auto* app = Application::current()) {
            app->commandManager()->executeCommand(u8"showPlugins");
        }
    });
    grp->addButton(btn1);

    auto* btn2 = new gui::RibbonButton();
    btn2->setText(u8"配置管理");
    btn2->clicked.addHandler([](gui::RibbonButton&, EventArgs&) {
        if (auto* app = Application::current()) {
            app->commandManager()->executeCommand(u8"showConfig");
        }
    });
    grp->addButton(btn2);
}

V_PLUGIN_DECLARE(AppShellPlugin, u8"app_shell", u8"1.0.0", u8"应用外壳：基础界面（插件信息与配置管理）", u8"Vine", { })

V_APPFW_NS_END
