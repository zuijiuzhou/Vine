#pragma once

class QCoreApplication;

V_APPFW_NS_BEGIN

class PluginManager;
class CommandManager;
class ServiceManager;
class ConfigManager;
class ConfigRegistry;
class EventBus;
class MainThreadDispatcher;

struct ApplicationData {
    PluginManager*        plugin_manager  = nullptr;
    ServiceManager*       service_manager = nullptr;
    CommandManager*       command_manager = nullptr;
    ConfigManager*        config_manager  = nullptr;
    ConfigRegistry*       config_registry = nullptr;
    EventBus*             event_bus       = nullptr;
    MainThreadDispatcher* main_dispatcher = nullptr;
    QCoreApplication*     app             = nullptr;

    int    argc = 0;
    char** argv = nullptr;

    virtual ~ApplicationData() = default;
};

V_APPFW_NS_END
