#include "ShowPluginsCommand.hpp"

#include <vine/appfw/gui/PluginManagerDialog.hpp>

#include <vine/appfw/Application.hpp>
#include <vine/appfw/PluginManager.hpp>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(ShowPluginsCommand, Command)

vine::co::Task<CommandResult> ShowPluginsCommand::execute(CommandExecutionContext* context)
{
    auto* app = context ? context->application() : nullptr;
    auto* pm  = app ? app->pluginManager() : nullptr;
    if (!pm) {
        co_return CommandResult(CommandStatus::Failed, String(u8"应用未就绪"));
    }

    auto* dlg = new gui::PluginManagerDialog(pm);
    dlg->setWindowTitle(u8"插件管理器");
    dlg->resize(520, 380);
    dlg->exec();
    delete dlg;
    co_return CommandResult(CommandStatus::Success);
}

V_APPFW_NS_END
