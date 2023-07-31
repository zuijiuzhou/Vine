#pragma once

#include "core/Inherit.h"

#include "appfw_export.h"

ETD_APPFW_NS_BEGIN
class CommandManager;
class AddinManager;
class ServiceManager;

class ETD_APPFW_API Application : public Inherit<Object, Application>
{

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
    Data *d_ptr;
};
ETD_APPFW_NS_END