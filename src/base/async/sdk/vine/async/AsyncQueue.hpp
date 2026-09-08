#pragma once

#include "async_global.hpp"

#include <algorithm>
#include <coroutine>
#include <cstddef>
#include <deque>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Task.hpp"

V_ASYNC_NS_BEGIN

namespace detail {

/**
 * @brief Shared state of an AsyncQueue.
 *
 * capacity_ == 0 means unbounded (push never suspends). poppers_/pushers_
 * hold the coroutines waiting for a value or room; handles are resumed outside
 * the mutex to avoid lock-held-resume deadlocks.
 */
template<typename T>
struct AsyncQueueState
{
    std::mutex mutex_;
    std::size_t capacity_{ 0 };
    std::deque<T> items_;
    std::vector<std::coroutine_handle<>> poppers_;
    std::vector<std::coroutine_handle<>> pushers_;
    bool closed_{ false };
};

/**
 * @brief Pops one item, waking a waiting bounded pusher.
 *
 * Throws when the queue is empty; on a closed-and-empty queue the throw
 * carries a "closed" message so poppers of a drained queue can stop.
 */
template<typename T>
T popOne(const std::shared_ptr<AsyncQueueState<T>>& state)
{
    std::vector<std::coroutine_handle<>> to_resume;
    T value;
    {
        std::lock_guard<std::mutex> lock(state->mutex_);
        if (state->items_.empty())
        {
            if (state->closed_)
            {
                throw std::runtime_error("async::AsyncQueue: closed");
            }
            throw std::logic_error("async::AsyncQueue: pop on empty queue");
        }
        value = std::move(state->items_.front());
        state->items_.pop_front();
        if (!state->pushers_.empty())
        {
            to_resume.push_back(state->pushers_.front());
            state->pushers_.erase(state->pushers_.begin());
        }
    }
    for (auto h : to_resume)
    {
        h.resume();
    }
    return value;
}

/**
 * @brief Awaiter for AsyncQueue::pop(); unregisters on destruction.
 */
template<typename T>
class QueuePopAwaiter
{
  public:
    explicit QueuePopAwaiter(std::shared_ptr<AsyncQueueState<T>> state) noexcept
      : state_(std::move(state))
    {}

    QueuePopAwaiter(const QueuePopAwaiter&) = delete;
    QueuePopAwaiter& operator=(const QueuePopAwaiter&) = delete;

    ~QueuePopAwaiter()
    {
        if (handle_)
        {
            std::lock_guard<std::mutex> lock(state_->mutex_);
            auto& waiters = state_->poppers_;
            waiters.erase(std::remove(waiters.begin(), waiters.end(), handle_), waiters.end());
        }
    }

    /**
     * @brief Returns whether a value is already available.
     *
     * @return true when pop can complete without suspending.
     */
    [[nodiscard]]
    bool await_ready() const noexcept
    {
        std::lock_guard<std::mutex> lock(state_->mutex_);
        return !state_->items_.empty() || state_->closed_;
    }

    bool await_suspend(std::coroutine_handle<> h) noexcept
    {
        handle_ = h;
        std::lock_guard<std::mutex> lock(state_->mutex_);
        if (!state_->items_.empty() || state_->closed_)
        {
            return false;
        }
        state_->poppers_.push_back(h);
        return true;
    }

    T await_resume()
    {
        return detail::popOne(state_);
    }

  private:
    std::shared_ptr<AsyncQueueState<T>> state_;
    std::coroutine_handle<> handle_{};
};

/**
 * @brief Awaiter for AsyncQueue::push(); suspends only when bounded and full.
 */
template<typename T>
class QueuePushAwaiter
{
  public:
    explicit QueuePushAwaiter(std::shared_ptr<AsyncQueueState<T>> state, T value) noexcept
      : state_(std::move(state)), value_(std::move(value))
    {}

    QueuePushAwaiter(const QueuePushAwaiter&) = delete;
    QueuePushAwaiter& operator=(const QueuePushAwaiter&) = delete;

    ~QueuePushAwaiter()
    {
        if (handle_)
        {
            std::lock_guard<std::mutex> lock(state_->mutex_);
            auto& waiters = state_->pushers_;
            waiters.erase(std::remove(waiters.begin(), waiters.end(), handle_), waiters.end());
        }
    }

    /**
     * @brief Returns whether room is available without suspending.
     *
     * @return true for closed/unbounded queues or when below capacity.
     */
    [[nodiscard]]
    bool await_ready() const noexcept
    {
        std::lock_guard<std::mutex> lock(state_->mutex_);
        return state_->closed_ || state_->capacity_ == 0
            || state_->items_.size() < state_->capacity_;
    }

    bool await_suspend(std::coroutine_handle<> h) noexcept
    {
        handle_ = h;
        std::lock_guard<std::mutex> lock(state_->mutex_);
        if (state_->closed_ || state_->capacity_ == 0
            || state_->items_.size() < state_->capacity_)
        {
            return false; // Room now; await_resume enqueues.
        }
        state_->pushers_.push_back(h);
        return true;
    }

