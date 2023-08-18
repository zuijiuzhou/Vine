#pragma once
#include "di_global.h"
#include "core/Object.h"

VINE_DI_NS_BEGIN

class Registration;
class VINE_DI_API Container : public Object
{
    VI_OBJECT_META
    
public:
    Container();

public:
    void add(Registration *reg);

    Object *resolve(Type type) const;

    template <typename T>
        requires Objectifiable<T>
    T *resolve() const;

private:
    struct Data;
    Data *const d;
};
using ContainerPtr = RefPtr<Container>;

template <typename T>
    requires Objectifiable<T>
T *Container::resolve() const
{
    return resolve(T::desc());
}

VINE_DI_NS_END