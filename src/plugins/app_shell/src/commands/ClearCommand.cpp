#include "ClearCommand.hpp"

#include <vine/appfw/Application.hpp>
#include <vine/appfw/UserIO.hpp>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(ClearCommand, Command)

vine::co::Task<CommandResult> ClearCommand::execute(CommandExecutionContext* context)
{
    auto* app = context ? context->application() : nullptr;
    auto* io  = app ? app->userIO() : nullptr;
    if (!io) {
        co_return CommandResult(CommandStatus::Failed, String(u8"用户 I/O 未就绪"));
    }

    io->clear();
    co_return CommandResult(CommandStatus::Success);
}

V_APPFW_NS_END
