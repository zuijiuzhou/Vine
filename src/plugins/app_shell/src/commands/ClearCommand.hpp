#pragma once

#include <vine/appfw/Command.hpp>
#include <vine/appfw/command_export.hpp>

V_APPFW_NS_BEGIN

/**
 * @brief Clears the application console (visual console panel or terminal).
 */
class ClearCommand : public Command {
    V_OBJECT_META_DECL;
    V_DECLARE_COMMAND(ClearCommand, u8"clear")

  public:
    String group() const override { return u8"控制台"; }
    String description() const override { return u8"清空控制台"; }
    CommandFlags flags() const override { return CommandFlags::None; }
    vine::co::Task<CommandResult> execute(CommandExecutionContext* context) override;
};

V_APPFW_NS_END
