#include <vine/appfw/PluginLoadContext.hpp>

#include <vine/appfw/Application.hpp>
#include <vine/appfw/ConfigRegistry.hpp>

V_APPFW_NS_BEGIN

struct PluginLoadContext::Data {
    Application* app = nullptr;
};

PluginLoadContext::PluginLoadContext(Application* app)
    : d(new Data)
{
    d->app = app;
}

PluginLoadContext::~PluginLoadContext()
{
    delete d;
}

ConfigRegistry* PluginLoadContext::configs() const
{
    return d->app ? d->app->configRegistry() : nullptr;
}

CommandManager* PluginLoadContext::commandManager() const
{
    return d->app ? d->app->commandManager() : nullptr;
}

V_APPFW_NS_END
