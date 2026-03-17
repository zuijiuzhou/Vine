#pragma once
#include "di_global.hpp"
#include <vine/RefObject.hpp>

V_DI_NS_BEGIN

class Registration;

V_DECLARE_PIMPL(Container)
V_DEFINE_PTR(Container)

class V_DI_API Container : public RefObject {
    V_OBJECT_META_DECL
    V_DECLARE_PRIVATE(Container)

  public:
    Container();

  public:
    void add(const Registration& reg);

    RefObject* resolve(Type type) const;

    template <RefObjectBased T>
    T* resolve() const;
};

template <RefObjectBased T>
T* Container::resolve() const
{
    return resolve(T::desc());
}

V_DI_NS_END
