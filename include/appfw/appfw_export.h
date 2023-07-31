#pragma once

#ifdef ETD_APPFW_LIB
#define ETD_APPFW_API __declspec(dllexport)
#else
#define ETD_APPFW_API __declspec(dllimport)
#endif

#define ETD_APPFW_NS_BEGIN \
namespace ETD_NS_NAME { \
    namespace appfw { 

#define ETD_APPFW_NS_END \
    }\
}