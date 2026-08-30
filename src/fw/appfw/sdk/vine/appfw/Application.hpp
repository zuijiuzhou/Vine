#pragma once

#include "appfw_global.hpp"

#include <memory>

#include <vine/Object.hpp>
#include <vine/RawPtr.hpp>

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

    RawPtr<CommandManager> commandManager() const;

    RawPtr<PluginManager> pluginManager() const;

    RawPtr<ServiceManager> serviceManager() const;

    /// Application-level config manager (single instance, lives with Application).
    RawPtr<ConfigManager> configManager() const;

    /// Config item registry (single instance, lives with Application).
    RawPtr<ConfigRegistry> configRegistry() const;

    /// In-process publish/subscribe bus (single instance, lives with Application).
    RawPtr<EventBus> eventBus() const;

    /// Main-thread marshaller used by EventBus for Main/Auto delivery.
    RawPtr<MainThreadDispatcher> mainThreadDispatcher() const;

    RawPtr<UserIO> userIO() const;

    int argc() const;

    char** argv() const;

  public:
    static RawPtr<Application> current();
};

V_APPFW_NS_END
