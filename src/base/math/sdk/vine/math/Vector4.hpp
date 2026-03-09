#pragma once

#include "math_global.hpp"

#include <cstddef>
#include <cstdint>

#include "Math.hpp"

VI_MATH_NS_BEGIN
template <typename T>
class Vector3;

/**
 * @brief A class representing a vector in 4D space
 * @tparam T Only accepts float double and integers(include boolean)
 */
template <typename T>
class Vector4 {
  public:
    using value_type = T;

  public:
    constexpr Vector4()
      : x(T())
      , y(T())
      , z(T())
      , w(T())
    {}

    constexpr Vector4(const Vector3<T>& vec3, T ww = 1.0)
      : x(vec3.x)
      , y(vec3.y)
      , z(vec3.z)
      , w(ww)
    {}

    constexpr Vector4(T xx, T yy, T zz, T ww)
      : x(xx)
      , y(yy)
      , z(zz)
      , w(ww)
    {}

  public:
    constexpr void set(const Vector3<T>& vec3)
    {
        x = vec3.x;
        y = vec3.y;
        z = vec3.z;
    }

    constexpr void set(const Vector3<T>& vec3, T ww)
    {
        x = vec3.x;
        y = vec3.y;
        z = vec3.z;
        w = ww;
    }

    constexpr void set(T xx, T yy, T zz, T ww)
    {
        x = xx;
        y = yy;
        z = zz;
        w = ww;
    }

    constexpr void get(T& xx, T& yy, T& zz, T& ww) const
    {
        xx = x;
        yy = y;
        zz = z;
        ww = w;
    }

    [[nodiscard]]
    constexpr const Vector3<T>& asVector3() const
    {
        return reinterpret_cast<const Vector3<T>&>(*this);
    }

    /**
     * @brief dot product
     *        only for real types (floating point and integers) not boolean.
     *        for integer types, overflow is possible.
     */

    [[nodiscard]]
    constexpr T dot(const Vector4<T>& other) const requires(Real<T>)
    {
        return static_cast<T>(x * other.x + y * other.y + z * other.z + w * other.w);
    }

    [[nodiscard]]
    constexpr TypeF<T> length2() const requires(Real<T>)
    {
        return safeLengthSquared(x, y, z, w);
    }

    /**
     * @brief length of the vector
     *        only for real types (floating point and integers) not boolean.
     */
    [[nodiscard]]
    constexpr TypeF<T> length() const requires(Real<T>)
    {
        return safeLength(x, y, z, w);
    }

    [[nodiscard]]
    constexpr TypeF<T> angleTo(const Vector4<T>& other) const requires(Real<T>)
    {
        using ft = TypeF<T>;
        auto dot = static_cast<ft>(x) * static_cast<ft>(other.x) + static_cast<ft>(y) * static_cast<ft>(other.y) +
                   static_cast<ft>(z) * static_cast<ft>(other.z) + static_cast<ft>(w) * static_cast<ft>(other.w);
        auto cross_x = static_cast<ft>(y) * static_cast<ft>(other.z) - static_cast<ft>(z) * static_cast<ft>(other.y) +
                       static_cast<ft>(w) * static_cast<ft>(other.x) - static_cast<ft>(x) * static_cast<ft>(other.w);
        auto cross_y = static_cast<ft>(z) * static_cast<ft>(other.x) - static_cast<ft>(x) * static_cast<ft>(other.z) +
                       static_cast<ft>(w) * static_cast<ft>(other.y) - static_cast<ft>(y) * static_cast<ft>(other.w);
        auto cross_z = static_cast<ft>(x) * static_cast<ft>(other.y) - static_cast<ft>(y) * static_cast<ft>(other.x) +
                       static_cast<ft>(w) * static_cast<ft>(other.z) - static_cast<ft>(z) * static_cast<ft>(other.w);
        auto cross_w = static_cast<ft>(x) * static_cast<ft>(other.w) - static_cast<ft>(w) * static_cast<ft>(other.x) +
                       static_cast<ft>(y) * static_cast<ft>(other.z) - static_cast<ft>(z) * static_cast<ft>(other.y);
        auto cross_len = std::sqrt(cross_x * cross_x + cross_y * cross_y + cross_z * cross_z + cross_w * cross_w);

        return std::atan2(cross_len, dot);
    }

    /**
     * @brief normalize the vector to unit length.
     *        only for floating point types.
     */
    [[nodiscard]]
    constexpr T normalize() requires(FP<T>)
    {
        auto len = length();

        if (len == T(0)) {
            x = T(0);
            y = T(0);
            z = T(0);
            w = T(0);
        }
        else {
            x /= len;
            y /= len;
            z /= len;
            w /= len;
        }

        return len;
    }

    [[nodiscard]]
    constexpr bool isZero() const
    {
        return x == T() && y == T() && z == T() && w == T();
    }

