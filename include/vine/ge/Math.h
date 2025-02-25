#pragma once

#include "ge_global.h"

#include <concepts>

#include "Types.h"

VI_GE_NS_BEGIN

template <typename T>
concept FP = std::is_floating_point<T>::value;

template <FP T> T length(const Vector2<T>& vec2);
template <FP T> T length(const Vector3<T>& vec3);
template <FP T> T length(const Vector4<T>& vec4);

template <FP T> T length2(const Vector2<T>& vec2);
template <FP T> T length2(const Vector3<T>& vec3);
template <FP T> T length2(const Vector4<T>& vec4);

template <FP T> T normalize(Vector2<T>& vec2);
template <FP T> T normalize(Vector3<T>& vec3);
template <FP T> T normalize(Vector4<T>& vec4);

template <FP T> T dot(const Vector2<T>& lhs, const Vector2<T>& rhs);
template <FP T> T dot(const Vector3<T>& lhs, const Vector3<T>& rhs);
template <FP T> T dot(const Vector4<T>& lhs, const Vector4<T>& rhs);

template <FP T> T          cross(const Vector2<T>& lhs, const Vector2<T>& rhs);
template <FP T> Vector3<T> cross(const Vector3<T>& lhs, const Vector3<T>& rhs);

template <FP T> T operator*(const Vector2<T>& lhs, const Vector2<T>& rhs);
template <FP T> T operator*(const Vector3<T>& lhs, const Vector3<T>& rhs);
template <FP T> T operator*(const Vector4<T>& lhs, const Vector4<T>& rhs);

template <FP T> T          operator^(const Vector2<T>& lhs, const Vector2<T>& rhs);
template <FP T> Vector3<T> operator^(const Vector3<T>& lhs, const Vector3<T>& rhs);

VI_GE_NS_END