#pragma once

#include <vine/ge/ge_global.hpp>

#include <cassert>
#include <type_traits>
#include <stdexcept>

VI_GE_NS_BEGIN

#define _VI_ASSERT_DOES_NOT_SUPPORT_BOOL                                                                               \
    if constexpr (std::is_same<T, bool>::value) {                                                                      \
        assert(false && "The current method does not support the bool type.");                                           \
        throw std::runtime_error("The current method does not support the bool type.");                                    \
    }

VI_GE_NS_END