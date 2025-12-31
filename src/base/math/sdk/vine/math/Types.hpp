#pragma once

#include "math_global.hpp"

#include <concepts>
#include <cstdint>
#include <type_traits>

VI_MATH_NS_BEGIN

/**
 * @brief floating point concept, includes float and double
 */
template <typename T>
concept FloatingPoint = std::is_floating_point<T>::value;

template <typename T>
concept FP = std::is_floating_point<T>::value;

/**
 * @brief Integral concept, includes all integral types (signed and unsigned and boolean)
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
 * @brief Arithmetic concept, includes all floating point and integral types (include boolean)
 */
template <typename T>
concept Arithmetic = FP<T> || Integral<T>;

/**
 * @brief evaluation type for arithmetic operations, following the c++ integral promotion rules
 */
template <Arithmetic T>
using TypeI = std::conditional_t<std::is_integral_v<T>, decltype(T() + T()), T>;

/**
 * @brief double for integral types, and T itself for floating point types
 */
template <Arithmetic T>
using TypeF = std::conditional_t<std::is_integral_v<T>, double, T>;

VI_MATH_NS_END