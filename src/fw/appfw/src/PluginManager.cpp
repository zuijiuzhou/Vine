#include <vine/appfw/PluginManager.hpp>

#ifdef V_CC_MSVC
#    include <Windows.h>
#endif // V_CC_MSVC

#include <algorithm>
#include <array>
#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#ifdef __linux__
#    include <unistd.h>
#endif // __linux__

#include <vine/appfw/Application.hpp>
#include <vine/appfw/CommandManager.hpp>
#include <vine/appfw/Plugin.hpp>
#include <vine/appfw/PluginLoadContext.hpp>
#include <vine/logging/Log.hpp>
#include <vine/runtime/DynamicLibraryLoader.hpp>

V_APPFW_NS_BEGIN

namespace
{

/**
 * @brief Returns the directory containing the current executable.
 */
std::filesystem::path executableDir()
{
#if defined(_WIN32)
    std::array<wchar_t, MAX_PATH> buf{};
    const DWORD len = GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
    if (len > 0 && len < buf.size()) {
        return std::filesystem::path(std::wstring(buf.data(), len)).parent_path();
    }
#elif defined(__linux__)
    std::array<char, 4096> buf{};
    const auto len = ::readlink("/proc/self/exe", buf.data(), buf.size() - 1);
    if (len > 0) {
        return std::filesystem::path(std::string(buf.data(), static_cast<std::size_t>(len))).parent_path();
    }
#endif
    return std::filesystem::current_path();
}

/**
 * @brief Returns the platform-specific default plugin directory.
 *
 * Windows: <exe>/plugins/vine. Linux: <appdir>/plugins/vine where appdir is
 * the directory containing the bin/ directory (the executable lives in bin).
 */
std::filesystem::path defaultPluginDirectory()
{
#if defined(_WIN32)
    return executableDir() / "plugins" / "vine";
#elif defined(__linux__)
    return executableDir().parent_path() / "plugins" / "vine";
#else
    return executableDir() / "plugins" / "vine";
#endif
}

/**
 * @brief Plugin search directory storage, initialized on first access.
 */
std::filesystem::path& pluginDirectoryStorage()
{
    static std::filesystem::path dir = defaultPluginDirectory();
    return dir;
}

/// Skip-list storage: plugin names that load() must not load.
std::vector<String>& skipListStorage()
{
    static std::vector<String> list;
    return list;
}

/// Returns the platform plugin library file extension.
std::filesystem::path pluginExtension()
{
#if defined(_WIN32)
    return ".dll";
#elif defined(__APPLE__)
    return ".dylib";
#else
    return ".so";
#endif
}

/// Resolves a plugin name or path to a library file.
std::filesystem::path resolvePluginPath(const String& str)
{
    const std::filesystem::path given(std::u8string_view(str.data(), str.size()));
    if (given.has_extension() && std::filesystem::exists(given)) {
        return given;
    }
    std::filesystem::path file = given;
    file += pluginExtension();
    return PluginManager::pluginDirectory() / file;
}

/// Converts a String to a std::string for fmt-based logging.
std::string toUtf8(const String& s)
{
    return std::string(reinterpret_cast<const char*>(s.data()), s.size());
}

/**
 * @brief Tags every command registered while alive with the given plugin name.
 *
 * RAII: restores the previous owner on destruction so the plugin's commands
 * (vinePluginRegisterCommands and the load lifecycle) are attributed to it.
 */
class RegistrationOwnerScope
{
  public:
    RegistrationOwnerScope(CommandManager* manager, const String& owner)
      : manager_(manager)
    {
        if (manager_ != nullptr) {
            manager_->setRegistrationOwner(owner);
        }
    }

    ~RegistrationOwnerScope()
    {
        if (manager_ != nullptr) {
            manager_->setRegistrationOwner({});
        }
    }

    RegistrationOwnerScope(const RegistrationOwnerScope&)            = delete;
    RegistrationOwnerScope& operator=(const RegistrationOwnerScope&) = delete;

  private:
    CommandManager* manager_;
};

} // namespace

/**
 * @brief A loaded plugin, its instance and the library it came from.
 */
struct LoadedPlugin {
    String                name;
    Plugin*               plugin;
    std::vector<String>   dependencies;
    std::filesystem::path path;  // Library file the plugin was loaded from.
};

