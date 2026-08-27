#include "HelloCommand.hpp"

#include <QMessageBox>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(HelloCommand, Command)

CommandResult HelloCommand::execute(CommandExecutionContext* context)
{
    (void)context;
    QMessageBox::information(nullptr, QStringLiteral("测试插件"), QStringLiteral("Hello from test_plugin!"));
    return CommandResult(CommandStatus::Success);
}

V_APPFW_NS_END
