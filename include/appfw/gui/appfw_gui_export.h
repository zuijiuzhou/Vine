#pragma once

#ifdef ETD_APPFW_GUI_LIB
#define ETD_APPFW_GUI_API __declspec(dllexport)
#else
#define ETD_APPFW_GUI_API __declspec(dllimport)
#endif

#define ETD_APPFW_GUI_BEGIN \
namespace ETD_NS_NAME { \
    namespace appfw { \
        namespace gui {
#define ETD_APPFW_GUI_NS_END \
        }\
    }\
}