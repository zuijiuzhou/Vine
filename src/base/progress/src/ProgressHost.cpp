#include <vine/progress/ProgressHost.hpp>

#include <algorithm>
#include <mutex>

#include <vine/progress/ProgressRange.hpp>
#include <vine/progress/ProgressScope.hpp>

V_PROGRESS_NS_BEGIN

namespace
{

/// Guards the active-host registry (hosts register/deregister on any thread).
std::mutex                 s_mutex;
/// All active hosts (foreground stack + background), registration order.
std::vector<ProgressHost*> s_active;
/// The foreground stack: nested LongRunning hosts, outermost first.
std::vector<ProgressHost*> s_foreground_stack;

/// Removes this host from the foreground stack if present.
void removeFromForegroundStack(ProgressHost* host)
{
    auto it = std::find(s_foreground_stack.begin(), s_foreground_stack.end(), host);
    if (it != s_foreground_stack.end()) {
        s_foreground_stack.erase(it);
    }
}

} // namespace

ProgressHost::ProgressHost()
  : indicator_(stop_source_.get_token())
{
    std::lock_guard lock(s_mutex);
    s_active.push_back(this);
}

ProgressHost::ProgressHost(std::stop_source source)
  : stop_source_(std::move(source))
  , indicator_(stop_source_.get_token())
{
    std::lock_guard lock(s_mutex);
    s_active.push_back(this);
}

ProgressHost::~ProgressHost()
{
    std::lock_guard lock(s_mutex);
    auto it = std::find(s_active.begin(), s_active.end(), this);
    if (it != s_active.end()) {
        s_active.erase(it);
    }
    // Removing from the stack restores the previous foreground host.
    removeFromForegroundStack(this);
}

ProgressHost* ProgressHost::current()
{
    std::lock_guard lock(s_mutex);
    return s_foreground_stack.empty() ? nullptr : s_foreground_stack.back();
}

bool ProgressHost::isActive()
{
    std::lock_guard lock(s_mutex);
    return !s_active.empty();
}

std::vector<ProgressHost*> ProgressHost::activeHosts()
{
    std::lock_guard lock(s_mutex);
    return s_active;
}

std::vector<ProgressHost*> ProgressHost::foregroundStack()
{
    std::lock_guard lock(s_mutex);
    return s_foreground_stack;
}

void ProgressHost::setForeground(bool fg)
{
    std::lock_guard lock(s_mutex);
    if (fg) {
        auto it = std::find(s_foreground_stack.begin(), s_foreground_stack.end(), this);
        if (it == s_foreground_stack.end()) {
            s_foreground_stack.push_back(this);
        }
    }
    else {
        removeFromForegroundStack(this);
    }
}

bool ProgressHost::isForeground() const
{
    std::lock_guard lock(s_mutex);
    return std::find(s_foreground_stack.begin(), s_foreground_stack.end(), this) != s_foreground_stack.end();
}

ProgressIndicator& ProgressHost::indicator()
{
    return indicator_;
}

const ProgressIndicator& ProgressHost::indicator() const
{
    return indicator_;
}

std::stop_source& ProgressHost::cancelSource()
{
    return stop_source_;
}

const std::stop_source& ProgressHost::cancelSource() const
{
    return stop_source_;
}

ProgressRange ProgressHost::range()
{
    return indicator_.start();
}

ProgressScope ProgressHost::scope(const std::string& name, double max)
{
    // The root range from range() is consumed by exactly this scope, so the
    // caller never touches the one-shot ProgressRange directly.
    return ProgressScope(range(), name, max);
}

void ProgressHost::setLabel(const std::string& label)
{
    label_ = label;
}

const std::string& ProgressHost::label() const
{
    return label_;
}

V_PROGRESS_NS_END
