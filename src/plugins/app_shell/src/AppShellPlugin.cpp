#include "AppShellPlugin.hpp"

#include "ConsoleLogRouter.hpp"

#include <QWidget>

#include <vine/appfw/Application.hpp>
#include <vine/appfw/ConfigItem.hpp>
#include <vine/appfw/ConfigManager.hpp>
#include <vine/appfw/ConfigStandard.hpp>
#include <vine/appfw/CommandManager.hpp>
#include <vine/appfw/MainThreadDispatcher.hpp>
#include <vine/appfw/PluginLoadContext.hpp>
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

#include <vine/appfw/plugin_export.hpp>

#include <vine/logging/Log.hpp>

V_APPFW_NS_BEGIN

namespace
{

/**
 * @brief Maps a log level to the console's semantic message type.
 */
gui::ConsoleMessageType toConsoleType(logging::LogLevel level)
{
    switch (level) {
    case logging::LogLevel::Warn:     return gui::ConsoleMessageType::Warning;
    case logging::LogLevel::Error:
    case logging::LogLevel::Critical: return gui::ConsoleMessageType::Error;
    default:                          return gui::ConsoleMessageType::Normal;
    }
}

} // namespace

V_OBJECT_META_IMPL(AppShellPlugin, Plugin)

AppShellPlugin::AppShellPlugin() = default;

PluginInfo AppShellPlugin::info() const
{
    return PluginInfo{ u8"app_shell", u8"1.0.0", u8"应用外壳：基础界面（插件信息与配置管理）", u8"Vine" };
}

