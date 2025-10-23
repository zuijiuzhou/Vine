#pragma once

#include <vine/core/RefObject.hpp>

#include "appfw_global.hpp"

VI_APPFW_NS_BEGIN

class VI_APPFW_API AddinLoadContext : public RefObject {
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(AddinLoadContext);
};
using AddinLoadContextPtr = RefPtr<AddinLoadContext>;

VI_APPFW_NS_END