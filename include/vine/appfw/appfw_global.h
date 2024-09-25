#pragma once

#include <vine/pltdef.h>
#include <vine/videf.h>

#ifdef VI_APPFW_LIB
#    define VI_APPFW_API __API_EXPORT__
#else
#    define VI_APPFW_API __API_IMPORT__
#endif

#define VI_APPFW_NS_BEGIN                                                                                              \
    namespace VI_ROOT_NS {                                                                                             \
    namespace appfw {

#define VI_APPFW_NS_END                                                                                                \
    }                                                                                                                  \
    }

