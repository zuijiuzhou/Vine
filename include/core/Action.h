#pragma once
#include <list>
#include <memory>

#include "core_export.h"

ETD_CORE_NS_BEGIN
template <typename... TArgs>
class ETD_CORE_API Signal
{
public:
    using Handler = std::function<void(TArgs...)>;
    using HandlerId = std::list<Handler>::const_iterator;

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

private:
    std::list<Handler> m_handlers;
};
ETD_CORE_NS_END