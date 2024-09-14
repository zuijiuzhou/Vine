#pragma once

#ifdef VI_RUNTIME_LIB
#define VI_RUNTIME_API __declspec(dllexport)
#else
#define VI_RUNTIME_API __declspec(dllimport)
#endif

#define VI_RUNTIME_NS_BEGIN \
namespace VI_NS_NAME { \
    namespace runtime { 

#define VI_RUNTIME_NS_END \
    }\
}

