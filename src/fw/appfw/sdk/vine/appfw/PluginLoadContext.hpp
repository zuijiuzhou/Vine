#pragma once

#include "appfw_global.hpp"

V_APPFW_NS_BEGIN

class Application;
class CommandManager;
class ConfigRegistry;

/**
 * @brief Plugin load context: passed to Plugin::load(), exposing host capabilities.
 *
 * Inside load(), the plugin obtains the config registry via configs() and the
 * command manager via commandManager() to register its commands. More accessors
 * such as serviceManager() can be added later.
 */
class V_APPFW_API PluginLoadContext {
  public:
    /// Constructs the load context with Application as the host.
    explicit PluginLoadContext(Application* app);
    ~PluginLoadContext();

    /// Config registry: plugins register config items (ConfigItem) here.
    ConfigRegistry* configs() const;

    /**
     * @brief Command manager: plugins register their commands here during load().
     *
     * @return The command manager, or nullptr if no Application is set.
     */
    CommandManager* commandManager() const;

  private:
    struct Data;
    Data* const d;
};

V_APPFW_NS_END
