#pragma once

#ifdef VI_GE_LIB
#define VI_GE_API __declspec(dllexport)
#else
#define VI_GE_API __declspec(dllimport)
#endif

#define VI_GE_NS_BEGIN \
namespace VI_NS_NAME { \
    namespace ge { 

#define VI_GE_NS_END \
    }\
}
