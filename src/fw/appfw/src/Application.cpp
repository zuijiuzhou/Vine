#include <QCoreApplication>

#include <vine/Exception.hpp>
#include <vine/Ptr.hpp>

#include <vine/appfw/PluginManager.hpp>
#include <vine/appfw/Application.hpp>
#include <vine/appfw/CommandManager.hpp>
#include <vine/appfw/ConfigManager.hpp>
#include <vine/appfw/ConfigRegistry.hpp>
#include <vine/appfw/ServiceManager.hpp>

#include "ApplicationData.hpp"

V_APPFW_NS_BEGIN

static Application* s_current_app = nullptr;

V_OBJECT_META_IMPL(Application, Object)

auto Application::dptr() -> ApplicationData*
{
    return d;
}

auto Application::dptr() const -> const ApplicationData*
{
    return d;
}

Application::Application(int argc, char** argv)
  : Application(new ApplicationData(), argc, argv)
{}

Application::Application(ApplicationData* data, int argc, char** argv)
  : d(data)
{
    if (s_current_app) {
        throw Exception(-1);
    }

    s_current_app = this;

    dptr()->plugin_manager  = new PluginManager;
    dptr()->service_manager = new ServiceManager;
    dptr()->command_manager = new CommandManager(this);
    dptr()->config_manager  = new ConfigManager;
    dptr()->config_registry = new ConfigRegistry;
    dptr()->argc            = argc;
    dptr()->argv            = argv;
}

Application::~Application()
{
    delete dptr()->plugin_manager;
    delete dptr()->service_manager;
    delete dptr()->command_manager;
    delete dptr()->config_manager;
    delete dptr()->config_registry;
    delete d;
    s_current_app = nullptr;
}

void Application::init()
{
    if (dptr()->app == nullptr) {
        dptr()->app = new QCoreApplication(dptr()->argc, dptr()->argv);
    }
}

int Application::run()
{
    return dptr()->app->exec();
}

void Application::exit(int code)
{
    QCoreApplication::exit(code);
}

CommandManager* Application::commandManager() const
{
    return dptr()->command_manager;
}

PluginManager* Application::pluginManager() const
{
    return dptr()->plugin_manager;
}

ServiceManager* Application::serviceManager() const
{
    return dptr()->service_manager;
}

ConfigManager* Application::configManager() const
{
    return dptr()->config_manager;
}

ConfigRegistry* Application::configRegistry() const
{
    return dptr()->config_registry;
}

Application* Application::current()
{
    return s_current_app;
}

int Application::argc() const
{
    return dptr()->argc;
}

char** Application::argv() const
{
    return dptr()->argv;
}

V_APPFW_NS_END
