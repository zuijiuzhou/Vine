#pragma once
#include "appfw_global.h"

#include "core/Inherit.h"

VINE_APPFW_NS_BEGIN

class VINE_APPFW_API ServiceManager : public Object
{
    VI_OBJECT_META
public:
public:
    // void registerService(Type service_type, );
};
using ServiceManagerPtr = RefPtr<ServiceManager>;

VINE_APPFW_NS_END