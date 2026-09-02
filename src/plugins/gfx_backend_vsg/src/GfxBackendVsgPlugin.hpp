#pragma once

#include <vine/appfw/Plugin.hpp>
#include <vine/vsg/vsg_global.hpp>

V_APPFW_NS_BEGIN
class PluginLoadContext;
V_APPFW_NS_END

V_VSG_NS_BEGIN

/**
 * @brief appfw plugin exposing the VSG render backend.
 *
 * On load() the plugin registers the "vsg" backend factory into
 * vine::graphics::RenderBackendRegistry, making the backend creatable by name
 * from anywhere in the application without a compile-time dependency on this
 * plugin or on VulkanSceneGraph.
 */
class V_VSG_API GfxBackendVsgPlugin : public vine::appfw::Plugin {
    V_OBJECT_META_DECL;

  public:
    GfxBackendVsgPlugin();
    ~GfxBackendVsgPlugin() override;

  public:
    /** @brief Registers the VSG backend factory into the registry. */
    void load(vine::appfw::PluginLoadContext* context) override;

    /** @brief Removes the backend registration on unload. */
    void unload(vine::appfw::PluginLoadContext* context) override;
};

V_VSG_NS_END
