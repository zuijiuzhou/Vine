#pragma once

#ifdef VI_APPFWGUI_LIB
#define VI_APPFWGUI_API __declspec(dllexport)
#else
#define VI_APPFWGUI_API __declspec(dllimport)
#endif

#define VI_APPFWGUI_NS_BEGIN \
namespace VI_NS_NAME { \
    namespace appfw { \
        namespace gui {
#define VI_APPFWGUI_NS_END \
        }\
    }\
}

