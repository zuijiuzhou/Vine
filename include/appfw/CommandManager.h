#pragma once
#include "core/Inherit.h"

#include "appfw_global.h"

VINE_APPFW_NS_BEGIN

class VINE_APPFW_API CommandManager : public Inherit<Object, CommandManager>{

    
};
using CommandManagerPtr = RefPtr<CommandManager>;

VINE_APPFW_NS_END