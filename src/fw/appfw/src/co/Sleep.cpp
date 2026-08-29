#include "Sleep.hpp"

#include <QMetaObject>

#include <utility>

namespace vine {
namespace appfw {
namespace co {

SleepAwaiter::SleepAwaiter(std::chrono::milliseconds duration, std::stop_token token)
    : token_(std::move(token))
    , timer_(std::make_unique<QTimer>())
{
    timer_->setSingleShot(true);
    timer_->setInterval(static_cast<int>(duration.count()));
}

bool SleepAwaiter::await_ready() const noexcept
{
    return token_.stop_requested();
}

void SleepAwaiter::await_suspend(std::coroutine_handle<> h)
{
    handle_ = h;

    if (token_.stop_possible())
    {
        cancel_ = std::make_unique<CancelRegistration>(token_, [this]() noexcept { onCancel(); });
    }

    QObject::connect(timer_.get(), &QTimer::timeout, timer_.get(), [this] { resumeIfPending(); });

    if (token_.stop_requested())
    {
        return;
    }

    timer_->start();
}

void SleepAwaiter::await_resume() const
{
    if (token_.stop_requested())
    {
        throw vine::co::OperationCanceled{};
    }
}

void SleepAwaiter::onCancel() noexcept
{
    QMetaObject::invokeMethod(timer_.get(), [this] { resumeIfPending(); }, Qt::QueuedConnection);
}

void SleepAwaiter::resumeIfPending() noexcept
{
    if (auto h = std::exchange(handle_, {}))
    {
        h.resume();
    }
}

SleepAwaiter sleep(std::chrono::milliseconds duration, std::stop_token token)
{
    return SleepAwaiter{ duration, std::move(token) };
}

} // namespace co
} // namespace appfw
} // namespace vine
