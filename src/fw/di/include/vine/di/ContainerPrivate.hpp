#pragma once
#include <vine/di/di_global.hpp>

#include <unordered_map>

#include <vine/di/Container.hpp>
#include <vine/di/Registration.hpp>

V_DI_NS_BEGIN

class ContainerPrivate : public RefObjectPrivate {
    V_DECLARE_PUBLIC(Container)

  public:
    ContainerPrivate();

  public:
    std::unordered_map<Type, Registration> regs;
};

V_DI_NS_END