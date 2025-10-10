#pragma once

#include "ge_global.hpp"

#include <concepts>
#include <cstdint>
#include <type_traits>

VI_GE_NS_BEGIN

/**
 * @brief floating point concept, includes float and double
 */
template <typename T>
concept FloatingPoint = std::is_floating_point<T>::value;

template <typename T>
concept FP = std::is_floating_point<T>::value;

/**
 * @brief Integral concept, includes all integer types (signed and unsigned and boolean)
 */
template <typename T>
concept Integral = std::integral<T>;

/**
 * @brief Interger concept, includes all integer types (signed and unsigned) but not boolean
 */
template <typename T>
concept Integer =
    std::is_same<T, int8_t>::value || std::is_same<T, uint8_t>::value || std::is_same<T, int16_t>::value ||
    std::is_same<T, uint16_t>::value || std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value ||
    std::is_same<T, int64_t>::value || std::is_same<T, uint64_t>::value;

/**
 * @brief Real concept, includes all floating point and integer types (but not boolean)
 */
template <typename T>
concept Real = FP<T> || Integer<T>;

/**
 * @brief double for integer types, and T itself for floating point types
 */
template <typename T> using LengthType = std::conditional_t<std::is_integral_v<T>, double, T>;

VI_GE_NS_END