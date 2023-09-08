#include <QCoreApplication>

#include <core/Exception.h>

#include <appfw/AddinManager.h>
#include <appfw/ServiceManager.h>
#include <appfw/CommandManager.h>
#include <appfw/Application.h>

VI_APPFW_NS_BEGIN

static Application *s_current_app = nullptr;

VI_OBJECT_META_IMPL(Application, Object)

struct Application::Data
{
    AddinManagerPtr addin_manager;
    ServiceManagerPtr service_manager;
    CommandManagerPtr command_manager;
    QCoreApplication *app = nullptr;

    int argc;
    char **argv;
};

Application::Application(int argc, char **argv)
    : d(new Data())
{
    if (s_current_app)
    {
        throw Exception(-1);
    }
    s_current_app = this;
    d->addin_manager = new AddinManager;
    d->service_manager = new ServiceManager;
    d->command_manager = new CommandManager;
    d->argc = argc;
    d->argv = argv;
}

Application::~Application()
{
    delete d;
    s_current_app = nullptr;
}

void Application::init()
{
    if (d->app == nullptr)
    {
        d->app = new QCoreApplication(d->argc, d->argv);
    }
}

int Application::run()
{
    return d->app->exec();
}

void Application::exit(int code)
{
    QCoreApplication::exit(code);
}

CommandManager *Application::commandManager() const
{
    return d->command_manager.get();
}

AddinManager *Application::addinManager() const
{
    return d->addin_manager.get();
}

ServiceManager *Application::serviceManager() const
{
    return d->service_manager.get();
}

Application *Application::current()
{
    return s_current_app;
}

int Application::argc() const
{
    return d->argc;
}

char **Application::argv() const
{
    return d->argv;
}

VI_APPFW_NS_END
