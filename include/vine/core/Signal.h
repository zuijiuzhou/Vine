#pragma once
#include <list>
#include <functional>

#include "core_global.h"

VI_CORE_NS_BEGIN

template <typename... TArgs>
class Signal
{
public:
    using Handler = std::function<void(TArgs...)>;
    using HandlerId = std::list<Handler>::const_iterator;

public:
    Signal() = default;
    Signal(const Signal&) = delete;
    Signal& operator=(const Signal&) = delete;

public:
    HandlerId addHandler(const Handler &handler)
    {
        auto x = m_handlers.emplace_back(handler);
        return --m_handlers.cend();
    }

    void removeHandler(const HandlerId &id)
    {
        m_handlers.erase(id);
    }

    void fire(TArgs... args)
    {
        for (auto &handler : m_handlers)
        {
            handler(std::forward<TArgs>(args)...);
        }
    }

    void clear(){
        m_handlers.clear();
    }

private:
    std::list<Handler> m_handlers;
};

VI_CORE_NS_END