    void await_resume()
    {
        std::vector<std::coroutine_handle<>> to_resume;
        {
            std::lock_guard<std::mutex> lock(state_->mutex_);
            if (state_->closed_)
            {
                throw std::logic_error("async::AsyncQueue: push on closed queue");
            }
            state_->items_.push_back(std::move(value_));
            if (!state_->poppers_.empty())
            {
                to_resume.push_back(state_->poppers_.front());
                state_->poppers_.erase(state_->poppers_.begin());
            }
        }
        for (auto h : to_resume)
        {
            h.resume();
        }
    }

  private:
    std::shared_ptr<AsyncQueueState<T>> state_;
    T value_;
    std::coroutine_handle<> handle_{};
};

} // namespace detail

/**
 * @brief Thread-safe async producer-consumer queue.
 *
 * push() enqueues a value (the returned task must be awaited or discarded);
 * pop() suspends until a value is available. With a non-zero capacity the
 * queue is bounded and push() suspends when full until a popper frees room.
 * close() stops further pushes; poppers of a closed, empty queue throw
 * std::runtime_error, while remaining items can still be drained. Multiple
 * producers and consumers are supported.
 *
 * @tparam T Element type.
 */
template<typename T>
class AsyncQueue
{
  public:
    /**
     * @brief Constructs a queue.
     *
     * @param capacity Maximum queued items; 0 means unbounded.
     */
    explicit AsyncQueue(std::size_t capacity = 0) noexcept
      : state_(std::make_shared<detail::AsyncQueueState<T>>())
    {
        state_->capacity_ = capacity;
    }

    AsyncQueue(const AsyncQueue&) = delete;
    AsyncQueue& operator=(const AsyncQueue&) = delete;

    /**
     * @brief Enqueues a value, suspending when a bounded queue is full.
     *
     * @param value Value to enqueue; moved into the queue.
     * @return A task completing once the value is enqueued.
     */
    [[nodiscard]]
    Task<void> push(T value)
    {
        co_await detail::QueuePushAwaiter<T>{ state_, std::move(value) };
    }

    /**
     * @brief Enqueues without suspending when there is room.
     *
     * @param value Value to enqueue; moved into the queue.
     * @return true if enqueued, false if the queue is full or closed.
     */
    bool tryPush(T value)
    {
        std::vector<std::coroutine_handle<>> to_resume;
        {
            std::lock_guard<std::mutex> lock(state_->mutex_);
            if (state_->closed_)
            {
                return false;
            }
            if (state_->capacity_ != 0 && state_->items_.size() >= state_->capacity_)
            {
                return false;
            }
            state_->items_.push_back(std::move(value));
            if (!state_->poppers_.empty())
            {
                to_resume.push_back(state_->poppers_.front());
                state_->poppers_.erase(state_->poppers_.begin());
            }
        }
        for (auto h : to_resume)
        {
            h.resume();
        }
        return true;
    }

    /**
     * @brief Removes and returns the next value, suspending if empty.
     *
     * @return A task yielding the next value; throws if closed and empty.
     */
    [[nodiscard]]
    Task<T> pop()
    {
        co_return co_await detail::QueuePopAwaiter<T>{ state_ };
    }

    /**
     * @brief Removes the next value without suspending.
     *
     * @param out Receives the value on success.
     * @return true if a value was removed.
     */
    bool tryPop(T& out)
    {
        try
        {
            out = detail::popOne(state_);
            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
    }

    /**
     * @brief Closes the queue; further pushes throw, poppers drain then stop.
     *
     * Queued waiters are popped and resumed one at a time from the shared
     * lists, never holding a handle across a resume: a resumed waiter whose
     * completion destroys a still-suspended sibling unregisters it, so the
     * next iteration simply does not find it.
     */
    void close() noexcept
    {
        for (;;)
        {
            std::coroutine_handle<> h;
            {
                std::lock_guard<std::mutex> lock(state_->mutex_);
                state_->closed_ = true; // No new waiters register once closed.
                if (!state_->poppers_.empty())
                {
                    h = state_->poppers_.back();
                    state_->poppers_.pop_back();
                }
                else if (!state_->pushers_.empty())
                {
                    h = state_->pushers_.back();
                    state_->pushers_.pop_back();
                }
                else
                {
                    break;
                }
            }
            h.resume(); // Resume outside the lock.
        }
    }

    /**
     * @brief Returns whether the queue is closed.
     *
     * @return true if close() was called.
     */
    [[nodiscard]]
    bool isClosed() const
    {
        std::lock_guard<std::mutex> lock(state_->mutex_);
        return state_->closed_;
    }

  private:
    std::shared_ptr<detail::AsyncQueueState<T>> state_;
};

V_ASYNC_NS_END
