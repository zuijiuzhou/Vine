#include "ToggleConsoleLogCommand.hpp"

#include "ConsoleLogRouter.hpp"

#include <vine/appfw/Application.hpp>
#include <vine/appfw/ConfigManager.hpp>
#include <vine/appfw/UserIO.hpp>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(ToggleConsoleLogCommand, Command)

vine::async::Task<CommandResult> ToggleConsoleLogCommand::execute(CommandExecutionContext* context)
{
    auto* app = context ? context->application() : nullptr;
    auto* io  = app ? app->userIO() : nullptr;
    auto* cfg = app ? app->configManager() : nullptr;

    if (!cfg) {
        if (io) {
            io->putString(String(u8"配置管理器不可用"));
        }
        co_return CommandResult(CommandStatus::Failed, String(u8"配置管理器不可用"));
    }

    const String key      = consoleLogConfigKey();
    const bool   enabled  = !cfg->getBool(key, true);
    cfg->setBool(key, enabled); // 触发 changed，AppShellPlugin 同步 sink 的原子开关

    if (io) {
        io->putString(enabled ? String(u8"日志输出到控制台：已开启") : String(u8"日志输出到控制台：已关闭"));
    }

    co_return CommandResult(CommandStatus::Success);
}

V_APPFW_NS_END
