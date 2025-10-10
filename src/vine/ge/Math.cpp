#include <vine/ge/Math.hpp>

#include <cmath>

VI_GE_NS_BEGIN

template <FP T> bool isZero(T val, T eps) {
    if (std::isnan(val)) return false;
    if (std::isnan(eps)) return val == 0;
    if (eps < 0) eps = -eps;
    return val >= -eps && val <= eps;
}
template <FP T> bool isEqual(T a, T b, T eps) {
    if (std::isnan(a) || std::isnan(b)) return false;
    if (std::isnan(eps)) return a == b;
    if (eps < 0) eps = -eps;
    if (a > b) {
        return b >= a - eps;
    }
    return a >= b - eps;
}


// template <FP T> bool isZero(const Vector2<T>& vec2, T eps) {
//     return isZero(vec2.x, eps) && isZero(vec2.y, eps);
// }
// template <FP T> bool isZero(const Vector3<T>& vec3, T eps) {
//     return isZero(vec3.x, eps) && isZero(vec3.y, eps) && isZero(vec3.z, eps);
// }
// template <FP T> bool isZero(const Vector4<T>& vec4, T eps) {
//     return isZero(vec4.x, eps) && isZero(vec4.y, eps) && isZero(vec4.z, eps) && isZero(vec4.w, eps);
// }

// template <FP T> bool isEqual(const Vector2<T>& lhs, const Vector2<T>& rhs, T eps) {
//     return isEqual(lhs.x, rhs.x, eps) && isEqual(lhs.y, rhs.y, eps);
// }
// template <FP T> bool isEqual(const Vector3<T>& lhs, const Vector3<T>& rhs, T eps) {
//     return isEqual(lhs.x, rhs.x, eps) && isEqual(lhs.y, rhs.y, eps) && isEqual(lhs.z, rhs.z, eps);
// }
// template <FP T> bool isEqual(const Vector4<T>& lhs, const Vector4<T>& rhs, T eps) {
//     return isEqual(lhs.x, rhs.x, eps) && isEqual(lhs.y, rhs.y, eps) && isEqual(lhs.z, rhs.z, eps) && isEqual(lhs.w, rhs.w, eps);
// }

// template <FP T> T length(const Vector2<T>& vec2) {
//     return sqrt(length2<T>(vec2));
// }
// template <FP T> T length(const Vector3<T>& vec3) {
//     return sqrt(length2<T>(vec3));
// }
// template <FP T> T length(const Vector4<T>& vec4) {
//     return sqrt(length2<T>(vec4));
// }

// template <FP T> T length2(const Vector2<T>& vec2) {
//     return vec2.x * vec2.x + vec2.y * vec2.y;
// }
// template <FP T> T length2(const Vector3<T>& vec3) {
//     return vec3.x * vec3.x + vec3.y * vec3.y + vec3.z * vec3.z;
// }
// template <FP T> T length2(const Vector4<T>& vec4) {
//     return vec4.x * vec4.x + vec4.y * vec4.y + vec4.z * vec4.z + vec4.w * vec4.w;
// }

// template <FP T> T normalize(Vector2<T>& vec2) {
//     auto len = length<T>(vec2);
//     if (len > 0) {
//         vec2 /= len;
//     }
//     return len;
// }
// template <FP T> T normalize(Vector3<T>& vec3) {
//     auto len = length<T>(vec3);
//     if (len > 0) {
//         vec3 /= len;
//     }
//     return len;
// }
// template <FP T> T normalize(Vector4<T>& vec4) {
//     auto len = length<T>(vec4);
//     if (len > 0) {
//         vec4 /= len;
//     }
//     return len;
// }

// template <FP T> T dot(const Vector2<T>& lhs, const Vector2<T>& rhs) {
//     return lhs.x * rhs.x + lhs.y * rhs.y;
// }
// template <FP T> T dot(const Vector3<T>& lhs, const Vector3<T>& rhs) {
//     return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
// }
// template <FP T> T dot(const Vector4<T>& lhs, const Vector4<T>& rhs) {
//     return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
// }

