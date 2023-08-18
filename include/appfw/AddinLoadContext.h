#pragma once

#include "core/Object.h"

#include "appfw_global.h"

VINE_APPFW_NS_BEGIN

class VINE_APPFW_API AddinLoadContext : public Object
{
    VI_OBJECT_META


};
using AddinLoadContextPtr = RefPtr<AddinLoadContext>;

VINE_APPFW_NS_END