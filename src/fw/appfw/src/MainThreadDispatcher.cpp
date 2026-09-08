#include <vine/appfw/MainThreadDispatcher.hpp>

#include <utility>

#include <QCoreApplication>
#include <QMetaObject>
#include <QThread>

V_APPFW_NS_BEGIN

bool MainThreadDispatcher::isMainThread() const
{
    const auto* app = QCoreApplication::instance();
    return app == nullptr || QThread::currentThread() == app->thread();
}

void MainThreadDispatcher::postToMain(std::function<void()> task)
{
    auto* app = QCoreApplication::instance();
    if (app == nullptr) {
        task();  // No event loop yet: degrade to synchronous delivery.
        return;
    }
    QMetaObject::invokeMethod(app, std::move(task), Qt::QueuedConnection);
}

V_APPFW_NS_END
