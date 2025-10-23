#pragma once
#include "di_global.hpp"
#include <vine/core/RefObject.hpp>

VI_DI_NS_BEGIN

class Registration;
class VI_DI_API Container : public RefObject {
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(Container);

  public:
    Container();

  public:
    void add(Registration* reg);

    RefObject* resolve(Type type) const;

    template <typename T>
        requires Objectifiable<T>
    T* resolve() const;

  private:
    struct Data;
    Data* const d;
};
using ContainerPtr = RefPtr<Container>;

template <typename T>
    requires Objectifiable<T>
T* Container::resolve() const {
    return resolve(T::desc());
}

VI_DI_NS_END