#pragma once

#ifdef VI_APPFW_LIB
#define VI_APPFW_API __declspec(dllexport)
#else
#define VI_APPFW_API __declspec(dllimport)
#endif

#define VI_APPFW_NS_BEGIN \
namespace VI_NS_NAME { \
    namespace appfw { 

#define VI_APPFW_NS_END \
    }\
}