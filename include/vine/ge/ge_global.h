#pragma once

#include <vine/pltdef.h>
#include <vine/videf.h>

#ifdef VI_GE_LIB
#define VI_GE_API __API_EXPORT__
#else
#define VI_GE_API __API_IMPORT__
#endif

#define VI_GE_NS_BEGIN \
VI_ROOT_NS_BEGIN \
namespace ge { 

#define VI_GE_NS_END \
VI_ROOT_NS_END \
}
