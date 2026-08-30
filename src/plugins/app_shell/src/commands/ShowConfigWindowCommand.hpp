#pragma once

#include <vine/appfw/Command.hpp>
#include <vine/appfw/command_export.hpp>

V_APPFW_NS_BEGIN

/**
 * @brief Opens the configuration window (ConfigWindow, hosted by this plugin).
 */
class ShowConfigWindowCommand : public Command {
    V_OBJECT_META_DECL;
    V_DECLARE_COMMAND(ShowConfigWindowCommand, u8"show_config")

  public:
    String group() const override { return u8"插件"; }
    String description() const override { return u8"打开配置窗口"; }
    CommandFlags flags() const override { return CommandFlags::None; }
    vine::async::Task<CommandResult> execute(CommandExecutionContext* context) override;
};

V_APPFW_NS_END
