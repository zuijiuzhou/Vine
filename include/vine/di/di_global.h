#pragma once

#ifdef VI_DI_LIB
#define VI_DI_API __declspec(dllexport)
#else
#define VI_DI_API __declspec(dllimport)
#endif

#define VI_DI_NS_BEGIN \
namespace VI_NS_NAME { \
    namespace di { 

#define VI_DI_NS_END \
    }\
}
