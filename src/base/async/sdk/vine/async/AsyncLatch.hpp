#pragma once

#include "async_global.hpp"

#include <coroutine>
#include <cstddef>
#include <memory>
#include <mutex>

#include "AsyncEvent.hpp"
#include "Task.hpp"

V_ASYNC_NS_BEGIN

namespace detail {

/**
 * @brief Shared state of an AsyncLatch.
 */
struct LatchState
{
    std::mutex mutex_;
    std::size_t count_{ 0 };
    AsyncEvent done_;
};

} // namespace detail

/**
 * @brief Count-down latch for one-time synchronization.
 *
 * wait() suspends until the initial count reaches zero via countDown(); once
 * released it returns immediately forever (manual-reset semantics). Thread-safe.
 */
class AsyncLatch
{
  public:
    /**
     * @brief Constructs a latch.
     *
     * @param count Number of countDown() calls required to release waiters.
     */
    explicit AsyncLatch(std::size_t count) : state_(std::make_shared<detail::LatchState>())
    {
        state_->count_ = count;
    }

    AsyncLatch(const AsyncLatch&) = delete;
    AsyncLatch& operator=(const AsyncLatch&) = delete;

    /**
     * @brief Decrements the count; releases waiters when it reaches zero.
     *
     * @param n Amount to decrement (clamped at the current count).
     */
    void countDown(std::size_t n = 1)
    {
        bool ready = false;
        {
            std::lock_guard<std::mutex> lock(state_->mutex_);
            if (n > state_->count_)
            {
                n = state_->count_;
            }
            state_->count_ -= n;
            ready = (state_->count_ == 0);
        }
        if (ready)
        {
            state_->done_.set();
        }
    }

    /**
     * @brief Returns whether the count has reached zero.
     *
     * @return true if wait() would not suspend.
     */
    [[nodiscard]]
    bool isReady() const
    {
        std::lock_guard<std::mutex> lock(state_->mutex_);
        return state_->count_ == 0;
    }

    /**
     * @brief Awaits until the count reaches zero.
     *
     * @return A task completing when the latch releases.
     */
    [[nodiscard]]
    Task<void> wait()
    {
        auto state = state_; // Keep the latch alive while awaiting.
        co_await state->done_;
    }

  private:
    std::shared_ptr<detail::LatchState> state_;
};

V_ASYNC_NS_END
