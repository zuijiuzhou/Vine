#include <vine/appfw/PluginLoadContext.hpp>

#include <utility>

#include <vine/appfw/Application.hpp>
#include <vine/appfw/ConfigRegistry.hpp>

V_APPFW_NS_BEGIN

struct PluginLoadContext::Impl {
    Application* app = nullptr;
    String       plugin_name;
};

PluginLoadContext::PluginLoadContext(Application* app, String plugin_name)
  : d(new Impl)
{
    d->app         = app;
    d->plugin_name = std::move(plugin_name);
}

PluginLoadContext::~PluginLoadContext() = default;

RawPtr<Application> PluginLoadContext::application() const
{
    return d->app;
}

RawPtr<ConfigRegistry> PluginLoadContext::configs() const
{
    return d->app ? d->app->configRegistry() : nullptr;
}

RawPtr<CommandManager> PluginLoadContext::commandManager() const
{
    return d->app ? d->app->commandManager() : nullptr;
}

RawPtr<EventBus> PluginLoadContext::eventBus() const
{
    return d->app ? d->app->eventBus() : nullptr;
}

const String& PluginLoadContext::pluginName() const
{
    return d->plugin_name;
}

bool PluginLoadContext::registerConfigItem(StandardCategory cat, StandardGroup grp, const ConfigItem& item)
{
    auto* reg = configs();
    return reg ? reg->addItem(cat, grp, item, d->plugin_name) : false;
}

std::vector<const ConfigItem*> PluginLoadContext::registeredConfigs() const
{
    auto* reg = configs();
    return reg ? reg->itemsForPlugin(d->plugin_name) : std::vector<const ConfigItem*>{};
}

V_APPFW_NS_END
