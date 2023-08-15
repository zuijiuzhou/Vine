#pragma once
#include "appfw_global.h"

#include "core/Inherit.h"


VINE_APPFW_NS_BEGIN

class AddinLoadContext;
class VINE_APPFW_API Addin : public Inherit<Object, Addin>
{
public:
    void Load(AddinLoadContext* context);
    void Unload();
};
using AddinPtr = RefPtr<Addin>;

VINE_APPFW_NS_END