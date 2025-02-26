#pragma once

#include <vine/ge/ge_global.h>

#include <cassert>
#include <type_traits>

VI_GE_NS_BEGIN

#define _VI_ASSERT_DOES_NOT_SUPPORT_BOOL                                                                               \
    if constexpr (std::is_same<T, bool>::value) {                                                                      \
        assert(false, "The current method does not support the bool type.");                                           \
        throw std::exception("The current method does not support the bool type.");                                    \
    }

VI_GE_NS_END