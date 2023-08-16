#pragma once
#include "di_global.h"

#include <functional>

#include "core/Inherit.h"

#include "Lifetime.h"

VINE_DI_NS_BEGIN

#define __CONF_FUNC_TEMP__ template <typename T> \
    requires is_base_of_object<T>

class Container;

using InstanceFactory = std::function<Object*(Type, Container*)>;

class VINE_DI_API Registration : public Inherit<Object, Registration>
{
private:
    Registration(Type type);

public:
    Registration *instance(Object *inst);
    __CONF_FUNC_TEMP__ Registration *instance(T *inst);

    Registration *impl(Type type);
    __CONF_FUNC_TEMP__ Registration *impl();

    Registration *lifetime(Lifetime lt);

    Registration *useFactory(InstanceFactory fac);

    Lifetime lifetime() const;

public:
    __CONF_FUNC_TEMP__ static Registration *create();

private:
    struct Data;
    Data *const d;
};

__CONF_FUNC_TEMP__ Registration *Registration::create()
{
    return new Registration(T::desc());
}

__CONF_FUNC_TEMP__ Registration *Registration::instance(T *obj)
{
    return instance((Object *)obj);
}

__CONF_FUNC_TEMP__ Registration *Registration::impl()
{
    return impl(T::desc());
}
#undef __CONF_FUNC_TEMP__

using RegistrationPtr = RefPtr<Registration>;
VINE_DI_NS_END