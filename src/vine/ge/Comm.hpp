#pragma once

#include <vine/ge/ge_global.hpp>

#include <cmath>

#include <vine/ge/Types.hpp>

VI_GE_NS_BEGIN

/**
 * @brief safe calculation of vector length squared, for floating point types, it is just normal calculation,
 *        for integer types, it is promoted to double first, then calculated
 */
template <Real T, Real... Rest>
inline TypeF<T> calc_vec_len2_safe(T first, Rest... rest)
{
    // static_assert((std::is_same_v<T, Rest> && ...), "All parameters must have type T");

    if constexpr (FP<T>) {
        return first * first + (... + (rest * rest));
    }
    else {
        using LT = TypeF<T>;

        auto sum = static_cast<LT>(first) * static_cast<LT>(first);

        ((sum += static_cast<LT>(rest) * static_cast<LT>(rest)), ...);

        return sum;
    }
}

/**
 * @brief safe calculation of vector length, for floating point types, it is just normal calculation,
 *        for integer types, it is promoted to double first, then calculated
 */
template <Real T, Real... Rest>
inline TypeF<T> calc_vec_len_safe(T first, Rest... rest)
{
    return std::sqrt(calc_vec_len2_safe(first, rest...));
}

/**
 * @brief safe multiplication, for floating point types, it is just normal multiplication,
 *        for integer types, it is promoted to double first, then multiplied
 */
template <Real T>
inline TypeF<T> multiply_safe(T first, T second)
{
    if constexpr (FP<T>) {
        return first * second;
    }
    else {
        using LT = TypeF<T>;

        return static_cast<LT>(first) * static_cast<LT>(first);
    }
}

/**
 * for boolean type, + is treated as logical OR
 * for other types, it is normal addition
 */
template <Arithmetic T>
inline T arithmetic_add(T left, T right)
{
    if constexpr (std::is_same_v<T, bool>) {
        return left || right;
    }
    else {
        return left + right;
    }
}

/**
 * for boolean type, - is treated as left AND (NOT right)
 * for other types, it is normal subtraction
 */
template <Arithmetic T>
inline T arithmetic_sub(T left, T right)
{
    if constexpr (std::is_same_v<T, bool>) {
        return left && !right;
    }
    else {
        return left - right;
    }
}

/**
 * for boolean type, * is treated as logical AND
 * for other types, it is normal multiplication
 */
template <Arithmetic T>
inline T arithmetic_multiply(T left, T right)
{
    if constexpr (std::is_same_v<T, bool>) {
        return left && right;
    }
    else {
        return left * right;
    }
}

/**
 * for boolean type, / is treated as logical AND
 * for other types, it is normal division
 */
template <Arithmetic T>
inline T arithmetic_division(T left, T right)
{
    if constexpr (std::is_same_v<T, bool>) {
        return left && right;
    }
    else {
        return left / right;
    }
}

// template <Real T>
// inline T fast_dot();

VI_GE_NS_END