#include <filesystem>
#include <iostream>

#include <QStandardPaths>

#include <vine/logging/Log.hpp>
#include <vine/logging/LogSink.hpp>

#include <vine/appfw/AppBuilder.hpp>
#include <vine/appfw/PluginManager.hpp>
#include <vine/appfw/gui/GuiAppBuilder.hpp>
#include <vine/appfw/gui/GuiApplication.hpp>

namespace fw    = vine::appfw;
namespace guifw = fw::gui;

int main(int argc, char** argv)
{
    // 通过 builder 构建并初始化 GUI 应用（内部会创建并显示主窗口）。
    fw::AppConfig config;
    config.name = "Vine";
    // config.plugin_dir 留空以使用默认插件搜索目录。

    auto app = guifw::createGuiApplication(config, argc, argv);

    // 日志同时输出到控制台和用户数据目录下按日期滚动的文件。
    // AppDataLocation 已包含应用名，形如 <user-data>/Vine，因此日志文件
    // 落在 <user-data>/Vine/logs/vine_2026-08-30.log（每日一文件）。
    const auto log_base = std::filesystem::path(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdU16String())
        / "logs" / "vine.log";

    ::vine::logging::initDefault(::vine::logging::LogConfig{
        .level = ::vine::logging::LogLevel::Info,
        .sinks = {
            ::vine::logging::LogSink::console(),
            ::vine::logging::LogSink::dailyFile(log_base),
        },
    });

    // 加载插件：app_shell 会在主窗口上注册 Ribbon 标签与命令。
    if (!app->pluginManager()->loadAll()) {
        std::cerr << "Some plugins failed to load" << std::endl;
    }

    return app->run();
}
