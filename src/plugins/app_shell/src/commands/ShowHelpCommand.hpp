#pragma once

#include <vine/appfw/Command.hpp>
#include <vine/appfw/command_export.hpp>

V_APPFW_NS_BEGIN

/**
 * @brief Shows a short help text through the application UserIO.
 */
class ShowHelpCommand : public Command {
    V_OBJECT_META_DECL;
    V_DECLARE_COMMAND(ShowHelpCommand, u8"show_help")

  public:
    String group() const override { return u8"帮助"; }
    String description() const override { return u8"显示帮助信息"; }
    CommandFlags flags() const override { return CommandFlags::None; }
    vine::async::Task<CommandResult> execute(CommandExecutionContext* context) override;
};

V_APPFW_NS_END
