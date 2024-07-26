#pragma once
#include "appfw_global.h"

#include <vine/core/Object.h>


VI_APPFW_NS_BEGIN

class AddinLoadContext;
class VI_APPFW_API Addin : public Object
{
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(Addin);

public:
    void Load(AddinLoadContext* context);
    void Unload();
};
using AddinPtr = RefPtr<Addin>;

VI_APPFW_NS_END