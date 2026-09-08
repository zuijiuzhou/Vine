#include "TestPlugin.hpp"

#include <vine/appfw/plugin_export.hpp>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(TestPlugin, Plugin)

TestPlugin::TestPlugin() = default;

V_DECLARE_PLUGIN(TestPlugin, u8"test_plugin", u8"测试插件", u8"1.0.0", u8"测试插件：依赖应用外壳", u8"Vine", { u8"app_shell" })

V_APPFW_NS_END
