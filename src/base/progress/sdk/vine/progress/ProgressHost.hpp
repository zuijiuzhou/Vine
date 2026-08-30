#pragma once

#include "progress_global.hpp"

#include <stop_token>
#include <string>
#include <vector>

#include <vine/progress/ProgressIndicator.hpp>

V_PROGRESS_NS_BEGIN

class ProgressRange;
class ProgressScope;

/**
 * @brief Ambient per-operation progress host (RAII).
 *
 * A host wraps the progress state of one long-running operation: a
 * ProgressIndicator, an owned cancellation source and an optional stage label.
 * It registers itself in a process-wide registry for the duration of the
 * operation, so code running inside the operation can reach it and the UI can
 * observe all running operations.
 *
 * Multiple hosts may be active concurrently. LongRunning operations form a
 * foreground stack: nested LongRunning commands push their host on top of the
 * parent's, so the innermost running command drives the main progress bar, and
 * when it ends the parent's host is restored automatically. Hosts outside the
 * stack are background hosts that run in parallel and are shown as a compact
 * indicator. New hosts are background by default; the command layer promotes
 * LongRunning commands with setForeground(true).
 *
 * The host performs no presentation; the UI layer (e.g. a progress presenter)
 * polls current()/activeHosts() and the indicators from the main thread.
 * Position updates are thread-safe, so the operation may report progress from
 * a worker thread while the presenter renders on the main thread.
 *
 * Operations report progress by carving scopes out of the root scale. Prefer
 * the scope() helper, which hides the one-shot ProgressRange entirely:
 *
 *     if (auto* host = ProgressHost::current()) {
 *         ProgressScope scope = host->scope("Exporting", count);
 *         for (...) { scope.next(1); ... }
 *     }
 */
class V_PROGRESS_API ProgressHost
{
  public:
    /**
     * @brief Constructs a background host owning its own cancellation source.
     */
    ProgressHost();

    /**
     * @brief Constructs a background host observing an external source.
     *
     * The indicator binds to the source's token, and cancelSource() refers to
     * the same shared stop state, so requesting a stop through either the
     * source or the host cancels the operation. Used by the command layer so
     * the UI cancel button and the command's own stop token stay in sync.
     *
     * @param source Cancellation source shared with the operation.
     */
    explicit ProgressHost(std::stop_source source);

    ~ProgressHost();

    ProgressHost(const ProgressHost&)            = delete;
    ProgressHost& operator=(const ProgressHost&) = delete;

    /**
     * @brief Returns the top of the foreground stack, or nullptr when empty.
     *
     * This is the innermost running LongRunning operation — the one whose
     * progress drives the main bar. Safe to call from any thread.
     *
     * @return The current foreground host, or nullptr.
     */
    static ProgressHost* current();

    /**
     * @brief Returns whether any host is currently active.
     *
     * Includes foreground-stack and background hosts.
     *
     * @return true if at least one operation is running.
     */
    static bool isActive();

    /**
     * @brief Returns a snapshot of all active hosts.
     *
     * Order is unspecified but stable for the duration of the snapshot; hosts
     * in the foreground stack and background hosts are all included.
     *
     * @return All active hosts.
     */
    static std::vector<ProgressHost*> activeHosts();

    /**
     * @brief Returns the foreground stack, outermost first.
     *
     * The stack contains the chain of nested LongRunning commands; hosts in it
     * are part of one operation, not parallel background work. The UI uses it
     * to tell in-chain hosts apart from genuine background hosts and to render
     * a chain breadcrumb.
     *
     * @return The foreground stack, outermost first.
     */
    static std::vector<ProgressHost*> foregroundStack();

    /**
     * @brief Pushes this host onto / removes it from the foreground stack.
     *
     * Promoting pushes on top of the current foreground (so a nested child
     * takes over the bar); demoting removes it from the stack. The host
     * destructor also removes it, restoring the previous foreground.
     *
     * @param fg true to push, false to remove.
     */
    void setForeground(bool fg = true);

    /**
     * @brief Returns whether this host is part of the foreground stack.
     *
     * true for in-chain LongRunning hosts; false for background hosts.
     *
     * @return true if this host is in the foreground stack.
     */
    bool isForeground() const;

    /**
     * @brief Returns the progress indicator backing this host.
     *
     * @return The indicator.
     */
    ProgressIndicator& indicator();

    /**
     * @brief Returns the progress indicator backing this host.
     *
     * @return The indicator.
     */
    const ProgressIndicator& indicator() const;

    /**
     * @brief Returns the cancellation source of this operation.
     *
     * The UI cancel button calls request_stop() on it; the operation observes
     * the request through ProgressScope::isCancelled() or indicator().token().
     *
     * @return The cancellation source.
     */
    std::stop_source& cancelSource();

    /**
     * @brief Returns the cancellation source of this operation.
     *
     * @return The cancellation source.
     */
    const std::stop_source& cancelSource() const;

    /**
     * @brief Returns the root range covering the whole [0, 1] scale.
     *
     * Pass it to a ProgressScope to map the operation's stages onto the global
     * progress. The range is one-shot: ProgressRange uses move-on-copy
     * semantics, so it can be consumed by exactly one ProgressScope, and every
     * call resets the indicator. Call it once at the start of the operation
     * and prefer scope() for the common top-level-stage case.
     *
     * @return The root progress range.
     */
    ProgressRange range();

    /**
     * @brief Creates the top-level scope covering the whole [0, 1] scale.
     *
     * Preferred over range(): it hides the one-shot ProgressRange entirely, so
     * the operation cannot trip over its move-on-copy semantics. The returned
     * scope maps its local [0, max] range onto the global scale; nest further
     * scopes inside it for sub-stages.
     *
     * @param name Stage label.
     * @param max Local range length.
     * @return The operation's top-level progress scope.
     */
    ProgressScope scope(const std::string& name = {}, double max = 1.0);

    /**
     * @brief Sets a human-readable label describing the current stage.
     *
     * @param label Stage label, may be empty.
     */
    void setLabel(const std::string& label);

    /**
     * @brief Returns the current stage label.
     *
     * @return The stage label.
     */
    const std::string& label() const;

  private:
    // stop_source_ is declared first so the indicator can bind to its token.
    std::stop_source stop_source_;
    ProgressIndicator indicator_;
    std::string label_;
};

V_PROGRESS_NS_END
