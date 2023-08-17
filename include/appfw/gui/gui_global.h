#pragma once

#ifdef VINE_APPFWGUI_LIB
#define VINE_APPFWGUI_API __declspec(dllexport)
#else
#define VINE_APPFWGUI_API __declspec(dllimport)
#endif

#define VINE_APPFWGUI_NS_BEGIN \
namespace VINE_NS_NAME { \
    namespace appfw { \
        namespace gui {
#define VINE_APPFWGUI_NS_END \
        }\
    }\
}