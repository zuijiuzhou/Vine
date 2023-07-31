#pragma once

#ifdef ETD_CORE_LIB
#define ETD_CORE_API __declspec(dllexport)
#else
#define ETD_CORE_API __declspec(dllimport)
#endif

#define ETD_CORE_NS_BEGIN namespace ETD_NS_NAME {
    
#define ETD_CORE_NS_END }