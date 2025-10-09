#pragma once

#include <vine/pltdef.hpp>
#include <vine/videf.hpp>

#ifdef VI_APPFWGUI_LIB
#    define VI_APPFWGUI_API __API_EXPORT__
#else
#    define VI_APPFWGUI_API __API_IMPORT__
#endif

#define VI_APPFWGUI_NS_BEGIN                                                                                           \
    namespace VI_ROOT_NS {                                                                                             \
    namespace appfw {                                                                                                  \
    namespace gui {
#define VI_APPFWGUI_NS_END                                                                                             \
    }                                                                                                                  \
    }                                                                                                                  \
    }
