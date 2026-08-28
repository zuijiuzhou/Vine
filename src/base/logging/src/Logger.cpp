#include <vine/logging/Logger.hpp>

#include <memory>
#include <utility>

#include <spdlog/spdlog.h>

#include "SpdlogInternal.hpp"

V_LOGGING_NS_BEGIN

struct Logger::Impl
{
    std::string                     name;
    std::shared_ptr<spdlog::logger> logger;
};

Logger::Logger()
  : d(std::make_shared<Impl>())
{}

Logger::Logger(std::string name, LogLevel level)
  : Logger(std::move(name), level, { LogSink::console() }, {})
{}

Logger::Logger(std::string name, LogLevel level, std::vector<LogSink> sinks, std::string pattern)
  : d(std::make_shared<Impl>())
{
    d->name = std::move(name);

    std::vector<spdlog::sink_ptr> spd_sinks;
    for (const auto& sink : sinks) {
        if (sink.d && sink.d->sink) {
            spd_sinks.push_back(sink.d->sink);
        }
    }

    d->logger = std::make_shared<spdlog::logger>(d->name, spd_sinks.begin(), spd_sinks.end());
    d->logger->set_level(toSpdlogLevel(level));
    if (!pattern.empty()) {
        d->logger->set_pattern(pattern);
    }
}

const std::string& Logger::name() const
{
    return d->name;
}

LogLevel Logger::level() const
{
    return d->logger ? toLogLevel(d->logger->level()) : LogLevel::Off;
}

void Logger::setLevel(LogLevel level)
{
    if (d->logger) {
        d->logger->set_level(toSpdlogLevel(level));
    }
}

void Logger::setPattern(const std::string& pattern)
{
    if (d->logger) {
        d->logger->set_pattern(pattern);
    }
}

void Logger::flush()
{
    if (d->logger) {
        d->logger->flush();
    }
}

bool Logger::isEnabled(LogLevel level) const
{
    return d->logger && d->logger->should_log(toSpdlogLevel(level));
}

void Logger::log(LogLevel level, std::string message, const std::source_location& loc)
{
    if (!d->logger || !d->logger->should_log(toSpdlogLevel(level))) {
        return;
    }

    d->logger->log(spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()}, toSpdlogLevel(level), message);
}

V_LOGGING_NS_END
