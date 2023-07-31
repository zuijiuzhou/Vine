#pragma once

#include "core/Inherit.h"

#include "appfw_export.h"

ETD_APPFW_NS_BEGIN

class AddinLoadContext;
class ETD_APPFW_API Addin : public Inherit<Object, Addin>
{
public:
    void Load(AddinLoadContext* context);
    void Unload();
};

ETD_APPFW_NS_END