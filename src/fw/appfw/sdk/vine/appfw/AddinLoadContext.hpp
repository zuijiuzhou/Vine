#pragma once

#include <vine/RefObject.hpp>

#include "appfw_global.hpp"

V_APPFW_NS_BEGIN

class V_APPFW_API AddinLoadContext : public RefObject {
    V_OBJECT_META_DECL;
};

using AddinLoadContextPtr = RefPtr<AddinLoadContext>;

V_APPFW_NS_END
