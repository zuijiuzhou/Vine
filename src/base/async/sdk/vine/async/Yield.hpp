#pragma once

#include "async_global.hpp"

#include <coroutine>
#include <thread>

V_ASYNC_NS_BEGIN

/**
 * @brief Awaiter that yields the current thread's CPU slice.
 *
 * await_suspend calls std::this_thread::yield() so the OS can schedule other
 * threads, then resumes the coroutine inline on the same thread.
 */
class YieldAwaiter
{
  public:
    /**
     * @brief Always suspends so the yield actually happens.
     *
     * @return false.
     */
    [[nodiscard]]
    bool await_ready() const noexcept
    {
        return false;
    }

    /**
     * @brief Yields the CPU to the OS, then resumes inline.
     *
     * @param h Coroutine to resume after yielding.
     */
    void await_suspend(std::coroutine_handle<> h) const noexcept
    {
        std::this_thread::yield();
        h.resume();
    }

    void await_resume() const noexcept {}
};

/**
 * @brief Returns an awaiter that yields the current thread.
 *
 * co_await yield() gives other threads a chance to run, then continues on the
 * same thread without changing the execution context.
 *
 * @return An awaiter; co_await it to yield.
 */
[[nodiscard]]
inline YieldAwaiter yield() noexcept
{
    return {};
}

V_ASYNC_NS_END
