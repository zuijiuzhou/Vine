#pragma once

#include <vine/vi_global.hpp>

#ifdef VI_SYSTEM_LIB
#define VI_SYSTEM_API __API_EXPORT__
#else
#define VI_SYSTEM_API __API_IMPORT__
#endif

#define VI_SYSTEM_NS_BEGIN \
namespace VI_ROOT_NS { \
    namespace system { 

#define VI_SYSTEM_NS_END \
    }\
}
