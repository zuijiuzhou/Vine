#include "ShowConfigWindowCommand.hpp"

#include "gui/ConfigWindow.hpp"

#include <vine/appfw/Application.hpp>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(ShowConfigWindowCommand, Command)

CommandResult ShowConfigWindowCommand::execute(CommandExecutionContext* context)
{
    auto* app = context ? context->application() : nullptr;
    if (!app) {
        return CommandResult(CommandStatus::Failed, String(u8"应用未就绪"));
    }
    auto* win = new gui::ConfigWindow(app->configRegistry(), app->configManager());
    win->setWindowTitle(u8"设置");
    win->resize(480, 360);
    win->show();
    return CommandResult(CommandStatus::Success);
}

V_APPFW_NS_END
