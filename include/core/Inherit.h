#pragma once

#include "Object.h"

ETD_CORE_NS_BEGIN

template <typename TParent, typename TSub>
    requires std::is_base_of<Object, TParent>::value
class Inherit : public TParent
{

public:
    template <typename... TArgs>
    Inherit(TArgs &&...args) : TParent(args...) {}

public:
    virtual const Class *isA() const override
    {
        return desc();
    }

    static const Class *desc()
    {
        static const Class *cls = new Class(TParent::desc());
        return cls;
    }

    template <typename... TArgs>
    static Ptr<TSub> create(TArgs &&...args)
    {
        return Ptr<TSub>(new TSub(args...));
    }

    template <typename... TArgs>
    static Ptr<TSub> create_if(bool flag, TArgs &&...args)
    {
        return Ptr<TSub>(flag ? new TSub(args...) : nullptr);
    }
};

ETD_CORE_NS_END