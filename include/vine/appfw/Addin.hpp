#pragma once
#include "appfw_global.hpp"

#include <vine/core/RefObject.hpp>


VI_APPFW_NS_BEGIN

class AddinLoadContext;
class VI_APPFW_API Addin : public RefObject {
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(Addin);

  public:
    void load(AddinLoadContext* context);
    void unload();

  public:
    String getName() const;

};
using AddinPtr = RefPtr<Addin>;

VI_APPFW_NS_END