#pragma once
#include "appfw_global.h"

#include <vine/core/Object.h>

VI_APPFW_NS_BEGIN

class VI_APPFW_API CommandManager : public Object {
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(CommandManager);

  public:
    CommandManager();
};
using CommandManagerPtr = RefPtr<CommandManager>;

VI_APPFW_NS_END