/**
 * @brief A discovered plugin library whose metadata has been queried.
 *
 * The instance is not created yet; only the library is loaded so that
 * vinePluginQuery() can be resolved for dependency resolution.
 */
struct Candidate {
    std::filesystem::path          path;
    vine::runtime::DynamicLibrary* lib;
    const PluginInfo*              info;
};

struct PluginManager::Impl {
    std::vector<LoadedPlugin> plugins;
};

PluginManager::PluginManager()
  : d(new Impl())
{}

PluginManager::~PluginManager()
{
    // Loaded plugin libraries are owned by the shared DynamicLibraryLoader
    // and are unloaded when it is destroyed at process exit.
    delete d;
}

void PluginManager::setPluginDirectory(const std::filesystem::path& dir)
{
    pluginDirectoryStorage() = dir;
}

std::filesystem::path PluginManager::pluginDirectory()
{
    return pluginDirectoryStorage();
}

void PluginManager::setSkipList(const std::vector<String>& names)
{
    skipListStorage() = names;
}

const std::vector<String>& PluginManager::skipList()
{
    return skipListStorage();
}

void PluginManager::addToSkipList(String name)
{
    skipListStorage().push_back(std::move(name));
}

void PluginManager::removeFromSkipList(const String& name)
{
    auto& list = skipListStorage();
    list.erase(std::remove(list.begin(), list.end(), name), list.end());
}

bool PluginManager::isSkipped(const String& name)
{
    const auto& list = skipListStorage();
    return std::find(list.begin(), list.end(), name) != list.end();
}

Plugin* PluginManager::load(const String& str)
{
    // Skip by input name (bare plugin name case); avoids loading the library.
    if (isSkipped(str)) {
        return nullptr;
    }

    const std::filesystem::path path = resolvePluginPath(str);
    if (path.empty()) {
        return nullptr;
    }

    // Step 1: load the plugin library (cached by the shared loader).
    vine::runtime::DynamicLibrary* lib = vine::runtime::DynamicLibraryLoader::instance().load(String(path.u8string()));
    if (!lib) {
        return nullptr;
    }

    // Step 2: query — is this a Vine plugin, and what is its metadata?
    using QueryFn = const PluginInfo* ();
    const auto query = lib->resolveSymbol<QueryFn>(u8"vinePluginQuery");
    if (!query) {
        return nullptr;
    }
    const PluginInfo* info = query();
    if (!info) {
        return nullptr;
    }

    // Skip by the plugin's declared name (covers path input).
    if (isSkipped(info->name)) {
        return nullptr;
    }

    // Reuse an already-loaded plugin with the same name.
    for (const auto& lp : d->plugins) {
        if (lp.name == info->name) {
            return lp.plugin;
        }
    }

    // Step 3: create the DLL-global plugin instance.
    using CreateFn = Plugin* ();
    const auto create = lib->resolveSymbol<CreateFn>(u8"vinePluginCreate");
    if (!create) {
        return nullptr;
    }

    Plugin* plugin = create();
    if (!plugin) {
        return nullptr;
    }

    // The query entry (from V_DECLARE_PLUGIN) is the single metadata source.
    plugin->setInfo(*info);

    // Register the plugin's commands (V_DECLARE_COMMAND) inside its own module:
    // the DLL exports vinePluginRegisterCommands, which runs in the plugin's
    // code and flushes its per-module queue into the CommandManager. The owner
    // scope attributes every command registered during the plugin's load
    // (module commands + lifecycle) to this plugin.
    using RegisterFn = void(CommandManager*);
    const auto register_cmds = lib->resolveSymbol<RegisterFn>(u8"vinePluginRegisterCommands");
    Application* app = Application::current();
    CommandManager* cm = app ? app->commandManager() : nullptr;
    {
        RegistrationOwnerScope owner_scope(cm, info->name);
        if (register_cmds) {
            register_cmds(cm);
        }

        // Three-phase lifecycle, aligned with loadAll(): preLoad(), then load(),
        // then postLoad() for cross-plugin wiring.
        PluginLoadContext context(app, info->name);
        plugin->preLoad(&context);
        plugin->load(&context);
        plugin->postLoad(&context);
    }

    d->plugins.push_back(LoadedPlugin{ info->name, plugin, plugin->info().dependencies, path });
    V_LOGI("Plugin '{}' loaded from '{}'", toUtf8(info->name), path.string());
    return plugin;
}

