#pragma once
#include "di_global.h"

#include <functional>

#include "core/Inherit.h"

VINE_DI_NS_BEGIN

#define __CONF_FUNC__ template <typename T> \
    requires std::is_base_of<Object, T>::value

class VINE_DI_API Registration : Inherit<Object, Registration>
{
public:
    Registration();

public:
    __CONF_FUNC__ Registration *instance(const T* obj);
    Registration *instance(const Object* obj);

public:
    __CONF_FUNC__ static Registration *create();

private:
    struct Data;
    Data *const d;
};

__CONF_FUNC__ Registration *Registration::create()
{
    return new Registration(T::desc());
}

__CONF_FUNC__ Registration *instance(const T* obj)
{
    return this->instance((Object*)obj);
}

using RegistrationPtr = RefPtr<Registration>;
VINE_DI_NS_END