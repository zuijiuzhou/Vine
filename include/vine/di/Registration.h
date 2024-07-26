#pragma once

#include <functional>

#include <vine/core/Class.h>
#include <vine/core/Object.h>
#include <vine/core/core_defs.h>

#include "Lifetime.h"
#include "di_global.h"

VI_DI_NS_BEGIN

class Container;

using InstanceFactory = std::function<Object*(Type, Container*)>;

class RegistrationPrivate;

class VI_DI_API Registration final : public Object {
    VI_OBJECT_META;
    VI_DISABLE_MOVE(Registration);

    friend class Container;

  private:
    Registration(Type type);
    Registration(const Registration& reg);

  public:
    Registration* instance(Object* inst);
    Object*       instance() const;

    Registration*   instanceFactory(InstanceFactory fac);
    InstanceFactory instanceFactory() const;

    Registration* impl(Type type);
    Type          impl();

    Registration* lifetime(Lifetime lt);
    Lifetime      lifetime() const;

    Type serviceType() const;


  public:
    Registration& operator=(const Registration& reg);

  public:
    template <typename T>
        requires Objectifiable<T>
    static Registration* create();

  private:
    RegistrationPrivate* const d;
};


template <typename T>
    requires Objectifiable<T>
Registration* Registration::create() {
    return new Registration(T::desc());
}


using RegistrationPtr = RefPtr<Registration>;

VI_DI_NS_END