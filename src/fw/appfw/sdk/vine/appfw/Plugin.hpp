#pragma once
#include "appfw_global.hpp"

#include <vector>

#include <vine/RefObject.hpp>


V_APPFW_NS_BEGIN

class PluginLoadContext;

/**
 * @brief Static metadata declared by a plugin.
 */
struct V_APPFW_API PluginInfo {
    String              name;         // Unique plugin name (identifier).
    String              version;      // Plugin version, e.g. "1.2.0".
    String              description;  // Human-readable description.
    String              vendor;       // Author or vendor.
    std::vector<String> dependencies; // Plugin names required before this one.
};

/**
 * @brief Base class of a loadable plugin.
 *
 * A plugin is an Object loaded by the PluginManager. The lifecycle is:
 * preLoad() then load() for every plugin, postLoad() for every plugin once all
 * have loaded (cross-plugin wiring), and unload() on shutdown.
 */
class V_APPFW_API Plugin : public Object {
    V_OBJECT_META_DECL;

  public:
    /**
     * @brief Declares the plugin's static metadata.
     *
     * @return The plugin metadata.
     */
    virtual PluginInfo info() const;

    /**
     * @brief Returns the plugin name (convenience for info().name).
     *
     * @return The plugin name.
     */
    String name() const;

  public:
    /**
     * @brief Called before load(); used for dependency checks and early setup.
     *
     * @param context Load context exposing host capabilities.
     */
    virtual void preLoad(PluginLoadContext* context);

    /**
     * @brief Registers the plugin's contributions (services, configs, ...).
     *
     * @param context Load context exposing host capabilities.
     */
    virtual void load(PluginLoadContext* context);

    /**
     * @brief Called after every plugin has loaded; used for cross-plugin wiring.
     *
     * @param context Load context exposing host capabilities.
     */
    virtual void postLoad(PluginLoadContext* context);

    /**
     * @brief Releases the plugin's resources on shutdown.
     *
     * @param context Load context exposing host capabilities.
     */
    virtual void unload(PluginLoadContext* context);
};

V_APPFW_NS_END
