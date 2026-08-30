#pragma once

#include <vine/appfw/Command.hpp>
#include <vine/appfw/command_export.hpp>

V_APPFW_NS_BEGIN

/**
 * @brief A trivial command contributed by the test plugin.
 */
class HelloCommand : public Command {
    V_OBJECT_META_DECL;
    V_DECLARE_COMMAND(HelloCommand, u8"test_hello")

  public:
    String group() const override { return u8"测试"; }
    String description() const override { return u8"测试命令"; }
    CommandFlags flags() const override { return CommandFlags::None; }
    vine::async::Task<CommandResult> execute(CommandExecutionContext* context) override;
};

V_APPFW_NS_END
