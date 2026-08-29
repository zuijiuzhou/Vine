#include "HelloCommand.hpp"

#include <QMessageBox>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(HelloCommand, Command)

vine::co::Task<CommandResult> HelloCommand::execute(CommandExecutionContext* context)
{
    (void)context;
    QMessageBox::information(nullptr, QStringLiteral("测试插件"), QStringLiteral("Hello from test_plugin!"));
    co_return CommandResult(CommandStatus::Success);
}

V_APPFW_NS_END
