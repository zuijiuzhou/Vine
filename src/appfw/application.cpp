#include <QCoreApplication>

#include <appfw/AddinManager.h>
#include <appfw/ServiceManager.h>
#include <appfw/CommandManager.h>
#include <appfw/Application.h>

ETD_APPFW_NS_BEGIN

static Application *s_current_app = nullptr;

struct Application::Data
{
    AddinManager::Ptr addin_manager;
    ServiceManager::Ptr service_manager;
    CommandManager::Ptr command_manager;
    QCoreApplication *app = nullptr;

    int argc;
    char **argv;
};

Application::Application(int argc, char **argv)
    : d_ptr(new Data())
{
    if (s_current_app)
    {
        throw std::exception("");
    }
    s_current_app = this;
    d_ptr->addin_manager = AddinManager::create();
    d_ptr->service_manager = ServiceManager::create();
    d_ptr->command_manager = CommandManager::create();
    d_ptr->argc = argc;
    d_ptr->argv = argv;
}

Application::~Application()
{
    delete d_ptr;
    s_current_app = nullptr;
}

void Application::init()
{
    if (d_ptr->app == nullptr)
    {
        d_ptr->app = new QCoreApplication(d_ptr->argc, d_ptr->argv);
    }
}

int Application::run()
{
    return d_ptr->app->exec();
}

void Application::exit(int code)
{
    QCoreApplication::exit(code);
}

CommandManager *Application::commandManager() const
{
    return d_ptr->command_manager.get();
}

AddinManager *Application::addinManager() const
{
    return d_ptr->addin_manager.get();
}

ServiceManager *Application::serviceManager() const
{
    return d_ptr->service_manager.get();
}

Application *Application::current()
{
    return s_current_app;
}

int Application::argc() const
{
    return d_ptr->argc;
}

char **Application::argv() const
{
    return d_ptr->argv;
}

ETD_APPFW_NS_END