#pragma once

#ifdef VI_CORE_LIB
#define VI_CORE_API __declspec(dllexport)
#else
#define VI_CORE_API __declspec(dllimport)
#endif

#define VI_CORE_NS_BEGIN namespace VI_NS_NAME {
    
#define VI_CORE_NS_END }