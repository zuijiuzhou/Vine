#pragma once
#include "appfw_global.h"

#include "core/Object.h"

VINE_APPFW_NS_BEGIN

class VINE_APPFW_API CommandManager : public Object
{
    VI_OBJECT_META
};
using CommandManagerPtr = RefPtr<CommandManager>;

VINE_APPFW_NS_END