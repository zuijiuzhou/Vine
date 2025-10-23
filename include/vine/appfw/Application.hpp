#pragma once

#include "appfw_global.hpp"

#include <vine/core/RefObject.hpp>

VI_APPFW_NS_BEGIN
class CommandManager;
class AddinManager;
class ServiceManager;

class VI_APPFW_API Application : public RefObject {
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(Application);

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
    VI_OBJECT_DATA
};
using ApplicationPtr = RefPtr<Application>;

VI_APPFW_NS_END
