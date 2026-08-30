#include <vine/appfw/UserIO.hpp>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(UserIO, Object);

UserIO::UserIO()
{}

void UserIO::setCommandManager(CommandManager* manager)
{
    command_manager_ = manager;
}

void UserIO::clear()
{}

RawPtr<CommandManager> UserIO::commandManager() const
{
    return command_manager_;
}

V_APPFW_NS_END
