#pragma once
#include "appfw_global.h"

#include "core/Object.h"

VINE_APPFW_NS_BEGIN

class VINE_APPFW_API AddinManager : public Object
{
    VI_OBJECT_META
};
using AddinManagerPtr = RefPtr<AddinManager>;

VINE_APPFW_NS_END