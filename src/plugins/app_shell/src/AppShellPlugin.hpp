#pragma once

#include <vine/appfw/Plugin.hpp>

V_APPFW_NS_BEGIN

/**
 * @brief App shell plugin: provides base GUI functionality (plugin info
 * viewer, configuration window) and adds two Ribbon buttons on the main
 * window to trigger them.
 *
 * The plugin is a thin orchestrator: the Ribbon/workspace UI is built by
 * AppShellUi, the console-log routing (config item, config sync, sink) is
 * owned by ConsoleLogRouter.
 */
class AppShellPlugin : public Plugin {
    V_OBJECT_META_DECL;

  public:
    AppShellPlugin();

  public:
    void load(PluginLoadContext* context) override;
    void unload(PluginLoadContext* context) override;
};

V_APPFW_NS_END
