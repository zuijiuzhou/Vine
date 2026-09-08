#include "AppShellPlugin.hpp"

#include <vine/appfw/gui/MainWindow.hpp>
#include <vine/appfw/plugin_export.hpp>

#include "AppShellUi.hpp"
#include "ConsoleLogRouter.hpp"

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(AppShellPlugin, Plugin)

AppShellPlugin::AppShellPlugin() = default;

void AppShellPlugin::load(PluginLoadContext* context)
{
    auto* wnd = gui::MainWindow::current();
    if (!wnd) {
        return;
    }

    // UI：Ribbon 选项卡 + 工作区停靠布局（见 AppShellUi）。
    buildAppShellRibbon(wnd);
    auto dock = buildAppShellDock(wnd);

    // 控制台日志路由：配置项注册、配置同步与 sink 安装（见 ConsoleLogRouter）。
    installConsoleLogSink(dock.console_panel, context);
}

void AppShellPlugin::unload(PluginLoadContext* context)
{
    (void)context;
    uninstallConsoleLogSink();
}

V_DECLARE_PLUGIN(AppShellPlugin, u8"app_shell", u8"AppShell", u8"1.0.0", u8"应用外壳：基础界面（插件信息与配置管理）", u8"Vine", { })

V_APPFW_NS_END
