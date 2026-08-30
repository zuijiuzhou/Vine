#pragma once

#include <vine/appfw/appfw_global.hpp>

#include <coroutine>
#include <chrono>
#include <functional>
#include <memory>
#include <stop_token>

#include <QObject>
#include <QTimer>

#include <vine/async/Cancellation.hpp>

namespace vine {
namespace appfw {
namespace async {

/**
 * @brief Awaitable that suspends the coroutine for a duration using QTimer.
 *
 * The timer lives on the thread that evaluates the co_await, which must run an
 * event loop. When a stop token is given and cancellation is requested, the
 * coroutine resumes early and await_resume throws vine::async::TaskCancelledException.
 */
class V_APPFW_API SleepAwaiter
{
  public:
    SleepAwaiter(std::chrono::milliseconds duration, std::stop_token token);

    SleepAwaiter(const SleepAwaiter&) = delete;
    SleepAwaiter& operator=(const SleepAwaiter&) = delete;
    SleepAwaiter(SleepAwaiter&&) = delete;
    SleepAwaiter& operator=(SleepAwaiter&&) = delete;

    ~SleepAwaiter() = default;

    [[nodiscard]]
    bool await_ready() const noexcept;

    void await_suspend(std::coroutine_handle<> h);

    void await_resume() const;

  private:
    struct CancelRegistration
    {
        std::stop_callback<std::function<void()>> callback;

        template<typename F>
        CancelRegistration(std::stop_token token, F&& f)
            : callback(std::move(token), std::forward<F>(f))
        {
        }
    };

    void onCancel() noexcept;
    void resumeIfPending() noexcept;

    std::stop_token token_;
    std::unique_ptr<QTimer> timer_;
    std::unique_ptr<CancelRegistration> cancel_;
    std::coroutine_handle<> handle_{};
};

/**
 * @brief Suspends the coroutine for the given duration.
 *
 * @param duration Time to sleep.
 * @param token Optional cancellation token.
 * @return An awaitable implementing the sleep.
 */
V_APPFW_API SleepAwaiter sleep(std::chrono::milliseconds duration, std::stop_token token = {});

} // namespace async
} // namespace appfw
} // namespace vine
