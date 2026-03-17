#pragma once

#include "appfw_global.hpp"

#include <vine/RefObject.hpp>

V_APPFW_NS_BEGIN
class CommandManager;
class AddinManager;
class ServiceManager;

class V_APPFW_API Application : public RefObject {
    V_OBJECT_META_DECL;

  public:
    Application(int argc, char** argv);
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

  private:
    struct Data;
    Data* const d;
};

using ApplicationPtr = RefPtr<Application>;

V_APPFW_NS_END
