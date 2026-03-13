#pragma once
#include "di_global.hpp"
#include <vine/RefObject.hpp>

VI_DI_NS_BEGIN

class Registration;

VI_DECLARE_PIMPL(Container)
VI_DEFINE_PTR(Container)

class VI_DI_API Container : public RefObject {
    VI_OBJECT_META_DECL
    VI_DECLARE_PRIVATE(Container)

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

VI_DI_NS_END