// template <FP T> T cross(const Vector2<T>& lhs, const Vector2<T>& rhs) {
//     return lhs.x * rhs.y - lhs.y * rhs.x;
// }
// template <FP T> Vector3<T> cross(const Vector3<T>& lhs, const Vector3<T>& rhs) {
//     return Vector3<T>(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
// }

// template <FP T> T operator*(const Vector2<T>& lhs, const Vector2<T>& rhs) {
//     return dot(lhs, rhs);
// }
// template <FP T> T operator*(const Vector3<T>& lhs, const Vector3<T>& rhs) {
//     return dot(lhs, rhs);
// }
// template <FP T> T operator*(const Vector4<T>& lhs, const Vector4<T>& rhs) {
//     return dot(lhs, rhs);
// }

// template <FP T> T operator^(const Vector2<T>& lhs, const Vector2<T>& rhs) {
//     return cross(lhs, rhs);
// }
// template <FP T> Vector3<T> operator^(const Vector3<T>& lhs, const Vector3<T>& rhs) {
//     return cross(lhs, rhs);
// }

// template <FP T> T angle(const Vector2<T>& lhs, const Vector2<T>& rhs) {
//     auto len1 = length(lhs);
//     if (len1 == 0. || std::isnan(len1)) return 0;

//     auto len2 = length(rhs);
//     if (len2 == 0. || std::isnan(len2)) return 0;

//     auto d = dot(lhs, rhs);
//     auto c = d / len1 * len2;
//     if (c < T(-1)) c = -1;
//     if (c > T(1)) c = 1;
//     return acos(c);
// }
// template <FP T> T angle(const Vector3<T>& lhs, const Vector3<T>& rhs) {
//     auto len1 = length(lhs);
//     if (len1 == 0 || std::isnan(len1)) return 0;

//     auto len2 = length(rhs);
//     if (len2 == 0 || std::isnan(len2)) return 0;

//     auto d = dot(lhs, rhs);
//     auto c = d / len1 * len2;
//     if (c < -1) c = -1;
//     if (c > -1) c = 1;
//     return acos(c);
// }
// template <FP T> T angle(const Vector4<T>& lhs, const Vector4<T>& rhs) {
//     auto len1 = length(lhs);
//     if (len1 == 0 || std::isnan(len1)) return 0;

//     auto len2 = length(rhs);
//     if (len2 == 0 || std::isnan(len2)) return 0;

//     auto d = dot(lhs, rhs);
//     auto c = d / len1 * len2;
//     if (c < -1) c = -1;
//     if (c > -1) c = 1;
//     return acos(c);
// }
// template <FP T> T angle(const Vector3<T>& lhs, const Vector3<T>& rhs, const Vector3<T>& ref) {
//     auto rad = angle(lhs, rhs);

//     if (dot(cross(lhs, rhs), ref) > 0) {
//         return rad;
//     }

//     return ge::PIPI - rad;
// }

template VI_GE_API bool isZero<float>(float, float);
template VI_GE_API bool isZero<double>(double, double);
template VI_GE_API bool isEqual<float>(float, float, float);
template VI_GE_API bool isEqual<double>(double, double, double);

// template VI_GE_API bool isZero(const Vector2<float>&, float);
// template VI_GE_API bool isZero(const Vector3<float>&, float);
// template VI_GE_API bool isZero(const Vector4<float>&, float);
// template VI_GE_API bool isZero(const Vector2<double>&, double);
// template VI_GE_API bool isZero(const Vector3<double>&, double);
// template VI_GE_API bool isZero(const Vector4<double>&, double);

// template VI_GE_API bool isEqual(const Vector2<float>&, const Vector2<float>&, float);
// template VI_GE_API bool isEqual(const Vector3<float>&, const Vector3<float>&, float);
// template VI_GE_API bool isEqual(const Vector4<float>&, const Vector4<float>&, float);
// template VI_GE_API bool isEqual(const Vector2<double>&, const Vector2<double>&, double);
// template VI_GE_API bool isEqual(const Vector3<double>&, const Vector3<double>&, double);
// template VI_GE_API bool isEqual(const Vector4<double>&, const Vector4<double>&, double);

