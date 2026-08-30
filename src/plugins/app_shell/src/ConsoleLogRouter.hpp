#pragma once

#include <vine/String.hpp>
#include <vine/appfw/appfw_global.hpp>

#include <atomic>
#include <memory>

V_APPFW_NS_BEGIN

/**
 * @brief Shared state controlling whether default-logger records are routed
 * to the console panel.
 *
 * Owned by AppShellPlugin, which creates the sink, and shared with the toggle
 * command. The flag is an std::atomic so the sink callback checks it lock-free
 * from any thread.
 */
std::shared_ptr<std::atomic<bool>> consoleLogEnabledState();

/**
 * @brief Config key of the console-log routing toggle.
 *
 * @return The dotted ConfigManager path of the bool value.
 */
const String& consoleLogConfigKey();

V_APPFW_NS_END
