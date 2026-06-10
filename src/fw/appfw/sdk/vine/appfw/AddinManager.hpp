#pragma once
#include "appfw_global.hpp"

#include <vine/RefObject.hpp>

V_APPFW_NS_BEGIN

class Addin;

class V_APPFW_API AddinManager {
  public:
    AddinManager();

  public:
    Addin* load(const String& str);

  private:
    struct Data;
    Data* const d;
};

V_APPFW_NS_END
