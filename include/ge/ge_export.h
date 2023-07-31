#pragma once

#ifdef ETD_GE_LIB
#define ETD_GE_API __declspec(dllexport)
#else
#define ETD_GE_API __declspec(dllimport)
#endif

#define ETD_GE_NS_BEGIN \
namespace ETD_NS_NAME { \
    namespace ge { 

#define ETD_GE_NS_END \
    }\
}