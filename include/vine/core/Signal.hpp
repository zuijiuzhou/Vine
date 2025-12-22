#pragma once
#include <functional>
#include <map>

#include "core_global.hpp"

VI_CORE_NS_BEGIN

template <typename... TArgs>
class Signal {
  public:
    using Handler   = std::function<void(TArgs...)>;
    using HandlerId = size_t;

  public:
    Signal()              = default;
    Signal(const Signal&) = delete;
    Signal(Signal&&)      = delete;

  public:
    HandlerId addHandler(const Handler& handler)
    {
        auto id = ++last_id_;
        handlers_.emplace_back(id, handler);
        return id;
    }

    void removeHandler(HandlerId id)
    {
        handlers_.erase(id);
    }

    void setBlocked(bool blocked)
    {
        is_blocked_ = blocked;
    }

    bool isBlocked() const
    {
        return is_blocked_;
    }

    void emit(TArgs... args)
    {
        for (auto& handler : handlers_) {
            handler(std::forward<TArgs>(args)...);
        }
    }

    void clear()
    {
        handlers_.clear();
    }

  private:
    std::map<HandlerId, Handler> handlers_;
    HandlerId                    last_id_{ 0 };
    bool                         is_blocked_{ false };
};

VI_CORE_NS_END