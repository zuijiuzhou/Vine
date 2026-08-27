#pragma once

#include <vine/appfw/Plugin.hpp>

V_APPFW_NS_BEGIN

/**
 * @brief Test plugin that depends on the plugin_manager plugin.
 */
class TestPlugin : public Plugin {
    V_OBJECT_META_DECL;

  public:
    TestPlugin();

  public:
    PluginInfo info() const override;
};

V_APPFW_NS_END
