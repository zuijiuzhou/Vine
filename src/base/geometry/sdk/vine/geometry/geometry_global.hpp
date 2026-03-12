#pragma once

#include <vine/vi_global.hpp>

#ifdef VI_GEOMETRY_LIB
#define VI_GEOMETRY_API __API_EXPORT__
#else
#define VI_GEOMETRY_API __API_IMPORT__
#endif

#define VI_GEOMETRY_NS_BEGIN \
VI_ROOT_NS_BEGIN \
namespace geometry { 

#define VI_GEOMETRY_NS_END \
VI_ROOT_NS_END \
}

