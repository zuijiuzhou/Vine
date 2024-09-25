#pragma once

#include <vine/pltdef.h>
#include <vine/videf.h>

#ifdef VI_CORE_LIB
#    define VI_CORE_API __API_EXPORT__
#else
#    define VI_CORE_API __API_IMPORT__
#endif

#define VI_CORE_NS_BEGIN VI_ROOT_NS_BEGIN

#define VI_CORE_NS_END VI_ROOT_NS_END