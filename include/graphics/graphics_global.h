#pragma once

#ifdef VINE_GRAPHICS_LIB
#define VINE_GRAPHICS_API __declspec(dllexport)
#else
#define VINE_GRAPHICS_API __declspec(dllimport)
#endif

#define VINE_GRAPHICS_NS_BEGIN \
namespace VINE_NS_NAME { \
    namespace graphics { 

#define VINE_GRAPHICS_NS_END \
    }\
}