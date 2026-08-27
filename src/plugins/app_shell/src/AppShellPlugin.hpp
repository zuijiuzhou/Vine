#pragma once

#include <vine/appfw/Plugin.hpp>

V_APPFW_NS_BEGIN

/**
 * @brief App shell plugin: provides base GUI functionality (plugin info
 * viewer, configuration window) and adds two Ribbon buttons on the main
 * window to trigger them.
 */
class AppShellPlugin : public Plugin {
    V_OBJECT_META_DECL;

  public:
    AppShellPlugin();

  public:
    PluginInfo info() const override;
    void load(PluginLoadContext* context) override;
};

V_APPFW_NS_END