bool PluginManager::loadAll()
{
    const std::filesystem::path dir = pluginDirectory();
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        return true;
    }

    // Step 1: scan the directory and collect the metadata of every unfiltered,
    // valid Vine plugin. No instance is created yet; only the library is loaded
    // so that vinePluginQuery() can be resolved.
    std::vector<Candidate> candidates;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_regular_file() || entry.path().extension() != pluginExtension()) {
            continue;
        }
        const std::filesystem::path path = entry.path();
        vine::runtime::DynamicLibrary* lib =
            vine::runtime::DynamicLibraryLoader::instance().load(String(path.u8string()));
        if (!lib) {
            continue;
        }
        using QueryFn = const PluginInfo* ();
        const auto query = lib->resolveSymbol<QueryFn>(u8"vinePluginQuery");
        if (!query) {
            continue; // Not a Vine plugin.
        }
        const PluginInfo* info = query();
        if (!info || isSkipped(info->name)) {
            continue;
        }
        const bool already_loaded = std::any_of(d->plugins.begin(), d->plugins.end(),
            [&info](const LoadedPlugin& lp) { return lp.name == info->name; });
        if (already_loaded) {
            continue;
        }
        candidates.push_back(Candidate{ path, lib, info });
    }

    // Step 2: dependency resolution. The satisfiable name set is the union of
    // the already-loaded plugins and the newly discovered candidates.
    std::vector<String> available;
    available.reserve(d->plugins.size() + candidates.size());
    for (const auto& lp : d->plugins) {
        available.push_back(lp.name);
    }
    for (const auto& c : candidates) {
        available.push_back(c.info->name);
    }

    // Collect every unsatisfied dependency so that they are reported as one
    // readable block instead of one log line per missing dependency.
    std::vector<std::pair<String, std::vector<String>>> missing_deps;
    for (const auto& c : candidates) {
        std::vector<String> missing;
        for (const auto& dep : c.info->dependencies) {
            if (std::find(available.begin(), available.end(), dep) == available.end()) {
                missing.push_back(dep);
            }
        }
        if (!missing.empty()) {
            missing_deps.emplace_back(c.info->name, std::move(missing));
        }
    }
    if (!missing_deps.empty()) {
        std::string report = "Plugin dependency resolution failed for " + std::to_string(missing_deps.size()) + " plugin(s):";
        for (const auto& problem : missing_deps) {
            report += "\n  Plugin '";
            report += toUtf8(problem.first);
            report += "':";
            for (const auto& dep : problem.second) {
                report += "\n    - missing dependency: ";
                report += toUtf8(dep);
            }
        }
        V_LOGE("{}", report);
        return false;
    }

    // Step 3: topological sort of the candidates so that every plugin comes
    // after its dependencies. Already-loaded plugins are satisfied roots and do
    // not appear in the order.
    std::map<String, std::size_t>         indegree;
    std::map<String, std::vector<String>> dependents;
    for (const auto& c : candidates) {
        indegree[c.info->name] = 0;
        for (const auto& dep : c.info->dependencies) {
            const bool is_candidate = std::any_of(candidates.begin(), candidates.end(),
                [&dep](const Candidate& other) { return other.info->name == dep; });
            if (is_candidate) {
                dependents[dep].push_back(c.info->name);
                ++indegree[c.info->name];
            }
        }
    }
    std::vector<String> order;
    std::vector<String> ready;
    for (const auto& pair : indegree) {
        if (pair.second == 0) {
            ready.push_back(pair.first);
        }
    }
    while (!ready.empty()) {
        const String name = ready.back();
        ready.pop_back();
        order.push_back(name);
        for (const auto& dependent : dependents[name]) {
            if (--indegree[dependent] == 0) {
                ready.push_back(dependent);
            }
        }
    }
    if (order.size() != candidates.size()) {
        // Plugins that could not be ordered are involved in a cycle; list them.
        std::string report = "Plugin dependency cycle detected among:";
        for (const auto& c : candidates) {
            if (std::find(order.begin(), order.end(), c.info->name) == order.end()) {
                report += "\n  - ";
                report += toUtf8(c.info->name);
            }
        }
        V_LOGE("{}", report);
        return false;
    }

    // Step 4: create the instances in dependency order.
    std::vector<LoadedPlugin> created;
    created.reserve(order.size());
    for (const auto& name : order) {
        const auto it = std::find_if(candidates.begin(), candidates.end(),
            [&name](const Candidate& c) { return c.info->name == name; });
        using CreateFn = Plugin* ();
        const auto create = it->lib->resolveSymbol<CreateFn>(u8"vinePluginCreate");
        if (!create) {
            V_LOGE("Plugin '{}' has no create entry point", toUtf8(name));
            return false;
        }
        Plugin* plugin = create();
        if (!plugin) {
            V_LOGE("Plugin '{}' failed to create its instance", toUtf8(name));
            return false;
        }

        // The query entry (from V_DECLARE_PLUGIN) is the single metadata source.
        plugin->setInfo(*it->info);

        // Register the plugin's commands (V_DECLARE_COMMAND) inside its own module.
        // The owner scope tags these module commands with the plugin name.
        using RegisterFn = void(CommandManager*);
        const auto register_cmds = it->lib->resolveSymbol<RegisterFn>(u8"vinePluginRegisterCommands");
        Application* app = Application::current();
        CommandManager* cm = app ? app->commandManager() : nullptr;
        {
            RegistrationOwnerScope owner_scope(cm, name);
            if (register_cmds) {
                register_cmds(cm);
            }
        }

        V_LOGI("Plugin '{}' loaded", toUtf8(name));
        created.push_back(LoadedPlugin{ name, plugin, plugin->info().dependencies, it->path });
    }

    // Step 5: three-phase lifecycle - preLoad() for every plugin (each one
    // registers its own commands), then load() for every plugin, then
    // postLoad() for every plugin. Each plugin gets its own context so it can
    // query pluginName() and its own registered configs. Every phase runs
    // inside a per-plugin owner scope so lifecycle-registered commands are
    // attributed to the plugin.
    for (const auto& lp : created) {
        RegistrationOwnerScope owner_scope(Application::current() ? Application::current()->commandManager() : nullptr, lp.name);
        PluginLoadContext context(Application::current(), lp.name);
        lp.plugin->preLoad(&context);
    }
    for (const auto& lp : created) {
        RegistrationOwnerScope owner_scope(Application::current() ? Application::current()->commandManager() : nullptr, lp.name);
        PluginLoadContext context(Application::current(), lp.name);
        lp.plugin->load(&context);
    }
    for (const auto& lp : created) {
        RegistrationOwnerScope owner_scope(Application::current() ? Application::current()->commandManager() : nullptr, lp.name);
        PluginLoadContext context(Application::current(), lp.name);
        lp.plugin->postLoad(&context);
    }

    // Register the created plugins (in dependency order).
    for (auto& lp : created) {
        d->plugins.push_back(std::move(lp));
    }
    return true;
}

