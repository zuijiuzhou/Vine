#pragma once

#ifdef VI_GRAPHICS_LIB
#define VI_GRAPHICS_API __declspec(dllexport)
#else
#define VI_GRAPHICS_API __declspec(dllimport)
#endif

#define VI_GRAPHICS_NS_BEGIN \
namespace VI_NS_NAME { \
    namespace graphics { 

#define VI_GRAPHICS_NS_END \
    }\
}
