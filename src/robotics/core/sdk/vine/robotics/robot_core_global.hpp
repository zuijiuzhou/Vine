#pragma once

#include <vine/core_global.hpp>

#ifdef V_ROBOTICSCORE_LIB
#    define V_ROBOTICS_CORE_API V_EXPORT
#else
#    define V_ROBOTICS_CORE_API V_IMPORT
#endif

#define V_ROBOTICS_NS_BEGIN                                                                                                                                       \
    namespace V_ROOT_NS                                                                                                                                        \
    {                                                                                                                                                          \
    namespace robotics                                                                                                                                            \
    {

#define V_ROBOTICS_NS_END                                                                                                                                         \
    }                                                                                                                                                          \
    }

#define V_ROBOTICS_KINEMATICS_NS_BEGIN                                                                                                                                    \
    V_ROBOTICS_NS_BEGIN                                                                                                                                           \
    namespace kinematics                                                                                                                                              \
    {

#define V_ROBOTICS_KINEMATICS_NS_END                                                                                                                                      \
    V_ROBOTICS_NS_END                                                                                                                                             \
    }

#define V_ROBOTICS_WORKCELL_NS_BEGIN                                                                                                                                     \
    V_ROBOTICS_NS_BEGIN                                                                                                                                             \
    namespace workcell                                                                                                                                                  \
    {

#define V_ROBOTICS_WORKCELL_NS_END                                                                                                                                       \
    }                                                                                                                                                              \
    V_ROBOTICS_NS_END

#define V_ROBOTICS_PROXIMITY_NS_BEGIN                                                                                                                                     \
    V_ROBOTICS_NS_BEGIN                                                                                                                                             \
    namespace proximity                                                                                                                                                  \
    {

#define V_ROBOTICS_PROXIMITY_NS_END                                                                                                                                       \
    }                                                                                                                                                              \
    V_ROBOTICS_NS_END
