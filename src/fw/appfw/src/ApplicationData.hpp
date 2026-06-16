#pragma once

class QCoreApplication;

V_APPFW_NS_BEGIN

class AddinManager;
class CommandManager;
class ServiceManager;

struct ApplicationData {
    AddinManager*     addin_manager   = nullptr;
    ServiceManager*   service_manager = nullptr;
    CommandManager*   command_manager = nullptr;
    QCoreApplication* app             = nullptr;

    int    argc = 0;
    char** argv = nullptr;

    virtual ~ApplicationData() = default;
};

V_APPFW_NS_END