Plugin* PluginManager::plugin(const String& name) const
{
    for (const auto& lp : d->plugins) {
        if (lp.name == name) {
            return lp.plugin;
        }
    }
    return nullptr;
}

bool PluginManager::isLoaded(const String& name) const
{
    return plugin(name) != nullptr;
}

std::size_t PluginManager::count() const
{
    return d->plugins.size();
}

std::filesystem::path PluginManager::libraryPath(const String& name) const
{
    for (const auto& lp : d->plugins) {
        if (lp.name == name) {
            return lp.path;
        }
    }
    return {};
}

std::vector<String> PluginManager::names() const
{
    std::vector<String> result;
    result.reserve(d->plugins.size());
    for (const auto& lp : d->plugins) {
        result.push_back(lp.name);
    }
    return result;
}

std::vector<Plugin*> PluginManager::plugins() const
{
    std::vector<Plugin*> result;
    result.reserve(d->plugins.size());
    for (const auto& lp : d->plugins) {
        result.push_back(lp.plugin);
    }
    return result;
}

std::vector<CommandInfo> PluginManager::commandInfosForPlugin(const String& name) const
{
    auto* plugin = this->plugin(name);
    return plugin ? plugin->commandInfos() : std::vector<CommandInfo>{};
}

std::vector<const ConfigItem*> PluginManager::configItemsForPlugin(const String& name) const
{
    auto* plugin = this->plugin(name);
    return plugin ? plugin->configItems() : std::vector<const ConfigItem*>{};
}

V_APPFW_NS_END
