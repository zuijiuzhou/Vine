#pragma once

#include "appfw_global.hpp"

#include <memory>
#include <vector>

#include <vine/RawPtr.hpp>
#include <vine/String.hpp>

#include "ConfigStandard.hpp"

V_APPFW_NS_BEGIN

class Application;
class CommandManager;
class ConfigItem;
class ConfigRegistry;
class EventBus;

/**
 * @brief Plugin load context: passed to Plugin::load(), exposing host capabilities.
 *
 * Inside load(), the plugin obtains the config registry via configs(), the
 * command manager via commandManager(), or the host Application via
 * application(). More specific accessors can be added later.
 */
class V_APPFW_API PluginLoadContext {
  public:
    /**
     * @brief Constructs the load context with Application as the host.
     *
     * @param app Host application.
     * @param plugin_name Name of the plugin this context belongs to.
     */
    explicit PluginLoadContext(Application* app, String plugin_name = {});
    ~PluginLoadContext();

    /**
     * @brief The host Application.
     *
     * @return The Application this context was created for, or nullptr if none.
     */
    RawPtr<Application> application() const;

    /**
     * @brief Config registry: plugins register config items (ConfigItem) here.
     */
    RawPtr<ConfigRegistry> configs() const;

    /**
     * @brief Command manager: plugins register their commands here during load().
     *
     * @return The command manager, or nullptr if no Application is set.
     */
    RawPtr<CommandManager> commandManager() const;

    /**
     * @brief The host publish/subscribe bus.
     *
     * @return The EventBus owned by the host Application, or nullptr if none.
     */
    RawPtr<EventBus> eventBus() const;

    /**
     * @brief Name of the plugin this context belongs to.
     *
     * @return The plugin name.
     */
    const String& pluginName() const;

    /**
     * @brief Registers a config item under a standard category/group, owned by
     * this plugin.
     *
     * @param cat Standard category.
     * @param grp Standard group.
     * @param item Item descriptor.
     * @return true if registered, false if the key already exists.
     */
    bool registerConfigItem(StandardCategory cat, StandardGroup grp, const ConfigItem& item);

    /**
     * @brief Config items registered by this plugin.
     *
     * @return The plugin's registered items.
     */
    std::vector<const ConfigItem*> registeredConfigs() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

V_APPFW_NS_END
