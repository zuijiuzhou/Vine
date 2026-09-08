#pragma once

#include "async_global.hpp"

#include <coroutine>

V_ASYNC_NS_BEGIN

/**
 * @brief Constrains a type to the minimal awaiter protocol.
 *
 * A type satisfying this concept can be awaited with co_await. Use it to
 * constrain custom awaiters or types accepted by generic APIs.
 */
template<typename T>
concept Awaitable = requires(T&& a) {
    a.await_ready();
    a.await_suspend(std::coroutine_handle<>{});
    a.await_resume();
};

/**
 * @brief Constrains a type that can decide where a coroutine resumes.
 *
 * A Schedulable type exposes a callable schedule() returning an awaitable
 * object. Awaiting that object suspends the coroutine and later resumes it in
 * the execution context chosen by the type.
 */
template<typename T>
concept Schedulable = requires(T& s) {
    { s.schedule() } -> Awaitable;
};

V_ASYNC_NS_END
