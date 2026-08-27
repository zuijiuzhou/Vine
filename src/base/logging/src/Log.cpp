#include <vine/logging/Log.hpp>

#include <utility>

V_LOGGING_NS_BEGIN

Logger& Log::defaultLogger()
{
    static Logger s_logger("vine");
    return s_logger;
}

void Log::init()
{
    defaultLogger() = Logger("vine");
}

void Log::init(LogLevel level)
{
    defaultLogger() = Logger("vine", level);
}

void Log::init(LogLevel level, std::vector<LogSink> sinks, std::string pattern)
{
    defaultLogger() = Logger("vine", level, std::move(sinks), std::move(pattern));
}

void Log::setLevel(LogLevel level)
{
    defaultLogger().setLevel(level);
}

void Log::setPattern(const std::string& pattern)
{
    defaultLogger().setPattern(pattern);
}

void Log::flush()
{
    defaultLogger().flush();
}

V_LOGGING_NS_END
