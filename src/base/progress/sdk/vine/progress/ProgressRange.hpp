#pragma once

#include "progress_global.hpp"

#include <memory>

V_PROGRESS_NS_BEGIN

class ProgressScope;

/**
 * @brief A shareable portion of the global progress scale.
 *
 * A range is allocated by a scope step or the indicator root and reports its
 * portion to the indicator exactly once: when complete() is called, or when
 * the last handle to it is destroyed. Copies share the same portion, so a
 * range can be passed around freely without double reporting; a ProgressScope
 * constructed from a range takes over the reporting responsibility.
 */
class V_PROGRESS_API ProgressRange
{
  public:
    friend class ProgressScope;

  public:
    ProgressRange();

    ProgressRange(const ProgressRange&)            = default;
    ProgressRange& operator=(const ProgressRange&) = default;

    ProgressRange(ProgressRange&&) noexcept            = default;
    ProgressRange& operator=(ProgressRange&&) noexcept = default;

    ~ProgressRange() = default;

  private:
    ProgressRange(const ProgressScope& parent, double start, double delta);

  public:
    /**
     * @brief Returns whether the range can still advance the progress.
     *
     * @return true if the range is attached to an indicator and not yet
     *         completed or handed over to a scope.
     */
    bool isActive() const;

    /**
     * @brief Returns whether the user requested a break.
     *
     * @return true if the operation should stop.
     */
    bool isCancelled() const;

    /**
     * @brief Completes the range and advances the indicator by its portion.
     *
     * Reporting happens at most once regardless of how many handles share the
     * range; completing through any handle disarms all of them.
     */
    void complete();

  private:
    struct State;

    std::shared_ptr<State> state_;
};

V_PROGRESS_NS_END
