#pragma once

#include "math_global.hpp"

#include "Types.hpp"

VI_MATH_NS_BEGIN

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

template <Real T> bool isZero(T val, T eps);
template <Real T> bool isEqual(T a, T b, T eps);

VI_MATH_NS_END