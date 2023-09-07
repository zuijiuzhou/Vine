#pragma once
#include "di_global.h"
#include <functional>
#include "core/Object.h"
#include "Lifetime.h"

VI_DI_NS_BEGIN

#define __CONF_FUNC_TEMP__ template <typename T> \
    requires Objectifiable<T>

class Container;

using InstanceFactory = std::function<Object*(Type, Container*)>;

class VI_DI_API Registration : public Object
{
    VI_OBJECT_META
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
    VI_OBJECT_DATA
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
VI_DI_NS_END