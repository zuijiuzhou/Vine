#pragma once

#include "appfw_global.hpp"

V_APPFW_NS_BEGIN

class Application;
class CommandManager;
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
    /// Constructs the load context with Application as the host.
    explicit PluginLoadContext(Application* app);
    ~PluginLoadContext();

    /**
     * @brief The host Application.
     *
     * @return The Application this context was created for, or nullptr if none.
     */
    Application* application() const;

    /**
     * @brief Config registry: plugins register config items (ConfigItem) here.
     */
    ConfigRegistry* configs() const;

    /**
     * @brief Command manager: plugins register their commands here during load().
     *
     * @return The command manager, or nullptr if no Application is set.
     */
    CommandManager* commandManager() const;

    /**
     * @brief The host publish/subscribe bus.
     *
     * @return The EventBus owned by the host Application, or nullptr if none.
     */
    EventBus* eventBus() const;

  private:
    struct Data;
    Data* const d;
};

V_APPFW_NS_END
