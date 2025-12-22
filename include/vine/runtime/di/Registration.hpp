#pragma once

#include <functional>

#include <vine/core/Class.hpp>
#include <vine/core/RefObject.hpp>

#include "Lifetime.hpp"
#include "di_global.hpp"

VI_DI_NS_BEGIN

class Container;

using InstanceFactory = std::function<RefObject*(Type, Container*)>;

class RegistrationPrivate;

class VI_DI_API Registration final : public RefObject {
    VI_OBJECT_META;
    VI_DISABLE_MOVE(Registration);

    friend class Container;

  private:
    Registration(Type type);
    Registration(const Registration& reg);

  public:
    Registration* instance(RefObject* inst);
    RefObject*    instance() const;

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
Registration* Registration::create()
{
    return new Registration(T::desc());
}

using RegistrationPtr = RefPtr<Registration>;

VI_DI_NS_END