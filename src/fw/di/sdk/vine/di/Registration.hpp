#pragma once

#include <functional>

#include <vine/Class.hpp>
#include <vine/RefObject.hpp>

#include "Lifetime.hpp"
#include "di_global.hpp"

V_DI_NS_BEGIN

class Container;
using InstanceFactory = std::function<RefObject*(Type, Container&)>;

class V_DI_API Registration final {

  private:
    Registration() = default;
    Registration(Type type);

  public:
    Registration& instance(RefObject* inst);

    RefObject* instance() const
    {
        return inst_.get();
    }

    Registration& instanceFactory(InstanceFactory fac)
    {
        inst_fac_ = std::move(fac);
        return *this;
    }

    InstanceFactory instanceFactory() const
    {
        return inst_fac_;
    }

    Registration& impl(Type type)
    {
        service_impl_type_ = type;
        return *this;
    }

    Type impl()
    {
        return service_impl_type_;
    }

    Registration& lifetime(Lifetime lt)
    {
        lifetime_ = lt;
        return *this;
    }

    Lifetime lifetime() const
    {
        return lifetime_;
    }

    Type serviceType() const
    {
        return service_type_;
    }

  public:
    Registration& operator=(const Registration& reg);

  public:
    template <ObjectBased T>
    static Registration create();

  private:
    Type service_type_{};
    Type service_impl_type_{};
    V_PTR(RefObject) inst_;
    InstanceFactory inst_fac_;
    Lifetime        lifetime_ = Lifetime::Transient;
};

template <ObjectBased T>
Registration Registration::create()
{
    return Registration(T::desc());
}

V_DI_NS_END
