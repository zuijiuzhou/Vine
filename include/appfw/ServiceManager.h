#pragma once
#include "core/Inherit.h"

#include "appfw_global.h"

VINE_APPFW_NS_BEGIN

class VINE_APPFW_API ServiceManager: public Inherit<Object, ServiceManager>{

public:
    

public:
    // void registerService(Type service_type, );

};
using ServiceManagerPtr = RefPtr<ServiceManager>;

VINE_APPFW_NS_END