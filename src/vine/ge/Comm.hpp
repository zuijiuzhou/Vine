#pragma once

#include <vine/ge/ge_global.hpp>

#include <cmath>

#include <vine/ge/Types.hpp>

VI_GE_NS_BEGIN

template <Real T, typename... Rest>
inline LengthType<T> calc_vec_len2_safe(T first, Rest... rest)
{
    // static_assert((std::is_same_v<T, Rest> && ...), "All parameters must have type T");

    if constexpr (FP<T>) {
        return first * first + (... + (rest * rest));
    }
    else {
        using LT = LengthType<T>;

        auto sum = static_cast<LT>(first) * static_cast<LT>(first);

        ((sum += static_cast<LT>(rest) * static_cast<LT>(rest)), ...);

        return sum;
    }
}

template <Real T, typename... Rest>
inline LengthType<T> calc_vec_len_safe(T first, Rest... rest)
{
    return std::sqrt(calc_vec_len2_safe(first, rest...));
}

template <Real T>
inline LengthType<T> multiply_safe(T first, T second)
{
    if constexpr (FP<T>) {
        return first * second;
    }
    else {
        using LT = LengthType<T>;

        return static_cast<LT>(first) * static_cast<LT>(first);
    }
}

template <Arithmetic T>
inline T advance_add(T left, T right)
{
    if constexpr (std::is_same_v<T, bool>) {
        return left || right;
    }
    else {
        return left + right;
    }
}

template <Arithmetic T>
inline T advance_sub(T left, T right)
{
    if constexpr (std::is_same_v<T, bool>) {
        return left && !right;
    }
    else {
        return left - right;
    }
}

template <Arithmetic T>
inline T advance_multiply(T left, T right)
{
    if constexpr (std::is_same_v<T, bool>) {
        return left && right;
    }
    else {
        return left * right;
    }
}

template <Arithmetic T>
inline T advance_division(T left, T right)
{
    if constexpr (std::is_same_v<T, bool>) {
        return left && right;
    }
    else {
        return left / right;
    }
}

VI_GE_NS_END