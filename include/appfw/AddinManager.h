#pragma once
#include "appfw_global.h"

#include "core/Inherit.h"


VINE_APPFW_NS_BEGIN

class VINE_APPFW_API AddinManager: public Inherit<Object, AddinManager>{

    
};
using AddinManagerPtr = RefPtr<AddinManager>;

VINE_APPFW_NS_END