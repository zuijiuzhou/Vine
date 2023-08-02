#pragma once

#ifdef VINE_GE_LIB
#define VINE_GE_API __declspec(dllexport)
#else
#define VINE_GE_API __declspec(dllimport)
#endif

#define VINE_GE_NS_BEGIN \
namespace VINE_NS_NAME { \
    namespace ge { 

#define VINE_GE_NS_END \
    }\
}