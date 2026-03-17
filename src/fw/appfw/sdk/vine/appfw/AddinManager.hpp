#pragma once
#include "appfw_global.hpp"

#include <vine/RefObject.hpp>

V_APPFW_NS_BEGIN

class Addin;

class V_APPFW_API AddinManager : public RefObject {
    V_OBJECT_META_DECL;

  public:
    AddinManager();

  public:
    Addin* load(const String& str);

  private:
    struct Data;
    Data* const d;
    ;
};

using AddinManagerPtr = RefPtr<AddinManager>;

V_APPFW_NS_END
