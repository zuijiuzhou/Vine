#pragma once

#include <vine/core_global.hpp>

#ifdef V_APPFW_LIB
#    define V_APPFW_API __API_EXPORT__
#else
#    define V_APPFW_API __API_IMPORT__
#endif

#define V_APPFW_NS_BEGIN                                                                                                                                       \
    namespace V_ROOT_NS                                                                                                                                        \
    {                                                                                                                                                          \
    namespace appfw                                                                                                                                            \
    {

#define V_APPFW_NS_END                                                                                                                                         \
    }                                                                                                                                                          \
    }

#define V_APPFWGUI_NS_BEGIN                                                                                                                                    \
    V_APPFW_NS_BEGIN                                                                                                                                           \
    namespace gui                                                                                                                                              \
    {

#define V_APPFWGUI_NS_END                                                                                                                                      \
    V_APPFW_NS_END                                                                                                                                             \
    }
