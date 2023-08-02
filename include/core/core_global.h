#pragma once

#ifdef VINE_CORE_LIB
#define VINE_CORE_API __declspec(dllexport)
#else
#define VINE_CORE_API __declspec(dllimport)
#endif

#define VINE_CORE_NS_BEGIN namespace VINE_NS_NAME {
    
#define VINE_CORE_NS_END }