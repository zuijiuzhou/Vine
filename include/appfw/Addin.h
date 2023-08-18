#pragma once
#include "appfw_global.h"

#include "core/Object.h"


VINE_APPFW_NS_BEGIN

class AddinLoadContext;
class VINE_APPFW_API Addin : public Object
{
    VI_OBJECT_META
public:
    void Load(AddinLoadContext* context);
    void Unload();
};
using AddinPtr = RefPtr<Addin>;

VINE_APPFW_NS_END