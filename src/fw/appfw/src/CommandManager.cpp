#include <vine/appfw/CommandManager.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <stop_token>
#include <utility>
#include <vector>

#include <vine/appfw/Application.hpp>
#include <vine/appfw/Command.hpp>

#include <vine/async/Cancellation.hpp>
#include <vine/async/DetachedTask.hpp>
#include <vine/async/SyncWait.hpp>

#include <vine/logging/Log.hpp>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(CommandExecutingEventArgs, EventArgs)

CommandExecutingEventArgs::CommandExecutingEventArgs(Command* command)
  : command_(command)
{}

Command* CommandExecutingEventArgs::command() const
{
    return command_;
}

V_OBJECT_META_IMPL(CommandExecutedEventArgs, EventArgs)

CommandExecutedEventArgs::CommandExecutedEventArgs(Command* command, const CommandResult& result)
  : command_(command)
  , result_(result)
{}

Command* CommandExecutedEventArgs::command() const
{
    return command_;
}

const CommandResult& CommandExecutedEventArgs::result() const
{
    return result_;
}

namespace
{

/**
 * @brief A registered command: its meta class, a no-arg factory and the
 * plugin that registered it (empty for host-app commands).
 */
struct RegisteredCommand
{
    Type                      class_type;
    std::function<Command*()> factory;
    String                    owner;
};

/// Converts a String to a std::string for fmt-based logging.
std::string toUtf8(const String& s)
{
    return std::string(reinterpret_cast<const char*>(s.data()), s.size());
}

} // namespace

/**
 * @brief Private data of CommandManager.
 */
struct CommandManager::Impl
{
    /**
     * @brief Resolves a command name through the alias map.
     *
     * @param name Command or alias name.
     * @return The canonical command name.
     */
    String resolveName(const String& name) const;

    /// Application that owns this manager (non-owning).
    Application* app;

    /// Execution call stack: commands currently running (non-owning).
    std::vector<Command*> stack;

    /// Execution history: commands that ran (non-owning, oldest first).
    std::vector<Command*> history;

    /// Registered commands by name (factory + meta class).
    std::map<String, RegisteredCommand> registry;

    /// Owner tag applied to commands registered while it is non-empty.
    String registration_owner;

    /// Snapshot handler invoked before an Undoable command runs (document layer).
    std::function<void()> snapshot_handler;

    /// Command aliases (alias -> canonical command name).
    std::map<String, String> aliases;

    /// Cancellation source for the currently running command chain.
    std::stop_source stop_source;
};

String CommandManager::Impl::resolveName(const String& name) const
{
    // A registered command name wins over an alias with the same name.
    if (registry.find(name) != registry.end()) {
        return name;
    }

    auto it = aliases.find(name);
    if (it != aliases.end()) {
        return it->second;
    }
    return name;
}

/**
 * @brief Private implementation of CommandExecutionContext.
 */
class CommandManager::Context : public CommandExecutionContext
{
  public:
    Context(Application* app, std::stop_token token)
      : app_(app)
      , token_(std::move(token))
    {}

    Application* application() const override
    {
        return app_;
    }

    std::stop_token stopToken() const override
    {
        return token_;
    }

    bool isCancelled() const override
    {
        return token_.stop_requested();
    }

  private:
    Application*   app_;
    std::stop_token token_;
};

CommandManager::CommandManager(Application* app)
  : d(new Impl{ app, {}, {}, {}, {} })
{}

CommandManager::~CommandManager()
{
    delete d;
}

Application* CommandManager::application() const
{
    return d->app;
}

CommandResult CommandManager::executeCommand(Command* command)
{
    return vine::async::syncWait(executeCommandAsync(command));
}

CommandResult CommandManager::executeCommand(const String& name)
{
    return vine::async::syncWait(executeCommandAsync(name));
}

vine::async::Task<CommandResult> CommandManager::executeCommandAsync(Command* command)
{
    if (!command) {
        co_return CommandResult(CommandStatus::Failed, String(u8"Command is null"));
    }

    // Exclusive commands cancel the running command chain before execution.
    if (static_cast<std::uint32_t>(command->flags()) & static_cast<std::uint32_t>(CommandFlags::Exclusive)) {
        d->stack.clear();
    }

    // A fresh cancellation source starts each command chain; nested commands
    // share the token so cancelCurrent() cancels the whole chain.
    if (d->stack.empty()) {
        d->stop_source = std::stop_source{};
    }
    d->stack.push_back(command);

    V_LOGI("Executing command '{}'", toUtf8(command->name()));

    // Pops the stack when the command completes, keeping the manager usable.
    // The pop is skipped when an Exclusive child already cleared the stack
    // (this command was cancelled and is no longer on top). When the chain
    // drains, the cancellation source is reset for the next chain.
    struct StackGuard {
        std::vector<Command*>& stack;
        std::stop_source&      stop_source;
        Command*               command;

        ~StackGuard()
        {
            if (!stack.empty() && stack.back() == command) {
                stack.pop_back();
                if (stack.empty()) {
                    stop_source = std::stop_source{};
                }
            }
        }
    } guard{ d->stack, d->stop_source, command };

    Context context(d->app, d->stop_source.get_token());

    // Undoable commands notify the document to snapshot its state first.
    if (d->snapshot_handler && (static_cast<std::uint32_t>(command->flags()) & static_cast<std::uint32_t>(CommandFlags::Undoable))) {
        d->snapshot_handler();
    }

    {
        CommandExecutingEventArgs args(command);
        executing.trigger(*this, args);
    }

    CommandResult result;
    try {
        result = co_await command->execute(&context);
    }
    catch (const vine::async::TaskCancelledException&) {
        // Nested commands propagate the cancellation upward by default; only the
        // outermost command reports it as a Cancelled result. A command that
        // wants to handle a cancelled child catches the exception in its own
        // execute().
        if (d->stack.size() > 1) {
            throw;
        }
        result = CommandResult(CommandStatus::Cancelled, String(u8"命令已取消"));
    }

    if (result.succeeded()) {
        V_LOGI("Command '{}' succeeded", toUtf8(command->name()));
    } else if (result.status() == CommandStatus::Cancelled) {
        V_LOGW("Command '{}' cancelled", toUtf8(command->name()));
    } else {
        V_LOGE("Command '{}' failed: {}", toUtf8(command->name()), toUtf8(result.message()));
    }

    {
        CommandExecutedEventArgs args(command, result);
        executed.trigger(*this, args);
    }

    // Record the execution.
    d->history.push_back(command);

    co_return result;
}

