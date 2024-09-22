#pragma once

#ifdef VI_CORE_LIB
#define VI_CORE_API __declspec(dllexport)
#else
#define VI_CORE_API __declspec(dllimport)
#endif

#define VI_CORE_NS_BEGIN namespace VI_NS_NAME {
    
#define VI_CORE_NS_END }

#ifdef __GNUC__
    #define __GCC__
#elif defined(__clang__)
    #define __CLANG__
#elif defined(_MSC_VER)
    #define __MSVC__
#elif defined(__INTEL_COMPILER)
    #define __INTELC__
#else
    #error Unknown compiler.
#endif