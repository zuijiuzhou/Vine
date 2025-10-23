#pragma once

#include <functional>

#include <vine/core/Class.hpp>
#include <vine/core/RefObject.hpp>
#include <vine/core/core_defs.hpp>

#include <vine/di/Lifetime.hpp>
#include <vine/di/Registration.hpp>
#include <vine/di/di_global.hpp>

VI_DI_NS_BEGIN

class Container;

class RegistrationPrivate {
    friend class Registration;
    friend class Container;

  private:
    Registration*   reg;
    Type            service_type      = nullptr;
    Type            service_impl_type = nullptr;
    RefObjectPtr       inst;
    InstanceFactory inst_fac;
    Lifetime        lifetime = Lifetime::Transient;
};
VI_DI_NS_END