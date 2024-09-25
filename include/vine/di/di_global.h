#pragma once

#include <vine/pltdef.h>
#include <vine/videf.h>

#ifdef VI_DI_LIB
#define VI_DI_API __API_EXPORT__
#else
#define VI_DI_API __API_IMPORT__
#endif

#define VI_DI_NS_BEGIN \
namespace VI_ROOT_NS { \
    namespace di { 

#define VI_DI_NS_END \
    }\
}
