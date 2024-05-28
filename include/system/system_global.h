#pragma once

#ifdef VI_SYSTEM_LIB
#define VI_SYSTEM_API __declspec(dllexport)
#else
#define VI_SYSTEM_API __declspec(dllimport)
#endif

#define VI_SYSTEM_NS_BEGIN \
namespace VI_NS_NAME { \
    namespace system { 

#define VI_SYSTEM_NS_END \
    }\
}