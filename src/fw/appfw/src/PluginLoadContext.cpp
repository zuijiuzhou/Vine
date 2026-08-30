#include <vine/appfw/PluginLoadContext.hpp>

#include <vine/appfw/Application.hpp>
#include <vine/appfw/ConfigRegistry.hpp>

V_APPFW_NS_BEGIN

struct PluginLoadContext::Impl {
    Application* app = nullptr;
};

PluginLoadContext::PluginLoadContext(Application* app)
    : d(new Impl)
{
    d->app = app;
}

PluginLoadContext::~PluginLoadContext()
{
    delete d;
}

Application* PluginLoadContext::application() const
{
    return d->app;
}

ConfigRegistry* PluginLoadContext::configs() const
{
    return d->app ? d->app->configRegistry() : nullptr;
}

CommandManager* PluginLoadContext::commandManager() const
{
    return d->app ? d->app->commandManager() : nullptr;
}

EventBus* PluginLoadContext::eventBus() const
{
    return d->app ? d->app->eventBus() : nullptr;
}

V_APPFW_NS_END
