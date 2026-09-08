#pragma once

#include "progress_global.hpp"

#include <atomic>
#include <stop_token>

V_PROGRESS_NS_BEGIN

class ProgressRange;
class ProgressScope;

/**
 * @brief Concrete progress indicator.
 *
 * Tracks the global progress position in [0, 1] and observes a cancellation
 * state supplied from outside as a std::stop_token. It performs no
 * presentation of its own; callers poll position() and observe cancellation
 * through isCancelled() or token(). Cancellation is requested through the
 * external std::stop_source that produced the token. position() and progress
 * increments are thread-safe; the scope/range graph is single-threaded.
 */
class V_PROGRESS_API ProgressIndicator
{
  public:
    friend class ProgressRange;
    friend class ProgressScope;

  public:
    /**
     * @brief Constructs an indicator observing an externally-owned token.
     *
     * The indicator never requests cancellation itself; request_stop() is
     * called on the external std::stop_source that produced the token. An
     * empty token (the default) is never cancelled.
     *
     * @param token Token observing the cancellation state.
     */
    explicit ProgressIndicator(std::stop_token token = {});

    ~ProgressIndicator();

    /**
     * @brief Resets the progress and returns the root range covering the whole
     *        scale.
     *
     * The returned range may be completed to advance the indicator to its end
     * or passed to a ProgressScope to carve out sub-stages. The indicator and
     * its root scope are reset on every call, so start() may be called again
     * to begin a new run and always returns a fresh range covering the whole
     * [0, 1] scale. Cancellation is owned externally and is not reset here;
     * use a fresh source/token for a new operation when a clean cancellation
     * state is required.
     *
     * @return The root range covering the whole [0, 1] scale.
     */
    ProgressRange start();

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
    bool isCancelled() const;

    /**
     * @brief Returns the cancellation token.
     *
     * The token observes the same cancellation state as isCancelled(); it can
     * be polled from other threads or used to register std::stop_callbacks.
     *
     * @return The std token backing this indicator's cancellation state.
     */
    std::stop_token token() const;

  private:
    void increment(double step);

    std::atomic<double>     position_{0.0};

    ProgressScope*          root_scope_{nullptr};

    std::stop_token         token_;
};

V_PROGRESS_NS_END
