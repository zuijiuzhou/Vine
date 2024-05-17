#pragma once
#include "appfw_global.h"

#include "core/Object.h"

VI_APPFW_NS_BEGIN

class VI_APPFW_API AddinManager : public Object
{
    VI_OBJECT_META
};
using AddinManagerPtr = RefPtr<AddinManager>;

VI_APPFW_NS_END