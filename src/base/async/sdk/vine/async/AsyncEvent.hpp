#pragma once

#include "async_global.hpp"

#include <cassert>
#include <coroutine>
#include <mutex>

V_ASYNC_NS_BEGIN

/**
 * @brief Thread-safe, manually reset async event with multiple waiters.
 *
 * A coroutine awaiting the event (co_await event) suspends until set() is
 * called. set() resumes every coroutine currently waiting and leaves the event
 * set (manual-reset semantics) until reset() clears it, after which new
 * awaiters suspend again. Thread-safe: the set flag and the intrusive waiter
 * list are guarded by an internal mutex; waiters are resumed on the thread
 * that called set(), outside the mutex.
 *
 * Lifecycle (framework contract, see async_global.hpp): the internal mutex
 * protects the set flag and the waiter list, NOT the coroutine frames. This
 * intrusive list stores raw Awaiter pointers that live in coroutine frames,
 * so it relies on the following contract:
 *   - the event must outlive every coroutine awaiting it;
 *   - a coroutine must not be destroyed concurrently with the set() that
 *     resumes it (no concurrent destroy during the resume window);
 *   - abandoning a wait (destroying the awaiting coroutine while queued) is
 *     safe: the Awaiter unregisters itself (queued_ guards the list);
 *   - resuming a waiter can synchronously destroy sibling waiters (a whenAny
 *     winner destroys still-suspended losers on completion), so set() never
 *     holds a waiter pointer across a resume.
 * The queued_ flag protects the LIST against concurrent destruction, not the
 * frame itself; frame lifetime is the caller's responsibility under the
 * contract above.
 */
class AsyncEvent
{
  public:
    AsyncEvent() noexcept = default;

    explicit AsyncEvent(bool initiallySet) noexcept : set_(initiallySet) {}

    AsyncEvent(const AsyncEvent&) = delete;
    AsyncEvent& operator=(const AsyncEvent&) = delete;

    ~AsyncEvent()
    {
        // Debug check: destroying an event with waiters still queued is a
        // lifetime violation — their ~Awaiter would later lock a destroyed
        // mutex_.
        assert(head_ == nullptr && tail_ == nullptr);
    }

    /**
     * @brief Sets the event and resumes every waiting coroutine.
     *
     * The event stays set until reset(). Each awaiting coroutine is resumed
     * outside the internal mutex, on the calling thread.
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

    /**
     * @brief Awaiter for co_await event; created fresh for each await.
     *
     * Non-copyable and non-movable: it lives in the awaiting coroutine frame
     * and is never reused. On destruction it unregisters itself from the
     * waiter list, so abandoning the await is safe.
     */
    class Awaiter
    {
      public:
        explicit Awaiter(AsyncEvent& event) noexcept : event_(event) {}

        Awaiter(const Awaiter&) = delete;
        Awaiter& operator=(const Awaiter&) = delete;
        Awaiter(Awaiter&&) = delete;
        Awaiter& operator=(Awaiter&&) = delete;

        ~Awaiter();

        /**
         * @brief Always defers to await_suspend for a single locked decision.
         *
         * @return false.
         */
        [[nodiscard]]
        bool await_ready() const noexcept
        {
            return false;
        }

        /**
         * @brief Registers this waiter and decides whether to suspend, all
         * under one lock.
         *
         * @param h The awaiting coroutine; stored to resume it on set().
         * @return false to resume inline when the event is already set.
         */
        bool await_suspend(std::coroutine_handle<> h) noexcept;

        void await_resume() const noexcept {}

      private:
        friend class AsyncEvent;

        AsyncEvent& event_;

        /// The awaiting coroutine; non-null while waiting or being resumed.
        std::coroutine_handle<> handle_{};

        /// true while this awaiter is linked into the waiter list.
        bool queued_{ false };

        /// List links; non-null while queued.
        Awaiter* next_{ nullptr };
        Awaiter* prev_{ nullptr };
    };

    /**
     * @brief Returns a fresh awaiter; co_await event suspends until set().
     *
     * @return An awaiter to be co_awaited immediately.
     */
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
    {
        std::lock_guard lock(mutex_);
        // Manual-reset: arm the flag exactly once, before any waiter is
        // resumed. Arming it inside the resume loop below would overwrite a
        // reset() issued by a resumed waiter (e.g. an edge-triggered event
        // clearing itself for the next batch), so it must happen here.
        set_ = true;
    }
    for (;;)
    {
        Awaiter* a = nullptr;
        {
            std::lock_guard lock(mutex_);
            if (head_)
            {
                a = head_;
                head_ = a->next_;
                if (!head_)
                {
                    tail_ = nullptr;
                }
                else
                {
                    head_->prev_ = nullptr;
                }
                a->next_ = nullptr;
                a->prev_ = nullptr;
                a->queued_ = false;
            }
        }
        if (!a)
        {
            break;
        }
        assert(a->handle_);
        a->handle_.resume(); // Resume outside the lock.
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

inline bool AsyncEvent::Awaiter::await_suspend(std::coroutine_handle<> h) noexcept
{
    assert(h);
    handle_ = h;
    std::lock_guard lock(event_.mutex_);
    if (event_.set_)
    {
        return false; // Already set: resume inline without suspending.
    }
    event_.enqueue(*this); // Park in the waiter list until set().
    return true;
}

inline AsyncEvent::Awaiter::~Awaiter()
{
    std::lock_guard lock(event_.mutex_);
    if (queued_)
    {
        event_.dequeue(*this);
    }
}

inline void AsyncEvent::enqueue(Awaiter& a) noexcept
{
    assert(!a.queued_);
    a.queued_ = true;
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
    assert(a.queued_);
    if (a.prev_)
    {
        a.prev_->next_ = a.next_;
    }
    else
    {
        assert(head_ == &a);
        head_ = a.next_;
    }

    if (a.next_)
    {
        a.next_->prev_ = a.prev_;
    }
    else
    {
        assert(tail_ == &a);
        tail_ = a.prev_;
    }

    a.prev_ = nullptr;
    a.next_ = nullptr;
    a.queued_ = false;
}

V_ASYNC_NS_END
