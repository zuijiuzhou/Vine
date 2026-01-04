#pragma once
#include "appfw_global.hpp"

#include <vine/RefObject.hpp>

VI_APPFW_NS_BEGIN

class Addin;
class VI_APPFW_API AddinManager : public RefObject {
    VI_OBJECT_META;

  public:
    AddinManager();

  public:
    Addin* load(const String& str);

  private:
    VI_OBJECT_DATA;
};
using AddinManagerPtr = RefPtr<AddinManager>;

VI_APPFW_NS_END