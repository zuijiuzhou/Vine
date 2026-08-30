#pragma once

#include "async_global.hpp"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <coroutine>
#include <memory>
#include <mutex>
#include <stop_token>
#include <thread>
#include <utility>

#include <vine/CancellationToken.hpp>

#include "Cancellation.hpp"
#include "Task.hpp"

V_ASYNC_NS_BEGIN

namespace detail {

/**
 * @brief Shared state that lets a sleeping coroutine cancel its timer.
 *
 * When the awaiting coroutine is destroyed while suspended, the awaiter's
 * destructor marks this state cancelled and wakes the timer thread, which then
 * refuses to resume the dangling coroutine. This mirrors AsyncEvent's
 * abandonment safety for detached-thread timers.
 */
struct SleepState
{
    std::mutex mutex_;
    std::condition_variable cv_;
    bool cancelled_{ false };
};

/**
 * @brief Awaiter that resumes a coroutine after a duration, or on cancellation.
 *
 * A detached thread sleeps in small slices so that a cancelled token wakes the
 * coroutine within one slice; await_resume then throws TaskCancelledException.
 */
class SleepAwaiter
{
  public:
    SleepAwaiter(std::chrono::milliseconds duration, CancellationToken token) noexcept
      : duration_(duration), token_(std::move(token))
    {}

    SleepAwaiter(const SleepAwaiter&) = delete;
    SleepAwaiter& operator=(const SleepAwaiter&) = delete;

    /**
     * @brief Cancels the timer when the awaiting frame is destroyed.
     */
    ~SleepAwaiter()
    {
        std::lock_guard<std::mutex> lock(state_->mutex_);
        state_->cancelled_ = true;
        state_->cv_.notify_all();
    }

    /**
     * @brief Returns whether the sleep completes without suspending.
     *
     * @return true for zero/negative durations or an already-cancelled token.
     */
    [[nodiscard]]
    bool await_ready() const noexcept
    {
        return duration_.count() <= 0 || token_.stop_requested();
    }

    /**
     * @brief Starts a detached timer thread and suspends.
     *
     * @param h Coroutine to resume after the duration (or cancellation).
     */
    void await_suspend(std::coroutine_handle<> h)
    {
        auto state = state_;
        auto token = token_;
        auto duration = duration_;

        std::thread([h, state = std::move(state), token = std::move(token), duration]() mutable {
            auto remaining = duration;
            while (remaining > std::chrono::milliseconds(0) && !token.stop_requested())
            {
                const auto slice = std::min(remaining, std::chrono::milliseconds(10));
                std::unique_lock<std::mutex> lock(state->mutex_);
                state->cv_.wait_for(lock, slice, [&] { return state->cancelled_; });
                if (state->cancelled_)
                {
                    return; // The awaiting coroutine was destroyed; never resume it.
                }
                remaining -= slice;
            }
            h.resume();
        }).detach();
    }

    /**
     * @brief Throws when the sleep was cut short by cancellation.
     */
    void await_resume()
    {
        if (token_.stop_requested())
        {
            throw TaskCancelledException{};
        }
    }

  private:
    std::chrono::milliseconds duration_;
    CancellationToken token_;
    std::shared_ptr<SleepState> state_{ std::make_shared<SleepState>() };
};

} // namespace detail

/**
 * @brief Suspends the coroutine for the given duration.
 *
 * A detached thread sleeps in small slices and resumes the coroutine when the
 * duration elapses or the token is cancelled. Cancellation resumes the
 * coroutine within one slice (10 ms) and await_resume throws
 * TaskCancelledException. Destroying the awaiting task before the timer fires
 * cancels the timer safely.
 *
 * @param duration Sleep duration; zero or negative returns immediately.
 * @param token Optional cancellation token.
 * @return A task that completes after the duration.
 */
inline Task<void> sleepFor(std::chrono::milliseconds duration, CancellationToken token = {})
{
    co_await detail::SleepAwaiter{ duration, std::move(token) };
}

V_ASYNC_NS_END
