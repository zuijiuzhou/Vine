#pragma once

#include "ge_global.hpp"

#include <concepts>
#include <cstdint>
#include <type_traits>

VI_GE_NS_BEGIN

template <typename T>
concept FP = std::is_floating_point<T>::value;

template <typename T>
concept INT8T64 =
    std::is_same<T, int8_t>::value || std::is_same<T, uint8_t>::value || std::is_same<T, int16_t>::value ||
    std::is_same<T, uint16_t>::value || std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value ||
    std::is_same<T, int64_t>::value || std::is_same<T, uint64_t>::value;

template <typename T>
concept INT = std::integral<T>;

template <typename T>
concept FP_INT = FP<T> || INT<T>;

template <typename T>
concept FP_INT8T64 = FP<T> && INT8T64<T>;


VI_GE_NS_END