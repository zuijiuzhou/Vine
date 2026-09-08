#include <vine/appfw/AppBuilder.hpp>

#include <QCoreApplication>

#include <vine/appfw/Application.hpp>
#include <vine/appfw/PluginManager.hpp>

V_APPFW_NS_BEGIN

std::unique_ptr<Application> createApplication(const AppConfig& config, int argc, char** argv)
{
    if (!config.plugin_dir.empty()) {
        PluginManager::setPluginDirectory(config.plugin_dir);
    }

    auto app = std::make_unique<Application>(argc, argv);
    app->init();

    if (!config.name.empty()) {
        QCoreApplication::setApplicationName(QString::fromStdString(config.name));
    }

    return app;
}

V_APPFW_NS_END
