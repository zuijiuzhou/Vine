#pragma once

#include <type_traits>
#include "core_global.h"

#include "Ptr.h"

VINE_CORE_NS_BEGIN

class Class;
class VINE_CORE_API Object
{
public:
    using Ptr = SharedPtr<Object>;

public:
    Object();
    virtual ~Object();

public:
    virtual const Class *isA() const;

    bool isKindOf(const Class *type) const;

    template <typename T>
        requires std::is_base_of<Object, T>::value
    bool isKindOf()
    {
        return isKindOf(T::desc());
    }

    void addRef();

    void removeRef();

    static const Class *desc();
    

private:
    struct Data;
    Data *d;
};

VINE_CORE_NS_END

// #define VI_OBJ(TParent) \
// static const Class* desc(); \
// virtual const Class* isA() const override;
