#include <vine/appfw/Plugin.hpp>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(Plugin, Object)

PluginInfo Plugin::info() const
{
    return {};
}

String Plugin::name() const
{
    return info().name;
}

void Plugin::preLoad(PluginLoadContext* context)
{
    // Commands (V_DECLARE_COMMAND) are registered by the PluginManager through
    // the plugin DLL's vinePluginRegisterCommands entry, which runs inside the
    // plugin module and flushes its per-module queue.
    (void)context;
}

void Plugin::load(PluginLoadContext* context)
{}

void Plugin::postLoad(PluginLoadContext* context)
{}

void Plugin::unload(PluginLoadContext* context)
{}

V_APPFW_NS_END
