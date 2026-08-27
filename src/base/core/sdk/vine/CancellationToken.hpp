#pragma once

#include "core_global.hpp"

#include <atomic>
#include <memory>

V_CORE_NS_BEGIN

/**
 * @brief A shareable cancellation flag.
 *
 * Copies of a CancellationToken refer to the same underlying flag, so cancelling
 * through any copy is observed by all copies. It is safe to use from multiple
 * threads.
 */
class V_CORE_API CancellationToken
{
  public:
    CancellationToken();

    /**
     * @brief Requests cancellation.
     */
    void cancel() noexcept;

    /**
     * @brief Returns whether cancellation has been requested.
     *
     * @return true if cancellation was requested.
     */
    [[nodiscard]]
    bool isCancellationRequested() const noexcept;

    /**
     * @brief Clears the cancellation request.
     */
    void reset() noexcept;

  private:
    std::shared_ptr<std::atomic<bool>> state_;
};

V_CORE_NS_END
