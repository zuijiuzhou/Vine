#include <vine/math/Math.hpp>

#include <cmath>

VI_MATH_NS_BEGIN

template <Real T>
bool isZero(T val, T eps)
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

template <Real T>
bool isEqual(T a, T b, T eps)
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

template VI_MATH_API bool isZero<float>(float, float);
template VI_MATH_API bool isZero<double>(double, double);
template VI_MATH_API bool isZero<int8_t>(int8_t, int8_t);
template VI_MATH_API bool isZero<uint8_t>(uint8_t, uint8_t);
template VI_MATH_API bool isZero<int16_t>(int16_t, int16_t);
template VI_MATH_API bool isZero<uint16_t>(uint16_t, uint16_t);
template VI_MATH_API bool isZero<int32_t>(int32_t, int32_t);
template VI_MATH_API bool isZero<uint32_t>(uint32_t, uint32_t);
template VI_MATH_API bool isZero<int64_t>(int64_t, int64_t);
template VI_MATH_API bool isZero<uint64_t>(uint64_t, uint64_t);
template VI_MATH_API bool isEqual<float>(float, float, float);
template VI_MATH_API bool isEqual<double>(double, double, double);
template VI_MATH_API bool isEqual<int8_t>(int8_t, int8_t, int8_t);
template VI_MATH_API bool isEqual<uint8_t>(uint8_t, uint8_t, uint8_t);
template VI_MATH_API bool isEqual<int16_t>(int16_t, int16_t, int16_t);
template VI_MATH_API bool isEqual<uint16_t>(uint16_t, uint16_t, uint16_t);
template VI_MATH_API bool isEqual<int32_t>(int32_t, int32_t, int32_t);
template VI_MATH_API bool isEqual<uint32_t>(uint32_t, uint32_t, uint32_t);
template VI_MATH_API bool isEqual<int64_t>(int64_t, int64_t, int64_t);
template VI_MATH_API bool isEqual<uint64_t>(uint64_t, uint64_t, uint64_t);

VI_MATH_NS_END