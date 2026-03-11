#pragma once
#include "core_global.hpp"

#include <functional>
#include <map>
#include <utility>
#include <vector>

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
    HandlerId addHandler(Handler handler)
    {
        auto id = ++last_id_;
        handlers_.emplace(id, std::move(handler));
        return id;
    }

    void removeHandler(HandlerId id)
    {
        handlers_.erase(id);
    }

    /**
     * @brief 设置当前信号是否被阻塞
     * @param val
     * @return 返回之前的状态
     */
    bool setBlocked(bool val)
    {
        const auto blocked = is_blocked_;
        is_blocked_        = val;
        return blocked;
    }

    bool isBlocked() const
    {
        return is_blocked_;
    }

    void emit(TArgs... args)
    {
        if (is_blocked_) {
            return;
        }

        // copy handlers to avoid issues with handlers being added/removed during emit
        std::vector<HandlerId> snapshot_ids;
        snapshot_ids.reserve(handlers_.size());

        for (const auto& [id, _] : handlers_) {
            snapshot_ids.push_back(id);
        }

        for (const auto id : snapshot_ids) {
            auto it = handlers_.find(id);
            if (it != handlers_.end()) {
                it->second(args...);
            }
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
