#pragma once
#include "di_global.h"

#include "core/Inherit.h"

VINE_DI_NS_BEGIN

class VINE_DI_API Container : Inherit<Object, Container> {
    public:

};
using ContainerPtr = RefPtr<Container>;

VINE_DI_NS_END