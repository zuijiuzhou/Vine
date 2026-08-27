#pragma once

class QCoreApplication;

V_APPFW_NS_BEGIN

class PluginManager;
class CommandManager;
class ServiceManager;
class ConfigManager;
class ConfigRegistry;

struct ApplicationData {
    PluginManager*    plugin_manager  = nullptr;
    ServiceManager*   service_manager = nullptr;
    CommandManager*   command_manager = nullptr;
    ConfigManager*    config_manager  = nullptr;
    ConfigRegistry*   config_registry = nullptr;
    QCoreApplication* app             = nullptr;

    int    argc = 0;
    char** argv = nullptr;

    virtual ~ApplicationData() = default;
};

V_APPFW_NS_END
