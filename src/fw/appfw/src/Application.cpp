#include <QCoreApplication>

#include <vine/Exception.hpp>
#include <vine/Ptr.hpp>

#include <vine/appfw/AddinManager.hpp>
#include <vine/appfw/Application.hpp>
#include <vine/appfw/CommandManager.hpp>
#include <vine/appfw/ServiceManager.hpp>

#include "ApplicationData.hpp"

V_APPFW_NS_BEGIN

static Application* s_current_app = nullptr;

V_OBJECT_META_IMPL(Application, Object)

inline auto Application::dptr() -> ApplicationData* { return static_cast<ApplicationData*>(d); }
inline auto Application::dptr() const -> const ApplicationData* { return static_cast<const ApplicationData*>(d); }

Application::Application(int argc, char** argv)
    : Application(new ApplicationData(), argc, argv)
{}

Application::Application(ApplicationData* data, int argc, char** argv)
  : d(data)
{
    if (s_current_app) { throw Exception(-1); }

    s_current_app = this;

    dptr()->addin_manager   = new AddinManager;
    dptr()->service_manager = new ServiceManager;
    dptr()->command_manager = new CommandManager;
    dptr()->argc            = argc;
    dptr()->argv            = argv;
}

Application::~Application()
{
    delete dptr()->addin_manager;
    delete dptr()->service_manager;
    delete dptr()->command_manager;
    delete d;
    s_current_app = nullptr;
}

void Application::init()
{
    if (dptr()->app == nullptr) { dptr()->app = new QCoreApplication(dptr()->argc, dptr()->argv); }
}

int Application::run()
{ return dptr()->app->exec(); }

void Application::exit(int code)
{ QCoreApplication::exit(code); }

CommandManager* Application::commandManager() const
{ return dptr()->command_manager; }

AddinManager* Application::addinManager() const
{ return dptr()->addin_manager; }

ServiceManager* Application::serviceManager() const
{ return dptr()->service_manager; }

Application* Application::current()
{ return s_current_app; }

int Application::argc() const
{ return dptr()->argc; }

char** Application::argv() const
{ return dptr()->argv; }

V_APPFW_NS_END
