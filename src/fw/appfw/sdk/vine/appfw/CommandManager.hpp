#pragma once

#include "appfw_global.hpp"

#include <functional>
#include <type_traits>
#include <utility>

#include <vine/Events.hpp>
#include <vine/appfw/Command.hpp>

V_APPFW_NS_BEGIN

class Application;

/**
 * @brief Event arguments fired when a command starts executing.
 *
 * Carries the command that is about to run.
 */
class V_APPFW_API CommandExecutingEventArgs : public EventArgs {
    V_OBJECT_META_DECL

  public:
    explicit CommandExecutingEventArgs(Command* command);

  public:
    /// The command that is about to execute.
    Command* command() const;

  private:
    Command* command_;
};

/**
 * @brief Event arguments fired when a command finishes executing.
 *
 * Carries the command that ran and its execution result.
 */
class V_APPFW_API CommandExecutedEventArgs : public EventArgs {
    V_OBJECT_META_DECL

  public:
    explicit CommandExecutedEventArgs(Command* command, const CommandResult& result);

  public:
    /// The command that finished executing.
    Command* command() const;

    /// The execution result.
    const CommandResult& result() const;

  private:
    Command* command_;
    CommandResult result_;
};

/**
 * @brief Single entry point for running and managing Commands.
 *
 * The CommandManager owns the command execution call stack, routes nested
 * command execution, enforces exclusive rules, records the execution history
 * of the commands that ran, and registers commands so they can be started by
 * name. Undo/Redo is snapshot-based and owned by the document layer; the
 * manager only notifies the document to snapshot its state before an Undoable
 * command runs. Commands must not invoke other commands directly; they run
 * children through this manager.
 */
class V_APPFW_API CommandManager
{
  public:
    explicit CommandManager(Application* app);
    ~CommandManager();

  public:
    /// Fired when a command begins executing (before its business logic runs).
    Event<CommandManager, CommandExecutingEventArgs> executing;

    /// Fired when a command finishes executing, carrying its result.
    Event<CommandManager, CommandExecutedEventArgs> executed;

  public:
    /**
     * @brief Returns the application this manager belongs to.
     *
     * @return The hosting Application.
     */
    Application* application() const;

    /**
     * @brief Executes a command.
     *
     * May be called from within a command to run a nested child command.
     * Exclusive commands first cancel the running command chain. Undoable
     * commands notify the document snapshot handler before executing.
     *
     * @param command Command to execute; must not be null.
     * @return The execution outcome.
     */
    CommandResult executeCommand(Command* command);

    /**
     * @brief Starts a registered command by name.
     *
     * Creates a fresh instance through the registered factory and executes it.
     *
     * @param name Registered command name.
     * @return The execution outcome; Failed when the name is not registered.
     */
    CommandResult executeCommand(const String& name);

    /**
     * @brief Returns the command at the top of the execution stack.
     *
     * @return The running command, or nullptr when idle.
     */
    Command* currentCommand() const;

    /**
     * @brief Returns the number of commands on the execution stack.
     *
     * @return Stack depth.
     */
    int runningCount() const;

    /**
     * @brief Returns the number of commands recorded in the execution history.
     *
     * @return History size.
     */
    int historyCount() const;

    /**
     * @brief Returns the command recorded at the given history index.
     *
     * Index 0 is the oldest recorded execution. The history holds non-owning
     * pointers; the caller must keep the commands alive to inspect them.
     *
     * @param index History index.
     * @return The recorded command, or nullptr when out of range.
     */
    Command* historyAt(int index) const;

    /**
     * @brief Clears the execution history.
     */
    void clearHistory();

    /**
     * @brief Registers a command so it can be started by name.
     *
     * @param command_class Meta class of the command.
     * @param name Unique name used to start the command.
     * @param factory Factory creating a new command instance.
     * @return true if registered, false if the name is already taken.
     */
    bool registerCommand(Type command_class, String name, std::function<Command*()> factory);

    /**
     * @brief Registers a default-constructible command type by name.
     *
     * The command type must have a no-arg constructor and Object meta
     * (V_OBJECT_META_IMPL).
     *
     * @tparam T Command type.
     * @param name Unique name used to start the command.
     * @return true if registered, false if the name is already taken.
     */
    template <typename T>
    bool registerCommand(String name)
    {
        static_assert(std::is_base_of<Command, T>::value, "T must derive from Command");
        return registerCommand(T::desc(), std::move(name), [] { return new T; });
    }

    /**
     * @brief Unregisters a command by name.
     *
     * @param name Command name.
     * @return true if removed, false if not found.
     */
    bool unregisterCommand(const String& name);

    /**
     * @brief Unregisters all commands of the given meta class.
     *
     * @param command_class Meta class of the commands to remove.
     * @return true if at least one command was removed.
     */
    bool unregisterCommand(Type command_class);

    /**
     * @brief Returns whether a command with the given name is registered.
     *
     * @param name Command name.
     * @return true if registered.
     */
    bool isRegistered(const String& name) const;

    /**
     * @brief Sets the handler invoked before an Undoable command executes.
     *
     * The document registers this handler to snapshot its state so the
     * command can be undone later. Undo/Redo themselves are owned by the
     * document layer, not by the CommandManager.
     *
     * @param handler Snapshot callback; empty disables the notification.
     */
    void setSnapshotHandler(std::function<void()> handler);

  private:
    class Context;

    struct Data;
    Data* const d;
};

V_APPFW_NS_END
