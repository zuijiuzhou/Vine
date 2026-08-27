#pragma once

#include "progress_global.hpp"

#include <mutex>

#include <vine/CancellationToken.hpp>

V_PROGRESS_NS_BEGIN

class ProgressRange;
class ProgressScope;

/**
 * @brief Concrete progress indicator.
 *
 * Tracks the global progress position in [0, 1] and a cancellation token. It
 * performs no presentation of its own; callers poll position() and drive
 * cancellation through the token. Progress increments are thread-safe.
 */
class V_PROGRESS_API ProgressIndicator
{
  public:
    friend class ProgressRange;
    friend class ProgressScope;

  public:
    ProgressIndicator();

    ~ProgressIndicator();

    /**
     * @brief Resets the indicator and returns the root progress range.
     *
     * @return The root range covering the whole [0, 1] scale.
     */
    ProgressRange start();

    /**
     * @brief Returns the root range of the indicator, or an empty range if it is null.
     *
     * @param indicator Indicator, may be nullptr.
     * @return The root range, or an empty range.
     */
    static ProgressRange start(ProgressIndicator* indicator);

    /**
     * @brief Returns the current global progress position.
     *
     * @return Overall progress in [0, 1].
     */
    double position() const;

    /**
     * @brief Returns whether cancellation has been requested.
     *
     * @return true if the operation should stop.
     */
    bool isCanceled() const;

    /**
     * @brief Returns the cancellation token.
     *
     * Call cancel() on the token to request cancellation.
     *
     * @return The cancellation token.
     */
    vine::CancellationToken& cancellationToken();

    /**
     * @brief Returns the cancellation token.
     *
     * @return The cancellation token.
     */
    const vine::CancellationToken& cancellationToken() const;

    /**
     * @brief Requests cancellation.
     */
    void cancel();

  private:
    void increment(double step);

    double                  position_{0.0};

    std::mutex              mutex_;

    ProgressScope*          root_scope_{nullptr};

    vine::CancellationToken token_;
};

V_PROGRESS_NS_END
