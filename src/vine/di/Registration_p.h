#pragma once

#include <functional>

#include <vine/core/Class.h>
#include <vine/core/Object.h>
#include <vine/core/core_defs.h>

#include <vine/di/Lifetime.h>
#include <vine/di/Registration.h>
#include <vine/di/di_global.h>

VI_DI_NS_BEGIN

class Container;

class RegistrationPrivate {
    friend class Registration;
    friend class Container;

  private:
    Registration*   reg;
    Type            service_type      = nullptr;
    Type            service_impl_type = nullptr;
    ObjectPtr       inst;
    InstanceFactory inst_fac;
    Lifetime        lifetime = Lifetime::Transient;
};
VI_DI_NS_END