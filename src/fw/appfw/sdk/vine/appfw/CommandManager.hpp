#pragma once
#include "appfw_global.hpp"

#include <vine/RefObject.hpp>

V_APPFW_NS_BEGIN

class V_APPFW_API CommandManager : public RefObject {
    V_OBJECT_META_DECL;

  public:
    CommandManager();
};

using CommandManagerPtr = RefPtr<CommandManager>;

V_APPFW_NS_END