void AppShellPlugin::load(PluginLoadContext* context)
{
    (void)context;

    auto* wnd = gui::MainWindow::current();
    if (!wnd) {
        return;
    }
    auto* bar = wnd->ribbonBar();

    // ---- 插件 选项卡 ----
    auto* pluginTab = new gui::RibbonTab();
    pluginTab->setTitle(u8"插件");
    bar->addTab(pluginTab);
    auto* pluginGrp = new gui::RibbonGroup();
    pluginGrp->setTitle(u8"插件管理");
    pluginTab->addGroup(pluginGrp);

    auto* btnPlugins = new gui::RibbonButton();
    btnPlugins->setText(u8"插件信息");
    btnPlugins->setIcon(gui::Icon(String(u8":/icons/show_plugins.svg")));
    btnPlugins->setButtonSize(gui::RibbonItemSize::Large);
    btnPlugins->clicked.addHandler([](gui::RibbonButton&, EventArgs&) {
        if (auto* app = Application::current()) {
            app->commandManager()->executeDetached(u8"show_plugins");
        }
    });
    pluginGrp->addButton(btnPlugins);

    auto* btnConfig = new gui::RibbonButton();
    btnConfig->setText(u8"配置管理");
    btnConfig->setIcon(gui::Icon(String(u8":/icons/show_config.svg")));
    btnConfig->setButtonSize(gui::RibbonItemSize::Large);
    btnConfig->clicked.addHandler([](gui::RibbonButton&, EventArgs&) {
        if (auto* app = Application::current()) {
            app->commandManager()->executeDetached(u8"show_config");
        }
    });
    pluginGrp->addButton(btnConfig);

    // ---- 帮助 选项卡 ----
    auto* helpTab = new gui::RibbonTab();
    helpTab->setTitle(u8"帮助");
    bar->addTab(helpTab);
    auto* helpGrp = new gui::RibbonGroup();
    helpGrp->setTitle(u8"帮助");
    helpTab->addGroup(helpGrp);

    auto* btnCommands = new gui::RibbonButton();
    btnCommands->setText(u8"命令管理器");
    btnCommands->setIcon(gui::Icon(String(u8":/icons/show_commands.svg")));
    btnCommands->setButtonSize(gui::RibbonItemSize::Large);
    btnCommands->clicked.addHandler([](gui::RibbonButton&, EventArgs&) {
        if (auto* app = Application::current()) {
            app->commandManager()->executeDetached(u8"show_commands");
        }
    });
    helpGrp->addButton(btnCommands);

    auto* btnHelp = new gui::RibbonButton();
    btnHelp->setText(u8"帮助");
    btnHelp->setIcon(gui::Icon(String(u8":/icons/show_help.svg")));
    btnHelp->setButtonSize(gui::RibbonItemSize::Large);
    btnHelp->clicked.addHandler([](gui::RibbonButton&, EventArgs&) {
        if (auto* app = Application::current()) {
            app->commandManager()->executeDetached(u8"show_help");
        }
    });
    helpGrp->addButton(btnHelp);

    auto* btnAbout = new gui::RibbonButton();
    btnAbout->setText(u8"关于");
    btnAbout->setIcon(gui::Icon(String(u8":/icons/about.svg")));
    btnAbout->setButtonSize(gui::RibbonItemSize::Large);
    btnAbout->clicked.addHandler([](gui::RibbonButton&, EventArgs&) {
        if (auto* app = Application::current()) {
            app->commandManager()->executeDetached(u8"about");
        }
    });
    helpGrp->addButton(btnAbout);

    // ---- 工作区布局：左 / 中央 / 右三个空面板，底部控制台 ----
    auto* mgr = wnd->dockPanelManager();

    auto* leftPanel = mgr->createDockPanel(u8"项目", gui::DockAreas::Left);
    leftPanel->setId(u8"dock_project");

    mgr->setCentralWidget(new gui::Control(new QWidget()));

    auto* rightPanel = mgr->createDockPanel(u8"属性", gui::DockAreas::Right);
    rightPanel->setId(u8"dock_properties");

    auto* consolePanel = new gui::ConsolePanel();
    auto* consoleDock  = mgr->createDockPanel(u8"控制台", consolePanel, gui::DockAreas::Bottom);
    consoleDock->setId(u8"dock_console");

    if (auto* app = ::vine::obj_cast<gui::GuiApplication>(Application::current())) {
        app->setConsolePanel(consolePanel);
    }

    // 将默认日志路由到控制台面板：日志可来自任意线程，需投递回主线程刷新 UI。
    // 开关的“真源”是 ConfigManager（配置窗口可改），原子只是 sink 的无锁缓存。
    auto* app = Application::current();
    if (app && app->configManager()) {
        auto* cfg       = app->configManager();
        const String key = consoleLogConfigKey();
        if (!cfg->contains(key)) {
            cfg->setBool(key, true);
        }
        consoleLogEnabledState()->store(cfg->getBool(key, true), std::memory_order_relaxed);

        // 配置窗口修改 / 命令切换都会触发 changed，这里同步 sink 的原子开关。
        config_change_handler_id_ = cfg->changed.addHandler(
            [alive = consoleLogEnabledState(), key](ConfigManager& mgr, ConfigChangedEventArgs& args) {
                if (args.key() == key) {
                    alive->store(mgr.getBool(key, true), std::memory_order_relaxed);
                }
            });
    }

    log_sink_ = logging::LogSink::function(
        [panel = consolePanel, alive = consoleLogEnabledState()](logging::LogLevel level, const std::string& message) {
            if (!alive->load(std::memory_order_relaxed)) {
                return;
            }
            String text(message.begin(), message.end());
            const auto type = toConsoleType(level);
            if (auto* app = Application::current(); app && app->mainThreadDispatcher()) {
                app->mainThreadDispatcher()->postToMain(
                    [panel, type, text = std::move(text)]() mutable { panel->append(type, text); });
            }
        });
    logging::defaultLogger().addSink(log_sink_);

    // 注册配置项，让开关出现在配置窗口（标准“日志”分类 → “控制台”分组）。
    if (context) {
        ConfigItem item(consoleLogConfigKey(), u8"日志输出到控制台", ConfigItemType::Bool);
        item.description(u8"开启后，默认日志输出到控制台面板").defaultValue(true);
        context->registerConfigItem(StandardCategory::Logging, StandardGroup::Console, item);
    }
}

void AppShellPlugin::unload(PluginLoadContext* context)
{
    (void)context;

    if (auto* app = Application::current(); app && app->configManager() && config_change_handler_id_ != 0) {
        app->configManager()->changed.removeHandler(config_change_handler_id_);
        config_change_handler_id_ = 0;
    }
    consoleLogEnabledState()->store(false, std::memory_order_relaxed);
}

V_PLUGIN_DECLARE(AppShellPlugin, u8"app_shell", u8"1.0.0", u8"应用外壳：基础界面（插件信息与配置管理）", u8"Vine", { })

V_APPFW_NS_END
