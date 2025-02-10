#pragma once
#include "ge_global.h"

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

constexpr inline bool isZero(float val, float eps) {
    if (eps < 0) eps = -eps;
    return val >= -eps && val <= eps;
}

constexpr inline bool isZero(double val, double eps) {
    if (eps < 0) eps = -eps;
    return val >= -eps && val <= eps;
}

constexpr inline bool isEqual(float a, float b, float eps) {
    if (eps < 0) eps = -eps;
    if (a > b) {
        return b >= a - eps;
    }
    return a >= b - eps;
}

constexpr inline bool isEqual(double a, double b, double eps) {
    if (eps < 0) eps = -eps;
    if (a > b) {
        return b >= a - eps;
    }
    return a >= b - eps;
}

VI_GE_NS_END