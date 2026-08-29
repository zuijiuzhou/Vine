#include <iostream>

#include <vine/logging/Log.hpp>

#include <vine/appfw/AppBuilder.hpp>
#include <vine/appfw/PluginManager.hpp>
#include <vine/appfw/gui/GuiAppBuilder.hpp>
#include <vine/appfw/gui/GuiApplication.hpp>

namespace fw    = vine::appfw;
namespace guifw = fw::gui;

int main(int argc, char** argv)
{
    // 初始化日志（默认 console + Info；可按需调整级别/sink）
    ::vine::logging::initDefault();

    // 通过 builder 构建并初始化 GUI 应用（内部会创建并显示主窗口）。
    fw::AppConfig config;
    config.name = "Vine";
    // config.plugin_dir 留空以使用默认插件搜索目录。

    auto app = guifw::createGuiApplication(config, argc, argv);

    // 加载插件：app_shell 会在主窗口上注册 Ribbon 标签与命令。
    if (!app->pluginManager()->loadAll()) {
        std::cerr << "Some plugins failed to load" << std::endl;
    }

    return app->run();
}
