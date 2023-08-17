#pragma once

#include <type_traits>
#include "core_global.h"

#include "Ptr.h"

VINE_CORE_NS_BEGIN

class Class;
class Object;

template <typename T>
concept Objectifiable = std::is_base_of<Object, T>::value;

class VINE_CORE_API Object
{
public:
    Object();
    virtual ~Object();

public:
    virtual const Class *isA() const;

    bool isKindOf(const Class *type) const;

    template <Objectifiable T>
    bool isKindOf()
    {
        return isKindOf(T::desc());
    }

    template <Objectifiable T>
    T *as()
    {
        if (isKindOf(T::desc()))
            return static_cast<T *>(this);
        return nullptr;
    }

    template <Objectifiable T>
    const T *as() const
    {
        if (isKindOf(T::desc()))
            return static_cast<const T *>(this);
        return nullptr;
    }

    void addRef();

    void removeRef(bool del = true);

    static const Class *desc();

private:
    struct Data;
    Data *d;
};

using ObjectPtr = RefPtr<Object>;

VINE_CORE_NS_END

// #define VI_OBJ(TParent) \
// static const Class* desc(); \
// virtual const Class* isA() const override;
