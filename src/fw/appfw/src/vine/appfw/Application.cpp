#include <QCoreApplication>

#include <vine/Exception.hpp>
#include <vine/Ptr.hpp>

#include <vine/appfw/AddinManager.hpp>
#include <vine/appfw/Application.hpp>
#include <vine/appfw/CommandManager.hpp>
#include <vine/appfw/ServiceManager.hpp>

V_APPFW_NS_BEGIN

static Application* s_current_app = nullptr;

V_OBJECT_META_IMPL(Application, Object)

struct Application::Data {
    AddinManager*     addin_manager;
    ServiceManager*   service_manager;
    CommandManager*   command_manager;
    QCoreApplication* app = nullptr;

    int    argc{};
    char** argv{};
};

Application::Application(int argc, char** argv)
  : d(new Data())
{
    if (s_current_app) { throw Exception(-1); }

    s_current_app = this;

    d->addin_manager   = new AddinManager;
    d->service_manager = new ServiceManager;
    d->command_manager = new CommandManager;
    d->argc            = argc;
    d->argv            = argv;
}

Application::~Application()
{
    delete d->addin_manager;
    delete d->service_manager;
    delete d->command_manager;
    delete d;
    s_current_app = nullptr;
}

void Application::init()
{
    if (d->app == nullptr) { d->app = new QCoreApplication(d->argc, d->argv); }
}

int Application::run()
{ return d->app->exec(); }

void Application::exit(int code)
{ QCoreApplication::exit(code); }

CommandManager* Application::commandManager() const
{ return d->command_manager; }

AddinManager* Application::addinManager() const
{ return d->addin_manager; }

ServiceManager* Application::serviceManager() const
{ return d->service_manager; }

Application* Application::current()
{ return s_current_app; }

int Application::argc() const
{ return d->argc; }

char** Application::argv() const
{ return d->argv; }

V_APPFW_NS_END
