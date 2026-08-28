#pragma once

#include "appfw_global.hpp"

#include <functional>

V_APPFW_NS_BEGIN

/**
 * @brief Main-thread marshaller for EventBus Main/Auto delivery.
 *
 * Concrete (non-virtual) Qt-backed implementation, owned by Application.
 * EventBus fetches the Application-owned instance via Application::current()
 * when posting; it is not exposed in EventBus's public API.
 */
class V_APPFW_API MainThreadDispatcher {
  public:
    MainThreadDispatcher() = default;
    ~MainThreadDispatcher() = default;

    /// true if the calling thread is the main thread.
    bool isMainThread() const;

    /// Posts task to be executed on the main thread.
    void postToMain(std::function<void()> task);
};

V_APPFW_NS_END
