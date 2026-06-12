#pragma once

#include "appfw_global.hpp"

#include <vine/Object.hpp>
#include <vine/appfw/ApplicationData.hpp>

V_APPFW_NS_BEGIN

class CommandManager;
class AddinManager;
class ServiceManager;

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
    virtual ~Application();

  public:
    virtual void init();

  public:
    virtual int run();

    void exit(int code);

    CommandManager* commandManager() const;

    AddinManager* addinManager() const;

    ServiceManager* serviceManager() const;

    int argc() const;

    char** argv() const;

  public:
    static Application* current();
};

V_APPFW_NS_END
