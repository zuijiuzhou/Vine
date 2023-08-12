#pragma once

#ifdef VINE_DI_LIB
#define VINE_DI_API __declspec(dllexport)
#else
#define VINE_DI_API __declspec(dllimport)
#endif

#define VINE_DI_NS_BEGIN \
namespace VINE_NS_NAME { \
    namespace di { 

#define VINE_DI_NS_END \
    }\
}