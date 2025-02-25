#include <vine/ge/Math.h>

#include <cmath>

VI_GE_NS_BEGIN

template <FP T> T length(const Vector2<T>& vec2) {
    return sqrt(length2<T>(vec2));
}
template <FP T> T length(const Vector3<T>& vec3) {
    return sqrt(length2<T>(vec3));
}
template <FP T> T length(const Vector4<T>& vec4) {
    return sqrt(length2<T>(vec4));
}

template <FP T> T length2(const Vector2<T>& vec2) {
    return vec2.x * vec2.x + vec2.y * vec2.y;
}
template <FP T> T length2(const Vector3<T>& vec3) {
    return vec3.x * vec3.x + vec3.y * vec3.y + vec3.z * vec3.z;
}
template <FP T> T length2(const Vector4<T>& vec4) {
    return vec4.x * vec4.x + vec4.y * vec4.y + vec4.z * vec4.z + vec4.w * vec4.w;
}

template <FP T> T normalize(Vector2<T>& vec2) {
    auto len = length<T>(vec2);
    if (len > 0) {
        vec2 /= len;
    }
    return len;
}
template <FP T> T normalize(Vector3<T>& vec3) {
    auto len = length<T>(vec3);
    if (len > 0) {
        vec3 /= len;
    }
    return len;
}
template <FP T> T normalize(Vector4<T>& vec4) {
    auto len = length<T>(vec4);
    if (len > 0) {
        vec4 /= len;
    }
    return len;
}

template <FP T> T dot(const Vector2<T>& lhs, const Vector2<T>& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}
template <FP T> T dot(const Vector3<T>& lhs, const Vector3<T>& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
template <FP T> T dot(const Vector4<T>& lhs, const Vector4<T>& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

template <FP T> T cross(const Vector2<T>& lhs, const Vector2<T>& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}
template <FP T> Vector3<T> cross(const Vector3<T>& lhs, const Vector3<T>& rhs) {
    return Vector3<T>(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
}

template <FP T> T operator*(const Vector2<T>& lhs, const Vector2<T>& rhs) {
    return dot(lhs, rhs);
}
template <FP T> T operator*(const Vector3<T>& lhs, const Vector3<T>& rhs) {
    return dot(lhs, rhs);
}
template <FP T> T operator*(const Vector4<T>& lhs, const Vector4<T>& rhs) {
    return dot(lhs, rhs);
}

template <FP T> T operator^(const Vector2<T>& lhs, const Vector2<T>& rhs) {
    return cross(lhs, rhs);
}
template <FP T> Vector3<T> operator^(const Vector3<T>& lhs, const Vector3<T>& rhs) {
    return cross(lhs, rhs);
}

template VI_GE_API float  length(const Vector2<float>&);
template VI_GE_API float  length(const Vector3<float>&);
template VI_GE_API float  length(const Vector4<float>&);
template VI_GE_API double length(const Vector2<double>&);
template VI_GE_API double length(const Vector3<double>&);
template VI_GE_API double length(const Vector4<double>&);

template VI_GE_API float  length2(const Vector2<float>&);
template VI_GE_API float  length2(const Vector3<float>&);
template VI_GE_API float  length2(const Vector4<float>&);
template VI_GE_API double length2(const Vector2<double>&);
template VI_GE_API double length2(const Vector3<double>&);
template VI_GE_API double length2(const Vector4<double>&);

template VI_GE_API float  normalize(Vector2<float>&);
template VI_GE_API float  normalize(Vector3<float>&);
template VI_GE_API float  normalize(Vector4<float>&);
template VI_GE_API double normalize(Vector2<double>&);
template VI_GE_API double normalize(Vector3<double>&);
template VI_GE_API double normalize(Vector4<double>&);

template VI_GE_API float  dot(const Vector2<float>&, const Vector2<float>&);
template VI_GE_API float  dot(const Vector3<float>&, const Vector3<float>&);
template VI_GE_API float  dot(const Vector4<float>&, const Vector4<float>&);
template VI_GE_API double dot(const Vector2<double>&, const Vector2<double>&);
template VI_GE_API double dot(const Vector3<double>&, const Vector3<double>&);
template VI_GE_API double dot(const Vector4<double>&, const Vector4<double>&);

template VI_GE_API float cross(const Vector2<float>&, const Vector2<float>&);
template VI_GE_API Vector3<float> cross(const Vector3<float>&, const Vector3<float>&);
template VI_GE_API double         cross(const Vector2<double>&, const Vector2<double>&);
template VI_GE_API Vector3<double> cross(const Vector3<double>&, const Vector3<double>&);

template VI_GE_API float  operator*(const Vector2<float>&, const Vector2<float>&);
template VI_GE_API float  operator*(const Vector3<float>&, const Vector3<float>&);
template VI_GE_API float  operator*(const Vector4<float>&, const Vector4<float>&);
template VI_GE_API double operator*(const Vector2<double>&, const Vector2<double>&);
template VI_GE_API double operator*(const Vector3<double>&, const Vector3<double>&);
template VI_GE_API double operator*(const Vector4<double>&, const Vector4<double>&);

template VI_GE_API float operator^(const Vector2<float>&, const Vector2<float>&);
template VI_GE_API Vector3<float> operator^(const Vector3<float>&, const Vector3<float>&);
template VI_GE_API double         operator^(const Vector2<double>&, const Vector2<double>&);
template VI_GE_API Vector3<double> operator^(const Vector3<double>&, const Vector3<double>&);

VI_GE_NS_END