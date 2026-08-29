#pragma once

#include <vine/appfw/Command.hpp>
#include <vine/appfw/command_export.hpp>

V_APPFW_NS_BEGIN

/**
 * @brief Shows the about dialog with this application's content.
 */
class AboutCommand : public Command {
    V_OBJECT_META_DECL;
    V_DECLARE_COMMAND(AboutCommand, u8"about")

  public:
    String group() const override { return u8"帮助"; }
    String description() const override { return u8"显示关于信息"; }
    CommandFlags flags() const override { return CommandFlags::None; }
    vine::co::Task<CommandResult> execute(CommandExecutionContext* context) override;
};

V_APPFW_NS_END
