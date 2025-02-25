#pragma once
#include "ge_global.h"

#include <cmath>

VI_GE_NS_BEGIN

constexpr double PI   = 3.14159265358979323846;
constexpr double PIPI = 6.283185307179586;
constexpr double PI_2 = 1.57079632679489661923;
constexpr double PI_4 = 0.785398163397448309616;

constexpr double E      = 2.71828182845904523536;  // e
constexpr double LOG2E  = 1.44269504088896340736;  // log2(e)
constexpr double LOG10E = 0.434294481903251827651; // log10(e)
constexpr double LN2    = 0.693147180559945309417; // ln(2)
constexpr double LN10   = 2.30258509299404568402;  // ln(10)

constexpr float  EPSF = 1e-6;
constexpr double EPSD = 1e-8;

template <typename T> bool isZero(T val, T eps) {
    //if (isnan(val) || isnan(eps)) return true;
    if (eps < 0) eps = -eps;
    return val >= -eps && val <= eps;
}

template <typename T> bool isEqual(T a, T b, T eps) {
    //if (isnan(a) || isnan(b) || isnan(eps)) return false;
    if (eps < 0) eps = -eps;
    if (a > b) {
        return b >= a - eps;
    }
    return a >= b - eps;
}

VI_GE_NS_END