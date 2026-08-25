#pragma once

#include "appfw_global.hpp"

#include <vine/Object.hpp>

V_APPFW_NS_BEGIN

class CommandManager;
class AddinManager;
class ServiceManager;
class ConfigManager;
class ConfigRegistry;
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

    /// 应用级配置管理器（唯一实例，随 Application 生命周期）。
    ConfigManager* configManager() const;

    /// 配置项注册表（唯一实例，随 Application 生命周期）。
    ConfigRegistry* configRegistry() const;

    int argc() const;

    char** argv() const;

  public:
    static Application* current();
};

V_APPFW_NS_END