// template VI_GE_API float  length(const Vector2<float>&);
// template VI_GE_API float  length(const Vector3<float>&);
// template VI_GE_API float  length(const Vector4<float>&);
// template VI_GE_API double length(const Vector2<double>&);
// template VI_GE_API double length(const Vector3<double>&);
// template VI_GE_API double length(const Vector4<double>&);

// template VI_GE_API float  length2(const Vector2<float>&);
// template VI_GE_API float  length2(const Vector3<float>&);
// template VI_GE_API float  length2(const Vector4<float>&);
// template VI_GE_API double length2(const Vector2<double>&);
// template VI_GE_API double length2(const Vector3<double>&);
// template VI_GE_API double length2(const Vector4<double>&);

// template VI_GE_API float  normalize(Vector2<float>&);
// template VI_GE_API float  normalize(Vector3<float>&);
// template VI_GE_API float  normalize(Vector4<float>&);
// template VI_GE_API double normalize(Vector2<double>&);
// template VI_GE_API double normalize(Vector3<double>&);
// template VI_GE_API double normalize(Vector4<double>&);

// template VI_GE_API float  dot(const Vector2<float>&, const Vector2<float>&);
// template VI_GE_API float  dot(const Vector3<float>&, const Vector3<float>&);
// template VI_GE_API float  dot(const Vector4<float>&, const Vector4<float>&);
// template VI_GE_API double dot(const Vector2<double>&, const Vector2<double>&);
// template VI_GE_API double dot(const Vector3<double>&, const Vector3<double>&);
// template VI_GE_API double dot(const Vector4<double>&, const Vector4<double>&);

// template VI_GE_API float cross(const Vector2<float>&, const Vector2<float>&);
// template VI_GE_API Vector3<float> cross(const Vector3<float>&, const Vector3<float>&);
// template VI_GE_API double         cross(const Vector2<double>&, const Vector2<double>&);
// template VI_GE_API Vector3<double> cross(const Vector3<double>&, const Vector3<double>&);

// template VI_GE_API float  operator*(const Vector2<float>&, const Vector2<float>&);
// template VI_GE_API float  operator*(const Vector3<float>&, const Vector3<float>&);
// template VI_GE_API float  operator*(const Vector4<float>&, const Vector4<float>&);
// template VI_GE_API double operator*(const Vector2<double>&, const Vector2<double>&);
// template VI_GE_API double operator*(const Vector3<double>&, const Vector3<double>&);
// template VI_GE_API double operator*(const Vector4<double>&, const Vector4<double>&);

// template VI_GE_API float operator^(const Vector2<float>&, const Vector2<float>&);
// template VI_GE_API Vector3<float> operator^(const Vector3<float>&, const Vector3<float>&);
// template VI_GE_API double         operator^(const Vector2<double>&, const Vector2<double>&);
// template VI_GE_API Vector3<double> operator^(const Vector3<double>&, const Vector3<double>&);

// template VI_GE_API float  angle(const Vector2<float>&, const Vector2<float>&);
// template VI_GE_API float  angle(const Vector3<float>&, const Vector3<float>&);
// template VI_GE_API float  angle(const Vector4<float>&, const Vector4<float>&);
// template VI_GE_API double angle(const Vector2<double>&, const Vector2<double>&);
// template VI_GE_API double angle(const Vector3<double>&, const Vector3<double>&);
// template VI_GE_API double angle(const Vector4<double>&, const Vector4<double>&);

// template VI_GE_API float  angle(const Vector3<float>& lhs, const Vector3<float>& rhs, const Vector3<float>& ref);
// template VI_GE_API double angle(const Vector3<double>& lhs, const Vector3<double>& rhs, const Vector3<double>& ref);

VI_GE_NS_END