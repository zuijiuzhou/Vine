#pragma once

#include <vine/pltdef.h>
#include <vine/videf.h>

#ifdef VI_GRAPHICS_LIB
#define VI_GRAPHICS_API __API_EXPORT__
#else
#define VI_GRAPHICS_API __API_IMPORT__
#endif

#define VI_GRAPHICS_NS_BEGIN \
namespace VI_ROOT_NS { \
    namespace graphics { 

#define VI_GRAPHICS_NS_END \
    }\
}
