#pragma once

#include "math_global.hpp"

#include <cmath>
#
#include "Types.hpp"

V_MATH_NS_BEGIN

constexpr double PI   = 3.14159265358979323846;
constexpr double PIPI = 6.283185307179586;
constexpr double PI_2 = 1.57079632679489661923;
constexpr double PI_4 = 0.785398163397448309616;

constexpr double E      = 2.71828182845904523536;  // e
constexpr double LOG2E  = 1.44269504088896340736;  // log2(e)
constexpr double LOG10E = 0.434294481903251827651; // log10(e)
constexpr double LN2    = 0.693147180559945309417; // ln(2)
constexpr double LN10   = 2.30258509299404568402;  // ln(10)

constexpr float  EPSF = 1e-6f;
constexpr double EPSD = 1e-8;

template <typename S>
constexpr S EPS()
{
    return S(0);
}

template <>
constexpr float EPS<float>()
{
    return EPSF;
}

template <>
constexpr double EPS<double>()
{
    return EPSD;
}

constexpr double RAD_TO_DEG = 180.0 / PI;
constexpr double DEG_TO_RAD = PI / 180.0;

/*
 * @brief Check if a value is zero within a given epsilon.
 */
template <Real T>
[[nodiscard]]
constexpr bool isZero(T val, T eps)
{
    if constexpr (FP<T>) {
        if (std::isnan(val) || std::isinf(val))
            return false;
        else if (std::isinf(eps))
            return true;
        else if (std::isnan(eps))
            return val == T(0);
    }

    if constexpr (std::is_signed_v<T> || FP<T>) {
        eps = std::abs(eps);
        return val >= -eps && val <= eps;
    }
    else {
        return val <= eps;
    }
}

/*
 * @brief Check if two values are equal within a given epsilon.
 */
template <Real T>
[[nodiscard]]
constexpr bool isEqual(T a, T b, T eps)
{
    if constexpr (FP<T>) {
        if (std::isnan(a) || std::isnan(b) || std::isinf(a) || std::isinf(b))
            return false;
        else if (std::isinf(eps))
            return true;
        else if (std::isnan(eps))
            return a == b;
    }

    if constexpr (std::is_signed_v<T> || FP<T>) {
        eps = std::abs(eps);
    }

    return (a > b) ? (a - b < eps) : (b - a < eps);
}

/**
 * @brief Safe multiplication, for floating point types, it is just normal multiplication,
 *        for integer types, it is promoted to double first, then multiplied.
 */
template <Real T>
[[nodiscard]]
constexpr TypeF<T> safeMultiply(T first, T second)
{
    if constexpr (FP<T>) {
        return first * second;
    }
    else {
        using ft = TypeF<T>;

        return static_cast<ft>(first) * static_cast<ft>(second);
    }
}

/**
 * @brief Safe calculation of vector length squared, for floating point types, it is just normal calculation,
 *        for integer types, it is promoted to double first, then calculated.
 */
// computeVectorLength2Safe
template <Real T, Real... Rest>
[[nodiscard]]
constexpr TypeF<T> safeLengthSquared(T first, Rest... rest)
{
    // static_assert((std::is_same_v<T, Rest> && ...), "All parameters must have type T");

    if constexpr (FP<T>) {
        return first * first + (... + (rest * rest));
    }
    else {
        using ft = TypeF<T>;
        auto sum = static_cast<ft>(first) * static_cast<ft>(first);
        ((sum += static_cast<ft>(rest) * static_cast<ft>(rest)), ...);
        return sum;
    }
}

/**
 * @brief Safe calculation of vector length, for floating point types, it is just normal calculation,
 *        for integer types, it is promoted to double first, then calculated.
 */
template <Real T, Real... Rest>
[[nodiscard]]
constexpr TypeF<T> safeLength(T first, Rest... rest)
{
    return std::sqrt(safeLengthSquared(first, rest...));
}

/**
 * for boolean type, + is treated as logical OR
 * for other types, it is normal addition
 */
template <Arithmetic T>
[[nodiscard]]
constexpr T arithmeticAdd(T left, T right)
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
[[nodiscard]]
constexpr T arithmeticSub(T left, T right)
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
[[nodiscard]]
constexpr T arithmeticMultiply(T left, T right)
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
[[nodiscard]]
constexpr T arithmeticDivision(T left, T right)
{
    if constexpr (std::is_same_v<T, bool>) {
        return left && right;
    }
    else {
        return left / right;
    }
}

template <Arithmetic T>
[[nodiscard]]
constexpr T arithmeticNagate(T left)
{
    if constexpr (std::is_same_v<T, bool>) {
        return !left;
    }
    else {
        return -left;
    }
}

V_MATH_NS_END
