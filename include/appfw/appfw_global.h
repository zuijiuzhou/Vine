#pragma once

#ifdef VINE_APPFW_LIB
#define VINE_APPFW_API __declspec(dllexport)
#else
#define VINE_APPFW_API __declspec(dllimport)
#endif

#define VINE_APPFW_NS_BEGIN \
namespace VINE_NS_NAME { \
    namespace appfw { 

#define VINE_APPFW_NS_END \
    }\
}