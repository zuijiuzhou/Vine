#pragma once

#include "co_global.hpp"

#include <exception>
#include <stop_token>

V_CO_NS_BEGIN

/**
 * @brief Token that can be polled to observe a cancellation request.
 *
 * Alias of std::stop_token. Awaitable operations accept an optional token and
 * throw OperationCanceled from await_resume when cancellation was requested
 * before the operation completed.
 */
using CancellationToken = std::stop_token;

/**
 * @brief Source that owns the cancellation state and can request cancellation.
 */
using CancellationSource = std::stop_source;

/**
 * @brief Exception thrown when a cancellable operation observes cancellation.
 */
class OperationCanceled : public std::exception
{
  public:
    [[nodiscard]]
    const char* what() const noexcept override
    {
        return "operation canceled";
    }
};

V_CO_NS_END
