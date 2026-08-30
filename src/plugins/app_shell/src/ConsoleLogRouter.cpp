#include "ConsoleLogRouter.hpp"

V_APPFW_NS_BEGIN

namespace
{

/**
 * @brief Process-wide toggle, initialized to enabled.
 */
std::shared_ptr<std::atomic<bool>>& consoleLogState()
{
    static std::shared_ptr<std::atomic<bool>> s_state = std::make_shared<std::atomic<bool>>(true);
    return s_state;
}

} // namespace

std::shared_ptr<std::atomic<bool>> consoleLogEnabledState()
{
    return consoleLogState();
}

const String& consoleLogConfigKey()
{
    static const String s_key = String(u8"logging.console_enabled");
    return s_key;
}

V_APPFW_NS_END
