#pragma once

#include "async_global.hpp"

#include <coroutine>
#include <exception>

V_ASYNC_NS_BEGIN

namespace detail {

/**
 * @brief Handler invoked when a detached coroutine throws.
 *
 * If null the process is terminated; set it to log instead of aborting.
 */
using DetachedExceptionHandler = void (*)(const std::exception_ptr&) noexcept;

inline DetachedExceptionHandler s_detachedExceptionHandler = nullptr;

} // namespace detail

/**
 * @brief Eager, fire-and-forget coroutine.
 *
 * Unlike Task, initial_suspend() is suspend_never, so the body starts
 * immediately when the coroutine function is called and the caller keeps no
 * handle. final_suspend() is also suspend_never, so the frame destroys itself
 * on completion; the returned DetachedTask is only an empty marker used for
 * the coroutine's return type.
 *
 * Exceptions invoke the handler set by setDetachedExceptionHandler, or
 * terminate the process when no handler is installed.
 */
class DetachedTask
{
  public:
    struct promise_type
    {
        /// Returns an empty marker: nobody keeps a handle to a detached task.
        [[nodiscard]]
        DetachedTask get_return_object() noexcept
        {
            return {};
        }

        /// Start the body immediately (no explicit resume needed).
        std::suspend_never initial_suspend() noexcept { return {}; }

        /// Do not suspend at completion: the frame destroys itself.
        std::suspend_never final_suspend() noexcept { return {}; }

        void return_void() noexcept {}

        void unhandled_exception() noexcept
        {
            if (detail::s_detachedExceptionHandler)
            {
                detail::s_detachedExceptionHandler(std::current_exception());
            }
            else
            {
                std::terminate();
            }
        }
    };

    DetachedTask() = default;
    DetachedTask(const DetachedTask&) = delete;
    DetachedTask& operator=(const DetachedTask&) = delete;
    DetachedTask(DetachedTask&&) = default;
    DetachedTask& operator=(DetachedTask&&) = default;
};

/**
 * @brief Sets the handler invoked when a detached coroutine throws.
 *
 * @param handler Callback receiving the captured exception. It may log and
 * return, in which case the exception is swallowed and the coroutine completes.
 */
inline void setDetachedExceptionHandler(detail::DetachedExceptionHandler handler) noexcept
{
    detail::s_detachedExceptionHandler = handler;
}

V_ASYNC_NS_END
