#include "TestNestedChildCommand.hpp"

#include <chrono>

#include <vine/async/Sleep.hpp>
#include <vine/progress/ProgressHost.hpp>
#include <vine/progress/ProgressRange.hpp>
#include <vine/progress/ProgressScope.hpp>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(TestNestedChildCommand, Command)

vine::async::Task<CommandResult> TestNestedChildCommand::execute(CommandExecutionContext* context)
{
    // 子命令是独立宿主（前台栈顶），上报自己的进度；父命令进度冻结在栈中。
    if (auto* host = vine::progress::ProgressHost::current()) {
        host->setLabel("子:压缩");
        vine::progress::ProgressScope scope = host->scope("子:压缩", 30);
        for (int i = 0; i < 30; ++i) {
            if (context && context->isCancelled()) {
                co_return CommandResult(CommandStatus::Cancelled);
            }
            scope.next(1);
            co_await vine::async::sleepFor(std::chrono::milliseconds(20));
        }
    }
    else {
        // 无宿主（父命令非 LongRunning）：仅做耗时 + 取消检查。
        for (int i = 0; i < 30; ++i) {
            if (context && context->isCancelled()) {
                co_return CommandResult(CommandStatus::Cancelled);
            }
            co_await vine::async::sleepFor(std::chrono::milliseconds(20));
        }
    }
    co_return CommandResult(CommandStatus::Success);
}

V_APPFW_NS_END
