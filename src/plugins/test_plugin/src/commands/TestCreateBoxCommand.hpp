#pragma once

#include <vine/appfw/Command.hpp>
#include <vine/appfw/command_export.hpp>

V_APPFW_NS_BEGIN

/**
 * @brief Coroutine test command: prompts for length/width/height and prints them.
 */
class TestCreateBoxCommand : public Command {
    V_OBJECT_META_DECL;
    V_DECLARE_COMMAND(TestCreateBoxCommand, u8"test_createbox")

  public:
    String group() const override { return u8"测试"; }
    String description() const override { return u8"测试协程命令：输入长宽高并打印"; }
    CommandFlags flags() const override { return CommandFlags::None; }
    vine::async::Task<CommandResult> execute(CommandExecutionContext* context) override;
};

V_APPFW_NS_END
