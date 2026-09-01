#pragma once

#include "appfw_global.hpp"

#include <memory>

#include <vine/Object.hpp>
#include <vine/raw_ptr.hpp>

V_APPFW_NS_BEGIN

class CommandManager;
class PluginManager;
class ServiceManager;
class ConfigManager;
class ConfigRegistry;
class EventBus;
class MainThreadDispatcher;
class UserIO;
class ApplicationData;

class V_APPFW_API Application : public Object {
    V_OBJECT_META_DECL;

  protected:
    ApplicationData*       dptr();
    const ApplicationData* dptr() const;

    std::unique_ptr<ApplicationData> d;

  public:
    Application(int argc, char** argv);

  protected:
    Application(ApplicationData* data, int argc, char** argv);

    /**
     * @brief Creates the application's UserIO.
     *
     * The base implementation returns a headless ConsoleUserIO; GUI
     * applications override it to return a visual implementation.
     *
     * @return The newly created UserIO, owned by the application.
     */
    virtual UserIO* createUserIO();

    /**
     * @brief Creates and stores the application's UserIO.
     */
    void setupUserIO();

  public:
    ~Application() override;

  public:
    virtual void init();

  public:
    virtual int run();

    void exit(int code);

    raw_ptr<CommandManager> commandManager() const;

    raw_ptr<PluginManager> pluginManager() const;

    raw_ptr<ServiceManager> serviceManager() const;

    /// Application-level config manager (single instance, lives with Application).
    raw_ptr<ConfigManager> configManager() const;

    /// Config item registry (single instance, lives with Application).
    raw_ptr<ConfigRegistry> configRegistry() const;

    /// In-process publish/subscribe bus (single instance, lives with Application).
    raw_ptr<EventBus> eventBus() const;

    /// Main-thread marshaller used by EventBus for Main/Auto delivery.
    raw_ptr<MainThreadDispatcher> mainThreadDispatcher() const;

    raw_ptr<UserIO> userIO() const;

    int argc() const;

    char** argv() const;

    /**
     * @brief Returns whether a long-running operation is in progress.
     *
     * While busy, the framework refuses new top-level commands with a
     * "another operation is in progress" result (see CommandFlags::LongRunning)
     * and the UI may show a progress bar / disable actions.
     *
     * @return true while a progress-host-backed operation is running.
     */
    bool isBusy() const;

  public:
    static raw_ptr<Application> current();
};

V_APPFW_NS_END
