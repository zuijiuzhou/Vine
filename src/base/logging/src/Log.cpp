#include <vine/logging/Log.hpp>

#include <utility>

V_LOGGING_NS_BEGIN

Logger& defaultLogger()
{
    static Logger s_logger("vine");
    return s_logger;
}

void initDefault(LogConfig config)
{
    if (config.sinks.empty()) {
        config.sinks.push_back(LogSink::console());
    }
    defaultLogger() = Logger("vine", config.level, std::move(config.sinks), std::move(config.pattern));
}

void flushDefault()
{
    defaultLogger().flush();
}

V_LOGGING_NS_END
