#pragma once

#include "co_global.hpp"
#include "Scheduler.hpp"
#include "Task.hpp"

#include <type_traits>
#include <utility>

V_CO_NS_BEGIN

/**
 * @brief Resumes the continuation on the given scheduler.
 *
 * Equivalent to co_await scheduler.schedule(); provided for a descriptive name
 * aligned with cppcoro's resume_on.
 *
 * @tparam S Scheduler type.
 * @param s Scheduler deciding the resume context.
 * @return An awaitable that suspends and resumes via the scheduler.
 */
template<scheduler S>
auto resumeOn(S& s) -> decltype(s.schedule())
{
    return s.schedule();
}

/**
 * @brief Starts a task on the given scheduler.
 *
 * The coroutine first hops to the scheduler's context, then runs the task.
 *
 * @tparam S Scheduler type.
 * @tparam T Result type of the task.
 * @param s Scheduler deciding where the task starts; must outlive the task.
 * @param task Task to run.
 * @return A task that runs task after scheduling onto s.
 */
template<scheduler S, typename T>
Task<T> scheduleOn(S& s, Task<T> task)
{
    co_await s.schedule(); // Hop to the scheduler's context before running the task.
    if constexpr (std::is_void_v<T>)
    {
        co_await std::move(task);
        co_return;
    }
    else
    {
        co_return co_await std::move(task);
    }
}

V_CO_NS_END