vine::async::Task<CommandResult> CommandManager::executeCommandAsync(const String& name)
{
    auto it = d->registry.find(d->resolveName(name));
    if (it == d->registry.end() || !it->second.factory) {
        co_return CommandResult(CommandStatus::Failed, String(u8"Command not registered"));
    }

    std::unique_ptr<Command> command(it->second.factory());
    co_return co_await executeCommandAsync(command.get());
}

void CommandManager::executeDetached(const String& name)
{
    [](vine::async::Task<CommandResult> task) -> vine::async::DetachedTask {
        co_await std::move(task);
    }(executeCommandAsync(name));
}

Command* CommandManager::currentCommand() const
{
    return d->stack.empty() ? nullptr : d->stack.back();
}

int CommandManager::runningCount() const
{
    return static_cast<int>(d->stack.size());
}

void CommandManager::cancelCurrent()
{
    d->stop_source.request_stop();
}

int CommandManager::historyCount() const
{
    return static_cast<int>(d->history.size());
}

Command* CommandManager::historyAt(int index) const
{
    if (index < 0 || index >= static_cast<int>(d->history.size())) {
        return nullptr;
    }
    return d->history[static_cast<std::size_t>(index)];
}

void CommandManager::clearHistory()
{
    d->history.clear();
}

bool CommandManager::registerCommand(Type command_class, String name, std::function<Command*()> factory)
{
    return d->registry.emplace(std::move(name), RegisteredCommand{ command_class, std::move(factory), d->registration_owner }).second;
}

void CommandManager::setRegistrationOwner(String owner)
{
    d->registration_owner = std::move(owner);
}

const String& CommandManager::registrationOwner() const
{
    return d->registration_owner;
}

bool CommandManager::unregisterCommand(const String& name)
{
    return d->registry.erase(name) > 0;
}

bool CommandManager::unregisterCommand(Type command_class)
{
    bool removed = false;
    for (auto it = d->registry.begin(); it != d->registry.end();) {
        if (it->second.class_type == command_class) {
            it = d->registry.erase(it);
            removed = true;
        } else {
            ++it;
        }
    }
    return removed;
}

bool CommandManager::registerAlias(const String& alias, const String& target)
{
    if (alias.empty() || target.empty()) {
        return false;
    }
    const bool inserted = d->aliases.emplace(alias, target).second;
    if (!inserted) {
        V_LOGW("Alias '{}' already registered, ignoring target '{}'", toUtf8(alias), toUtf8(target));
    }
    return inserted;
}

bool CommandManager::unregisterAlias(const String& alias)
{
    return d->aliases.erase(alias) > 0;
}

bool CommandManager::isRegistered(const String& name) const
{
    return d->registry.find(d->resolveName(name)) != d->registry.end();
}

std::vector<String> CommandManager::names() const
{
    std::vector<String> result;
    result.reserve(d->registry.size());
    for (const auto& entry : d->registry)
    {
        result.push_back(entry.first);
    }
    return result;
}

std::vector<std::pair<String, String>> CommandManager::aliases() const
{
    std::vector<std::pair<String, String>> result;
    result.reserve(d->aliases.size());
    for (const auto& entry : d->aliases)
    {
        result.emplace_back(entry.first, entry.second);
    }
    return result;
}

std::vector<CommandInfo> CommandManager::commandInfos() const
{
    // std::map keeps registry keys sorted, so the result is ordered by name.
    std::vector<CommandInfo> result;
    result.reserve(d->registry.size());

    for (const auto& entry : d->registry) {
        CommandInfo info;
        info.name  = entry.first;
        info.owner = entry.second.owner;
        if (entry.second.factory) {
            std::unique_ptr<Command> command(entry.second.factory());
            info.group       = command->group();
            info.description = command->description();
        }
        result.push_back(std::move(info));
    }

    // Attach each alias to the entry of its target command.
    for (const auto& alias : d->aliases) {
        for (auto& info : result) {
            if (info.name == alias.second) {
                info.aliases.push_back(alias.first);
                break;
            }
        }
    }

    return result;
}

std::vector<CommandInfo> CommandManager::commandInfosForPlugin(const String& owner) const
{
    std::vector<CommandInfo> result;
    for (const auto& info : commandInfos()) {
        if (info.owner == owner) {
            result.push_back(info);
        }
    }
    return result;
}

void CommandManager::setSnapshotHandler(std::function<void()> handler)
{
    d->snapshot_handler = std::move(handler);
}

V_APPFW_NS_END
