#pragma once

#include "appfw_global.hpp"

#include <any>
#include <cstdint>

#include <vine/Object.hpp>
#include <vine/String.hpp>
#include <vine/async/Task.hpp>

V_APPFW_NS_BEGIN

class Application;
class CommandManager;

/**
 * @brief Execution characteristics of a Command.
 *
 * Flags describe how the CommandManager treats a command at run time.
 * Version 1 defines two core flags: Undoable for commands that modify business
 * data, and Exclusive for commands that need exclusive execution rights.
 */
enum class CommandFlags : std::uint32_t
{
    /**
     * @brief Default: a plain one-shot command.
     */
    None = 0,

    /**
     * @brief Modifies business data; participates in Undo/Redo.
     */
    Undoable = 1 << 0,

    /**
     * @brief Holds exclusive execution while running.
     */
    Exclusive = 1 << 1
};

/**
 * @brief Outcome of executing a Command.
 */
enum class CommandStatus : std::uint8_t
{
    /**
     * @brief The command completed successfully.
     */
    Success = 0,

    /**
     * @brief The command failed to complete.
     */
    Failed,

    /**
     * @brief The command was cancelled before completing.
     */
    Cancelled,
};

/**
 * @brief Result returned by Command::execute().
 *
 * Carries the execution status, an optional message describing the outcome
 * (usually set on failure), and optional business data produced by the
 * command. succeeded() reports whether the command completed successfully.
 */
class V_APPFW_API CommandResult
{
  public:
    CommandResult() = default;

    /**
     * @brief Constructs a result with a status and optional message.
     *
     * @param status Execution outcome.
     * @param message Optional description, usually a failure reason.
     */
    explicit CommandResult(CommandStatus status, String message = {});

  public:
    /**
     * @brief Returns the execution outcome.
     *
     * @return The status.
     */
    CommandStatus status() const;

    /**
     * @brief Returns whether the command completed successfully.
     *
     * @return true if the status is Success.
     */
    bool succeeded() const;

    /**
     * @brief Returns the optional message describing the outcome.
     *
     * @return The message.
     */
    const String& message() const;

    /**
     * @brief Returns the business data produced by the command.
     *
     * Empty when the command produced no data. Retrieve the typed value with
     * std::any_cast<T>.
     *
     * @return The data.
     */
    const std::any& data() const;

    /**
     * @brief Sets the business data produced by the command.
     *
     * @param data The data.
     */
    void setData(std::any data);

  private:
    CommandStatus status_ = CommandStatus::Success;
    String message_;
    std::any data_;
};

/**
 * @brief Execution context handed to Command::execute().
 *
 * Created by the CommandManager for each execution and passed to the command.
 * The command reads application resources (services, config, ...) through
 * application(). The CommandManager provides a private implementation; user
 * code never constructs a context directly.
 */
class V_APPFW_API CommandExecutionContext
{
  public:
    virtual ~CommandExecutionContext() = default;

    /**
     * @brief Returns the application executing the command.
     *
     * @return The hosting Application.
     */
    virtual Application* application() const = 0;
};

/**
 * @brief Base class of all user operations.
 *
 * A Command describes one user operation, runs the concrete business logic in
 * execute(), and accesses application services through the execution context.
 * It does not store long-lived application state and does not manage UI or
 * document lifecycles.
 *
 * Commands run through the CommandManager. A command may start nested
 * commands, but only via application()->commandManager(), never directly.
 */
class V_APPFW_API Command : public Object
{
    V_OBJECT_META_DECL;
    V_DISABLE_COPY_MOVE(Command);

  public:
    /**
     * @brief Default-constructs a command.
     *
     * Required so commands can be created through the registered factory
     * (registerCommand<T> instantiates T with new T).
     */
    Command() = default;

  public:
    /**
     * @brief Unique command name used as identifier and for menus.
     *
     * @return The command name.
     */
    virtual String name() const = 0;

    /**
     * @brief Group the command belongs to (menu/toolbar grouping).
     *
     * May be empty when the command has no group.
     *
     * @return The group name.
     */
    virtual String group() const = 0;

    /**
     * @brief Short human-readable description of what the command does.
     *
     * @return The description, empty when not provided.
     */
    virtual String description() const { return {}; }

    /**
     * @brief Execution characteristics of this command.
     *
     * @return The command flags.
     */
    virtual CommandFlags flags() const = 0;

    /**
     * @brief Runs the command business logic.
     *
     * The command may suspend on asynchronous operations (for example user
     * input or an asynchronous delay) by co_awaiting them.
     *
     * @param context Execution context providing application access and
     *                nested command execution.
     * @return A task yielding the execution outcome.
     */
    virtual vine::async::Task<CommandResult> execute(CommandExecutionContext* context) = 0;
};

V_APPFW_NS_END
