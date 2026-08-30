#pragma once
#include "appfw_global.hpp"

#include <vector>

#include <vine/RefObject.hpp>


V_APPFW_NS_BEGIN

class PluginLoadContext;
class ConfigItem;
struct CommandInfo;

/**
 * @brief Static metadata declared by a plugin.
 */
struct V_APPFW_API PluginInfo {
    String              name;         // Unique plugin name (identifier).
    String              display_name; // Human-friendly name shown in UI; falls back to name when empty.
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
     * @brief Returns the plugin's static metadata.
     *
     * Populated by the PluginManager from the plugin's vinePluginQuery() entry
     * when the plugin is created; V_DECLARE_PLUGIN is the single source of the
     * metadata.
     *
     * @return The plugin metadata.
     */
    const PluginInfo& info() const;

    /**
     * @brief Returns the plugin name (convenience for info().name).
     *
     * @return The plugin name.
     */
    String name() const;

    /**
     * @brief Reports the commands registered by this plugin.
     *
     * Queries the host CommandManager for commands attributed to this plugin
     * (see CommandManager::setRegistrationOwner).
     *
     * @return The plugin's command metadata (may be empty).
     */
    std::vector<CommandInfo> commandInfos() const;

    /**
     * @brief Reports the config items registered by this plugin.
     *
     * Queries the host ConfigRegistry for items owned by this plugin.
     *
     * @return The plugin's config items (may be empty).
     */
    std::vector<const ConfigItem*> configItems() const;

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

  private:
    friend class PluginManager;

    /**
     * @brief Sets the plugin's static metadata.
     *
     * Called by the PluginManager when the plugin is created (from the plugin's
     * vinePluginQuery() entry); plugin authors do not call this.
     *
     * @param info Plugin metadata.
     */
    void setInfo(PluginInfo info);

    /// Static metadata set by the PluginManager at load time.
    PluginInfo info_;
};

V_APPFW_NS_END
