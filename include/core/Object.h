#pragma once

#include <type_traits>

#include "core_export.h"
#include "Str.h"
#include "Class.h"
#include "Ptr.h"

ETD_CORE_NS_BEGIN
class Class;
class ETD_CORE_API Object
{
public:
    using Ptr = SharedPtr<Object>;

public:
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

    static const Class *desc();

private:
    struct Data;
    Data *d_ptr;
};

ETD_CORE_NS_END