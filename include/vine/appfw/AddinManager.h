#pragma once
#include "appfw_global.h"

#include <vine/core/Object.h>

VI_APPFW_NS_BEGIN

class VI_APPFW_API AddinManager : public Object {
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(AddinManager);

  public:
    AddinManager();
};
using AddinManagerPtr = RefPtr<AddinManager>;

VI_APPFW_NS_END