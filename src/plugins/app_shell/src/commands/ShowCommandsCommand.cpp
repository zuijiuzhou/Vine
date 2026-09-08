#include "ShowCommandsCommand.hpp"

#include <vine/appfw/gui/CommandManagerDialog.hpp>

#include <vine/appfw/Application.hpp>
#include <vine/appfw/CommandManager.hpp>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(ShowCommandsCommand, Command)

vine::async::Task<CommandResult> ShowCommandsCommand::execute(CommandExecutionContext* context)
{
    auto* app = context ? context->application() : nullptr;
    auto* cm  = app ? app->commandManager() : nullptr;
    if (!cm) {
        co_return CommandResult(CommandStatus::Failed, String(u8"命令管理器未就绪"));
    }

    auto* dlg = new gui::CommandManagerDialog(cm);
    dlg->resize(640, 420);
    dlg->exec();
    delete dlg;
    co_return CommandResult(CommandStatus::Success);
}

V_APPFW_NS_END
