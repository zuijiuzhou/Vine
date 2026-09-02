#pragma once

#include <vine/appfw/Command.hpp>
#include <vine/appfw/command_export.hpp>

V_APPFW_NS_BEGIN

/**
 * @brief Lists every registered render backend and its metadata.
 *
 * Queries graphics::RenderBackendRegistry and prints one line per backend
 * (name, display name, description, version, vendor) through the application
 * UserIO (console in GUI, stdout headless). One plugin may register several
 * backends, so each entry is listed individually.
 */
class ShowRenderBackendsCommand : public Command {
    V_OBJECT_META_DECL;
    V_DECLARE_COMMAND(ShowRenderBackendsCommand, u8"show_render_backends")

  public:
    String group() const override { return u8"插件"; }
    String description() const override { return u8"显示已注册渲染后端"; }
    CommandFlags flags() const override { return CommandFlags::None; }
    vine::async::Task<CommandResult> execute(CommandExecutionContext* context) override;
};

V_APPFW_NS_END
