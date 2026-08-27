#pragma once

#include <vine/appfw/Command.hpp>
#include <vine/appfw/command_export.hpp>

V_APPFW_NS_BEGIN

/**
 * @brief Shows the currently loaded plugins and their library paths.
 */
class ShowPluginsCommand : public Command {
    V_OBJECT_META_DECL;
    V_DECLARE_COMMAND(ShowPluginsCommand, u8"showPlugins")

  public:
    String group() const override { return u8"插件"; }
    CommandFlags flags() const override { return CommandFlags::None; }
    CommandResult execute(CommandExecutionContext* context) override;
};

V_APPFW_NS_END
