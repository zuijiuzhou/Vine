#pragma once

#include <vine/pltdef.h>
#include <vine/videf.h>

#ifdef VI_RUNTIME_LIB
#define VI_RUNTIME_API __API_EXPORT__
#else
#define VI_RUNTIME_API __API_IMPORT__
#endif

#define VI_RUNTIME_NS_BEGIN \
namespace VI_ROOT_NS { \
    namespace runtime { 

#define VI_RUNTIME_NS_END \
    }\
}

