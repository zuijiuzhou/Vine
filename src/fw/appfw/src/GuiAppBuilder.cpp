#include <vine/appfw/gui/GuiAppBuilder.hpp>

#include <QCoreApplication>

#include <vine/appfw/gui/GuiApplication.hpp>
#include <vine/appfw/PluginManager.hpp>

V_APPFWGUI_NS_BEGIN

std::unique_ptr<GuiApplication> createGuiApplication(const AppConfig& config, int argc, char** argv)
{
    if (!config.plugin_dir.empty()) {
        PluginManager::setPluginDirectory(config.plugin_dir);
    }

    auto app = std::make_unique<GuiApplication>(argc, argv);
    app->init();

    if (!config.name.empty()) {
        QCoreApplication::setApplicationName(QString::fromStdString(config.name));
    }

    return app;
}

V_APPFWGUI_NS_END
