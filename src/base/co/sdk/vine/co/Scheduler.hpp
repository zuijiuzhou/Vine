#pragma once

#include "co_global.hpp"

#include <coroutine>

V_CO_NS_BEGIN

namespace detail {

/**
 * @brief Constrains a type to the minimal awaiter protocol.
 */
template<typename T>
concept awaitable = requires(T&& a) {
    a.await_ready();
    a.await_suspend(std::coroutine_handle<>{});
    a.await_resume();
};

} // namespace detail

/**
 * @brief Constrains a type that can decide where a coroutine resumes.
 *
 * A scheduler exposes a callable schedule() returning an awaitable object.
 * Awaiting that object suspends the coroutine and later resumes it in the
 * execution context chosen by the scheduler.
 */
template<typename T>
concept scheduler = requires(T& s) {
    { s.schedule() } -> detail::awaitable;
};

/**
 * @brief Scheduler that resumes a coroutine inline without suspending.
 *
 * schedule() returns an awaiter whose await_ready() is always true, so
 * co_await schedule() never suspends and the coroutine keeps running on the
 * same thread. Useful as the default when no context switch is required.
 */
class InlineScheduler
{
  public:
    /**
     * @brief Awaiter that never suspends.
     *
     * await_ready() returning true makes the coroutine skip suspension and
     * continue inline on the calling thread.
     */
    class ScheduleAwaiter
    {
      public:
        [[nodiscard]]
        bool await_ready() const noexcept
        {
            return true; // Never suspends; continue inline.
        }

        void await_suspend(std::coroutine_handle<>) const noexcept {}

        void await_resume() const noexcept {}
    };

    /**
     * @brief Returns an awaiter that resumes inline (never suspends).
     *
     * @return An awaiter whose await_ready() is true.
     */
    [[nodiscard]]
    ScheduleAwaiter schedule() const noexcept
    {
        return {};
    }
};

V_CO_NS_END
