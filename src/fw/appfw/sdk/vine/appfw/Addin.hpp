#pragma once
#include "appfw_global.hpp"

#include <vine/RefObject.hpp>


V_APPFW_NS_BEGIN

class AddinLoadContext;

class V_APPFW_API Addin : public Object {
    V_OBJECT_META_DECL;

  public:
    void load(AddinLoadContext* context);
    void unload();

  public:
    String getName() const;
};

V_APPFW_NS_END
