#pragma once

#include <vine/core/Object.h>

#include "appfw_global.h"

VI_APPFW_NS_BEGIN

class VI_APPFW_API AddinLoadContext : public Object {
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(AddinLoadContext);
};
using AddinLoadContextPtr = RefPtr<AddinLoadContext>;

VI_APPFW_NS_END