#pragma once

#include <vine/appfw/Plugin.hpp>

#include <vine/logging/LogSink.hpp>

#include <cstddef>

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
    void unload(PluginLoadContext* context) override;

  private:
    /// Sink forwarding default-logger records to the console panel; kept alive
    /// for the plugin's lifetime.
    logging::LogSink log_sink_;

    /// Handler id of the ConfigManager::changed subscription (0 = unset).
    std::size_t config_change_handler_id_{ 0 };
};

V_APPFW_NS_END
