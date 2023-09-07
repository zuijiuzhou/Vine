#pragma once
#include "appfw_global.h"

#include "core/Inherit.h"

VI_APPFW_NS_BEGIN

class VI_APPFW_API ServiceManager : public Object
{
    VI_OBJECT_META
public:
public:
    // void registerService(Type service_type, );
};
using ServiceManagerPtr = RefPtr<ServiceManager>;

VI_APPFW_NS_END