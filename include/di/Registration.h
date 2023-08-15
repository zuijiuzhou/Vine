#pragma once
#include "di_global.h"

#include <functional>

#include "core/Inherit.h"

VINE_DI_NS_BEGIN

#define __CONF_FUNC_TEMP__ template <typename T> \
    requires std::is_base_of<Object, T>::value

class VINE_DI_API Registration : Inherit<Object, Registration>
{
private:
    Registration(Type type);

public:
    __CONF_FUNC_TEMP__ Registration *instance(const T *inst);
    Registration *instance(const Object *inst);

    __CONF_FUNC_TEMP__ Registration *implementedBy();
    Registration *implementedBy(Type type);

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

__CONF_FUNC_TEMP__ Registration *Registration::instance(const T *obj)
{
    return instance((Object *)obj);
}

__CONF_FUNC_TEMP__ Registration *Registration::implementedBy()
{
    return implementedBy(type);
}
#undef __CONF_FUNC_TEMP__

using RegistrationPtr = RefPtr<Registration>;
VINE_DI_NS_END