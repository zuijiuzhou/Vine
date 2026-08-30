#pragma once

#include <vine/vi_global.hpp>

/**
 * @file async_global.hpp
 * @brief Framework-wide conventions for the vine::async coroutine module.
 *
 * Lifetime contract: a coroutine suspended at a co_await must not be
 * destroyed concurrently by another thread while the awaited object is
 * completing it (between the completion thread swapping out the waiter list
 * and resuming each waiter). Abandoning an await — destroying the awaiting
 * coroutine before completion — is supported: every awaiter unregisters
 * itself on destruction. But concurrent destruction during completion is
 * undefined behavior, exactly as in cppcoro/folly.
 */

#define V_ASYNC_NS_BEGIN \
    namespace vine \
    { \
    namespace async \
    {

#define V_ASYNC_NS_END \
    } \
    }
