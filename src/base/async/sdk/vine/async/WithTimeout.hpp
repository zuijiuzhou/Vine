#pragma once

#include "async_global.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include <vine/CancellationToken.hpp>
#include <vine/Exception.hpp>
#include <vine/String.hpp>

#include "Cancellation.hpp"
#include "Sleep.hpp"
#include "Task.hpp"
#include "When.hpp"

V_ASYNC_NS_BEGIN

/**
 * @brief Exception thrown when a task does not complete in time.
 *
 * Derives from vine::Exception with code Exception::TIMEOUT.
 */
class TimeoutException : public vine::Exception
{
  public:
    TimeoutException()
      : vine::Exception(vine::Exception::Code::TIMEOUT, String(u8"operation timed out"))
    {}
};

namespace detail {

/**
 * @brief A task that "wins" the timeout race by throwing TimeoutException.
 */
template<typename T>
Task<T> timeoutTask(std::chrono::milliseconds timeout, CancellationToken token)
{
    co_await sleepFor(timeout, std::move(token));
    throw TimeoutException{};
}

} // namespace detail

/**
 * @brief Runs a task, failing with TimeoutException if it exceeds a duration.
 *
 * Races the task against a timer; if the timer wins, the still-running task is
 * destroyed (cooperative cancellation via structured concurrency) and
 * TimeoutException is thrown. If the task wins, its result or exception is
 * returned unchanged. The token is forwarded to the timer.
 *
 * @tparam T Result type of the task (non-void).
 * @param task Task to run.
 * @param timeout Maximum duration allowed.
 * @param token Optional token forwarded to the timer.
 * @return A task yielding the result or throwing TimeoutException.
 */
template<typename T>
    requires (!std::is_void_v<T>)
Task<T> withTimeout(Task<T> task, std::chrono::milliseconds timeout, CancellationToken token = {})
{
    std::vector<Task<T>> race;
    race.push_back(std::move(task));
    race.push_back(detail::timeoutTask<T>(timeout, std::move(token)));
    co_return co_await whenAny(std::move(race));
}

/**
 * @brief Runs a void task, failing with TimeoutException if it exceeds a duration.
 *
 * @param task Void task to run.
 * @param timeout Maximum duration allowed.
 * @param token Optional token forwarded to the timer.
 * @return A task completing on success or throwing TimeoutException.
 */
inline Task<void> withTimeout(Task<void> task,
                              std::chrono::milliseconds timeout,
                              CancellationToken token = {})
{
    auto timed_out = std::make_shared<std::atomic<bool>>(false);

    std::vector<AnyTask> race;
    race.push_back(std::move(task));
    race.push_back([timeout, token = std::move(token), timed_out]() -> Task<void> {
        co_await sleepFor(timeout, std::move(token));
        timed_out->store(true);
    }());

    co_await whenAny(std::move(race));

    if (timed_out->load())
    {
        throw TimeoutException{};
    }
}

V_ASYNC_NS_END
