#pragma once

#include "ge_global.hpp"

#include "Types.hpp"

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

constexpr float  EPSF = 1e-6f;
constexpr double EPSD = 1e-8;

template <Real T> bool isZero(T val, T eps);
template <Real T> bool isEqual(T a, T b, T eps);

// template <FP T> bool isZero(const Vector2<T>& vec2, T eps);
// template <FP T> bool isZero(const Vector3<T>& vec3, T eps);
// template <FP T> bool isZero(const Vector4<T>& vec4, T eps);
// template <FP T> bool isEqual(const Vector2<T>& lhs, const Vector2<T>& rhs, T eps);
// template <FP T> bool isEqual(const Vector3<T>& lhs, const Vector3<T>& rhs, T eps);
// template <FP T> bool isEqual(const Vector4<T>& lhs, const Vector4<T>& rhs, T eps);

// template <FP T> T length(const Vector2<T>& vec2);
// template <FP T> T length(const Vector3<T>& vec3);
// template <FP T> T length(const Vector4<T>& vec4);

// template <FP T> T length2(const Vector2<T>& vec2);
// template <FP T> T length2(const Vector3<T>& vec3);
// template <FP T> T length2(const Vector4<T>& vec4);

// template <FP T> T normalize(Vector2<T>& vec2);
// template <FP T> T normalize(Vector3<T>& vec3);
// template <FP T> T normalize(Vector4<T>& vec4);

// template <FP T> T dot(const Vector2<T>& lhs, const Vector2<T>& rhs);
// template <FP T> T dot(const Vector3<T>& lhs, const Vector3<T>& rhs);
// template <FP T> T dot(const Vector4<T>& lhs, const Vector4<T>& rhs);

// template <FP T> T          cross(const Vector2<T>& lhs, const Vector2<T>& rhs);
// template <FP T> Vector3<T> cross(const Vector3<T>& lhs, const Vector3<T>& rhs);

// template <FP T> T operator*(const Vector2<T>& lhs, const Vector2<T>& rhs);
// template <FP T> T operator*(const Vector3<T>& lhs, const Vector3<T>& rhs);
// template <FP T> T operator*(const Vector4<T>& lhs, const Vector4<T>& rhs);

// template <FP T> T          operator^(const Vector2<T>& lhs, const Vector2<T>& rhs);
// template <FP T> Vector3<T> operator^(const Vector3<T>& lhs, const Vector3<T>& rhs);

// template <FP T> T angle(const Vector2<T>& lhs, const Vector2<T>& rhs);
// template <FP T> T angle(const Vector3<T>& lhs, const Vector3<T>& rhs);
// template <FP T> T angle(const Vector4<T>& lhs, const Vector4<T>& rhs);

// template <FP T> T angle(const Vector3<T>& lhs, const Vector3<T>& rhs, const Vector3<T>& ref);

VI_GE_NS_END