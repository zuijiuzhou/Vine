#include "Scheduler.hpp"

#include <QMetaObject>

namespace vine {
namespace appfw {
namespace co {

Scheduler::Scheduler(QObject* parent)
    : QObject(parent)
{
}

Scheduler::~Scheduler() = default;

void Scheduler::postResume(std::coroutine_handle<> h)
{
    // Queued invocation runs on this object's thread once its event loop turns.
    QMetaObject::invokeMethod(this, [h] { h.resume(); }, Qt::QueuedConnection);
}

} // namespace co
} // namespace appfw
} // namespace vine
