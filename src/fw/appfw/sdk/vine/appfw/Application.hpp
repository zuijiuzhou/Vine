#pragma once

#include "appfw_global.hpp"

#include <vine/Object.hpp>

V_APPFW_NS_BEGIN

class CommandManager;
class AddinManager;
class ServiceManager;
class ConfigManager;
class ConfigRegistry;
class UserIO;
class ApplicationData;

class V_APPFW_API Application : public Object {
    V_OBJECT_META_DECL;

  protected:
    ApplicationData*       dptr();
    const ApplicationData* dptr() const;

    struct Data;
    ApplicationData* const d;

  public:
    Application(int argc, char** argv);

  protected:
    Application(ApplicationData* data, int argc, char** argv);

  public:
    ~Application() override;

  public:
    virtual void init();

  public:
    virtual int run();

    void exit(int code);

    CommandManager* commandManager() const;

    AddinManager* addinManager() const;

    ServiceManager* serviceManager() const;

    /// Application-level config manager (single instance, lives with Application).
    ConfigManager* configManager() const;

    /// Config item registry (single instance, lives with Application).
    ConfigRegistry* configRegistry() const;

    UserIO* userIO() const;

    int argc() const;

    char** argv() const;

  public:
    static Application* current();
};

V_APPFW_NS_END
