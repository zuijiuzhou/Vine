#pragma once

#include <vine/core_global.hpp>

#ifdef VI_APPFW_LIB
#    define VI_APPFW_API __API_EXPORT__
#else
#    define VI_APPFW_API __API_IMPORT__
#endif

#define VI_APPFW_NS_BEGIN                                                                                              \
    namespace VI_ROOT_NS                                                                                               \
    {                                                                                                                  \
    namespace appfw                                                                                                    \
    {

#define VI_APPFW_NS_END                                                                                                \
    }                                                                                                                  \
    }

#define VI_APPFWGUI_NS_BEGIN                                                                                           \
    VI_APPFW_NS_BEGIN                                                                                                  \
    namespace gui                                                                                                      \
    {

#define VI_APPFWGUI_NS_END                                                                                             \
    VI_APPFW_NS_END                                                                                                    \
    }
