#pragma once

#include "core_global.hpp"

#include "Class.hpp"
#include "Object.hpp"

V_CORE_NS_BEGIN

template <ObjectBased Parent, typename Sub>
class Inherit : public Parent {
  public:
    template <typename... TArgs>
    Inherit(TArgs&&... args)
      : Parent(std::forward<TArgs>(args)...)
    {}

  public:
    virtual const Class* getClass() const noexcept override
    {
        return desc();
    }

    static const Class* desc()
    {
        static const Class* cls = new Class(Parent::desc(), typeid(Sub));
        return cls;
    }

    // template <typename... TArgs>
    // static Sub* create(TArgs&&... args)
    // {
    //     return new Sub(std::forward<TArgs>(args)...);
    // }

    // template <typename... TArgs>
    // static Sub* create_if(bool flag, TArgs&&... args)
    // {
    //     return flag ? new Sub(std::forward<TArgs>(args)...) : nullptr;
    // }
};

V_CORE_NS_END
