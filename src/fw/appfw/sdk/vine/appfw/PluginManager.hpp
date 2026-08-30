#pragma once
#include "appfw_global.hpp"

#include <cstddef>
#include <filesystem>
#include <vector>

#include <vine/String.hpp>

V_APPFW_NS_BEGIN

struct CommandInfo;
class ConfigItem;
class Plugin;

/**
 * @brief Loads and manages plugins.
 *
 * Provides the plugin search directory configuration and loads plugins from
 * dynamic libraries. The search directory defaults to <exe>/plugins/vine and
 * can be overridden before loading.
 */
class V_APPFW_API PluginManager {
  public:
    PluginManager();
    ~PluginManager();

  public:
    /**
     * @brief Sets the directory where plugins are searched.
     *
     * Global static configuration; call before loading plugins.
     *
     * @param dir Plugin directory.
     */
    static void setPluginDirectory(const std::filesystem::path& dir);

    /**
     * @brief Returns the configured plugin search directory.
     *
     * Defaults to <exe>/plugins/vine on Windows and <appdir>/plugins/vine on
     * Linux (appdir is the directory containing the bin/ directory).
     *
     * @return The plugin directory.
     */
    static std::filesystem::path pluginDirectory();

    /**
     * @brief Sets the list of plugin names skipped by load().
     *
     * Plugins whose name is in the list are not loaded.
     *
     * @param names Plugin names to skip.
     */
    static void setSkipList(const std::vector<String>& names);

    /**
     * @brief Returns the list of plugin names skipped by load().
     *
     * Returns a const view of the internal list; no copy is made.
     *
     * @return The skip list.
     */
    static const std::vector<String>& skipList();

    /**
     * @brief Adds a plugin name to the skip list.
     *
     * @param name Plugin name to skip.
     */
    static void addToSkipList(String name);

    /**
     * @brief Removes a plugin name from the skip list.
     *
     * @param name Plugin name.
     */
    static void removeFromSkipList(const String& name);

    /**
     * @brief Returns whether a plugin name is in the skip list.
     *
     * @param name Plugin name.
     * @return true if skipped.
     */
    static bool isSkipped(const String& name);

  public:
    /**
     * @brief Loads a plugin by name or library path.
     *
     * A name is resolved against pluginDirectory() with the platform library
     * extension. The library is queried for the Vine plugin entry points and
     * the DLL-global plugin instance is created (two-step load).
     *
     * @param str Plugin name or library path.
     * @return The loaded plugin, or nullptr on failure.
     */
    Plugin* load(const String& str);

    /**
     * @brief Loads every plugin library in the plugin directory.
     *
     * Scans the plugin directory and collects the metadata of every unfiltered,
     * valid Vine plugin without creating instances yet. Dependencies are then
     * resolved against the newly discovered plugins and the already-loaded ones;
     * when every dependency is satisfiable, instances are created in dependency
     * order and the three-phase lifecycle runs: preLoad() for all plugins, then
     * load() for all plugins, then postLoad() for all plugins.
     *
     * @return false if a plugin has an unsatisfied dependency or a dependency cycle.
     */
    bool loadAll();

  public:
    /**
     * @brief Returns the loaded plugin with the given name.
     *
     * @param name Plugin name.
     * @return The plugin instance, or nullptr if not loaded.
     */
    Plugin* plugin(const String& name) const;

    /**
     * @brief Returns whether a plugin with the given name is loaded.
     *
     * @param name Plugin name.
     * @return true if loaded.
     */
    bool isLoaded(const String& name) const;

    /**
     * @brief Returns the number of loaded plugins.
     *
     * @return The plugin count.
     */
    std::size_t count() const;

    /**
     * @brief Returns the library file path of the loaded plugin.
     *
     * @param name Plugin name.
     * @return The library path, or an empty path if the plugin is not loaded.
     */
    std::filesystem::path libraryPath(const String& name) const;

    /**
     * @brief Returns the names of all loaded plugins in load order.
     *
     * @return The plugin names.
     */
    std::vector<String> names() const;

    /**
     * @brief Returns the instances of all loaded plugins in load order.
     *
     * @return The plugin instances.
     */
    std::vector<Plugin*> plugins() const;

    /**
     * @brief Reports the commands registered by the given plugin.
     *
     * Delegates to the host CommandManager; commands are attributed to a plugin
     * while it is being loaded (see CommandManager::setRegistrationOwner).
     *
     * @param name Plugin name.
     * @return The plugin's command metadata (may be empty).
     */
    std::vector<CommandInfo> commandInfosForPlugin(const String& name) const;

    /**
     * @brief Reports the config items registered by the given plugin.
     *
     * Delegates to the host ConfigRegistry; items are recorded with their owner
     * when the plugin registers them through PluginLoadContext.
     *
     * @param name Plugin name.
     * @return The plugin's config items (may be empty).
     */
    std::vector<const ConfigItem*> configItemsForPlugin(const String& name) const;

  private:
    struct Impl;
    Impl* const d;
};

V_APPFW_NS_END
