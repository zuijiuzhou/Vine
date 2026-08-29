#pragma once

#include "co_global.hpp"

#include <coroutine>
#include <mutex>

V_CO_NS_BEGIN

/**
 * @brief Thread-safe, manually reset async event with multiple waiters.
 *
 * A coroutine awaiting the event suspends until set() is called. set() resumes
 * every coroutine currently waiting and leaves the event set until reset().
 * The event must outlive every coroutine currently awaiting it.
 */
class AsyncEvent
{
  public:
    AsyncEvent() noexcept = default;

    explicit AsyncEvent(bool initiallySet) noexcept : set_(initiallySet) {}

    AsyncEvent(const AsyncEvent&) = delete;
    AsyncEvent& operator=(const AsyncEvent&) = delete;

    ~AsyncEvent() = default;

    /**
     * @brief Sets the event and resumes every waiting coroutine.
     */
    void set() noexcept;

    /**
     * @brief Clears the event so that future awaiters suspend.
     */
    void reset() noexcept;

    /**
     * @brief Returns whether the event is currently set.
     *
     * @return true if the event is set.
     */
    [[nodiscard]]
    bool isSet() const noexcept;

    class Awaiter
    {
      public:
        explicit Awaiter(AsyncEvent& event) noexcept : event_(event) {}

        Awaiter(const Awaiter&) = delete;
        Awaiter& operator=(const Awaiter&) = delete;
        Awaiter(Awaiter&&) = delete;
        Awaiter& operator=(Awaiter&&) = delete;

        ~Awaiter();

        [[nodiscard]]
        bool await_ready() const noexcept;

        bool await_suspend(std::coroutine_handle<> h) noexcept;

        void await_resume() const noexcept {}

      private:
        friend class AsyncEvent;

        AsyncEvent& event_;
        std::coroutine_handle<> handle_{};
        Awaiter* next_{ nullptr };
        Awaiter* prev_{ nullptr };
    };

    [[nodiscard]]
    Awaiter operator co_await() noexcept
    {
        return Awaiter{ *this };
    }

  private:
    void enqueue(Awaiter& a) noexcept;
    void dequeue(Awaiter& a) noexcept;

    mutable std::mutex mutex_;
    bool set_{ false };
    Awaiter* head_{ nullptr };
    Awaiter* tail_{ nullptr };
};

inline void AsyncEvent::set() noexcept
{
    Awaiter* first = nullptr;
    {
        std::lock_guard lock(mutex_);
        set_ = true;
        // Detach the whole waiter list and resume outside the lock.
        first = head_;
        head_ = nullptr;
        tail_ = nullptr;
    }

    while (first)
    {
        auto* next = first->next_;
        first->prev_ = nullptr;
        first->next_ = nullptr;
        first->handle_.resume(); // Resume on the thread that called set().
        first = next;
    }
}

inline void AsyncEvent::reset() noexcept
{
    std::lock_guard lock(mutex_);
    set_ = false;
}

inline bool AsyncEvent::isSet() const noexcept
{
    std::lock_guard lock(mutex_);
    return set_;
}

inline bool AsyncEvent::Awaiter::await_ready() const noexcept
{
    std::lock_guard lock(event_.mutex_);
    return event_.set_;
}

inline bool AsyncEvent::Awaiter::await_suspend(std::coroutine_handle<> h) noexcept
{
    handle_ = h;
    std::lock_guard lock(event_.mutex_);
    if (event_.set_)
    {
        return false; // Already set: do not suspend.
    }
    event_.enqueue(*this); // Park in the waiter list until set().
    return true;
}

inline AsyncEvent::Awaiter::~Awaiter()
{
    std::lock_guard lock(event_.mutex_);
    event_.dequeue(*this);
}

inline void AsyncEvent::enqueue(Awaiter& a) noexcept
{
    a.prev_ = tail_;
    a.next_ = nullptr;
    if (tail_)
    {
        tail_->next_ = &a;
    }
    else
    {
        head_ = &a;
    }
    tail_ = &a;
}

inline void AsyncEvent::dequeue(Awaiter& a) noexcept
{
    if (a.prev_)
    {
        a.prev_->next_ = a.next_;
    }
    else if (head_ == &a)
    {
        head_ = a.next_;
    }
    else
    {
        return;
    }

    if (a.next_)
    {
        a.next_->prev_ = a.prev_;
    }
    else if (tail_ == &a)
    {
        tail_ = a.prev_;
    }

    a.prev_ = nullptr;
    a.next_ = nullptr;
}

V_CO_NS_END
