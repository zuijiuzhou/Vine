#pragma once

#include "core/Inherit.h"

#include "appfw_global.h"

VINE_APPFW_NS_BEGIN

class AddinLoadContext;
class VINE_APPFW_API Addin : public Inherit<Object, Addin>
{
public:
    void Load(AddinLoadContext* context);
    void Unload();
};

VINE_APPFW_NS_END