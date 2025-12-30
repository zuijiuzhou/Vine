#pragma once

#include <vine/vi_global.hpp>

#ifdef VI_MATH_LIB
#define VI_MATH_API __API_EXPORT__
#else
#define VI_MATH_API __API_IMPORT__
#endif

#define VI_MATH_NS_BEGIN \
VI_ROOT_NS_BEGIN \
namespace math { 

#define VI_MATH_NS_END \
VI_ROOT_NS_END \
}
