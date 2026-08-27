#pragma once

#include "logging_global.hpp"

#include <string>
#include <vector>

#include "LogLevel.hpp"
#include "LogSink.hpp"
#include "Logger.hpp"

V_LOGGING_NS_BEGIN

/**
 * @brief Static facade over the process-wide default logger.
 *
 * init() configures the default logger; defaultLogger() returns it. The
 * V_LOGT..V_LOGC macros log to the default logger with the current source
 * location.
 */
class V_LOGGING_API Log
{
  public:
    /**
     * @brief Reconfigures the default logger as console + Info level.
     */
    static void init();

    /**
     * @brief Reconfigures the default logger as console with the given level.
     *
     * @param level Initial level.
     */
    static void init(LogLevel level);

    /**
     * @brief Reconfigures the default logger with the given sinks and pattern.
     *
     * @param level Initial level.
     * @param sinks Destinations; an empty list adds a console sink.
     * @param pattern Format pattern; see Logger::setPattern() for the flags.
     */
    static void init(LogLevel level, std::vector<LogSink> sinks, std::string pattern = {});

    /**
     * @brief Returns the process-wide default logger.
     *
     * @return The default logger.
     */
    static Logger& defaultLogger();

    /**
     * @brief Sets the minimum level of the default logger.
     *
     * @param level Minimum level.
     */
    static void setLevel(LogLevel level);

    /**
     * @brief Sets the format pattern of the default logger.
     *
     * See Logger::setPattern() for the pattern flags.
     *
     * @param pattern Format pattern.
     */
    static void setPattern(const std::string& pattern);

    /**
     * @brief Flushes the default logger.
     */
    static void flush();
};

V_LOGGING_NS_END

/*
 * Macros logging to the default logger.
 *
 * They forward the source location (std::source_location::current()) together
 * with an std::format string and its arguments, for example:
 *
 *     V_LOGI("mesh loaded, {} triangles", count);
 *
 * Levels: V_LOGT(trace) V_LOGD(debug) V_LOGI(info)
 *         V_LOGW(warn)  V_LOGE(error) V_LOGC(critical)
 */
#define V_LOGT(...) ::vine::logging::Log::defaultLogger().trace(::std::source_location::current(), __VA_ARGS__)
#define V_LOGD(...) ::vine::logging::Log::defaultLogger().debug(::std::source_location::current(), __VA_ARGS__)
#define V_LOGI(...) ::vine::logging::Log::defaultLogger().info(::std::source_location::current(), __VA_ARGS__)
#define V_LOGW(...) ::vine::logging::Log::defaultLogger().warn(::std::source_location::current(), __VA_ARGS__)
#define V_LOGE(...) ::vine::logging::Log::defaultLogger().error(::std::source_location::current(), __VA_ARGS__)
#define V_LOGC(...) ::vine::logging::Log::defaultLogger().critical(::std::source_location::current(), __VA_ARGS__)