    [[nodiscard]]
    constexpr bool isZero(T eps) const requires(Real<T>)
    {
        return math::isZero<T>(x, eps) && math::isZero<T>(y, eps) && math::isZero<T>(z, eps) && math::isZero<T>(w, eps);
    }

    [[nodiscard]]
    constexpr bool isEqual(const Vector4<T>& other) const
    {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    [[nodiscard]]
    constexpr bool isEqual(const Vector4<T>& other, T eps) const requires(Real<T>)
    {
        return math::isEqual<T>(x, other.x, eps) && math::isEqual<T>(y, other.y, eps) && math::isEqual<T>(z, other.z, eps) && math::isEqual<T>(w, other.w, eps);
    }

  public:
    [[nodiscard]]
    constexpr bool operator==(const Vector4<T>& right) const
    {
        return x == right.x && y == right.y && z == right.z && w == right.w;
    }

    [[nodiscard]]
    constexpr bool operator!=(const Vector4<T>& right) const
    {
        return !(*this == right);
    }

    [[nodiscard]]
    constexpr Vector4<T> operator+(const Vector4<T>& right) const
    {
        return Vector4<T>(arithmeticAdd(x, right.x), arithmeticAdd(y, right.y), arithmeticAdd(z, right.z), arithmeticAdd(w, right.w));
    }

    [[nodiscard]]
    constexpr Vector4<T> operator-(const Vector4<T>& right) const
    {
        return Vector4<T>(arithmeticSub(x, right.x), arithmeticSub(y, right.y), arithmeticSub(z, right.z), arithmeticSub(w, right.w));
    }

    [[nodiscard]]
    constexpr Vector4<T> operator*(T scale) const
    {
        return Vector4<T>(arithmeticMultiply(x, scale), arithmeticMultiply(y, scale), arithmeticMultiply(z, scale), arithmeticMultiply(w, scale));
    }

    [[nodiscard]]
    constexpr Vector4<T> operator/(T scale) const
    {
        return Vector4<T>(arithmeticDivision(x, scale), arithmeticDivision(y, scale), arithmeticDivision(z, scale), arithmeticDivision(w, scale));
    }

    constexpr Vector4<T>& operator+=(const Vector4<T>& right)
    {
        x = arithmeticAdd(x, right.x);
        y = arithmeticAdd(y, right.y);
        z = arithmeticAdd(z, right.z);
        w = arithmeticAdd(w, right.w);

        return *this;
    }

    constexpr Vector4<T>& operator-=(const Vector4<T>& right)
    {
        x = arithmeticSub(x, right.x);
        y = arithmeticSub(y, right.y);
        z = arithmeticSub(z, right.z);
        w = arithmeticSub(w, right.w);

        return *this;
    }

    constexpr Vector4<T>& operator*=(T scale)
    {
        x = arithmeticMultiply(x, scale);
        y = arithmeticMultiply(y, scale);
        z = arithmeticMultiply(z, scale);
        w = arithmeticMultiply(w, scale);

        return *this;
    }

    constexpr Vector4<T>& operator/=(T scale)
    {
        x = arithmeticDivision(x, scale);
        y = arithmeticDivision(y, scale);
        z = arithmeticDivision(z, scale);
        w = arithmeticDivision(w, scale);

        return *this;
    }

    [[nodiscard]]
    constexpr Vector4<T> operator-() const
    {
        return Vector4<T>(arithmeticNagate(x), arithmeticNagate(y), arithmeticNagate(z), arithmeticNagate(w));
    }

    /**
     * @brief dot product
     */
    [[nodiscard]]
    constexpr T operator*(const Vector4<T>& other) const requires(Real<T>)
    {
        return dot(other);
    }

    [[nodiscard]]
    constexpr T& operator[](size_t index)
    {
        // assert(index < 4);
        return data[index];
    }

    [[nodiscard]]
    constexpr const T& operator[](size_t index) const
    {
        // assert(index < 4);
        return data[index];
    }

  public:
    union
    {
        struct {
            T x, y, z, w;
        };

        T data[4];
    };
};

using Vec4b    = Vector4<bool>;
using Vec4i8   = Vector4<int8_t>;
using Vec4ui8  = Vector4<uint8_t>;
using Vec4i16  = Vector4<int16_t>;
using Vec4ui16 = Vector4<uint16_t>;
using Vec4i32  = Vector4<int32_t>;
using Vec4ui32 = Vector4<uint32_t>;
using Vec4i64  = Vector4<int64_t>;
using Vec4ui64 = Vector4<uint64_t>;
using Vec4i    = Vec4i32;
using Vec4ui   = Vec4ui32;
using Vec4f    = Vector4<float>;
using Vec4d    = Vector4<double>;

VI_MATH_NS_END
