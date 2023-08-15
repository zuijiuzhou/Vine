#pragma once
#include "Object.h"
#include "Class.h"
#include "String.h"

VINE_CORE_NS_BEGIN

template <typename TParent, typename TSub>
    requires std::is_base_of<Object, TParent>::value
class Inherit : public TParent
{
public:
    template <typename... TArgs>
    Inherit(TArgs &&...args) : TParent(args...) {}

    virtual ~Inherit(){}
public:
    virtual const Class *isA() const override
    {
        return desc();
    }

    static const Class *desc()
    {
        static const Class *cls = new Class(TParent::desc(), typeid(TSub));
        return cls;
    }

    template <typename... TArgs>
    static TSub* create(TArgs &&...args)
    {
        return new TSub(args...);
    }

    template <typename... TArgs>
    static TSub*create_if(bool flag, TArgs &&...args)
    {
        return flag ? new TSub(args...) : nullptr;
    }
};

VINE_CORE_NS_END