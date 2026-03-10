#pragma once
#include "math_global.hpp"

#include <cstddef>
#include <cstdint>

#include "Math.hpp"

VI_MATH_NS_BEGIN

template <typename T>
class Point3;
template <typename T>
class Vector2;

/**
 * @brief A class representing a vector in 3D space
 * @tparam T Only accepts float double and integers(include boolean)
 */
template <typename T>
class Vector3 {
  public:
    using value_type = T;

  public:
    constexpr Vector3()
      : x(T())
      , y(T())
      , z(T())
    {}

    constexpr Vector3(const Vector2<T>& vec2, T zz = 0.)
      : x(vec2.x)
      , y(vec2.y)
      , z(zz)
    {}

    constexpr Vector3(T xx, T yy, T zz)
      : x(xx)
      , y(yy)
      , z(zz) {};

  public:
    constexpr void set(const Vector2<T>& vec2)
    {
        x = vec2.x;
        y = vec2.y;
    }

    constexpr void set(const Vector2<T>& vec2, T zz)
    {
        x = vec2.x;
        y = vec2.y;
        z = zz;
    }

    constexpr void set(T xx, T yy, T zz)
    {
        x = xx;
        y = yy;
        z = zz;
    }

    constexpr void get(T& xx, T& yy, T& zz) const
    {
        xx = x;
        yy = y;
        zz = z;
    }

    [[nodiscard]]
    Point3<T> toPoint() const;

    [[nodiscard]]
    constexpr const Point3<T>& asPoint() const
    {
        return reinterpret_cast<const Point3<T>&>(*this);
    }

    [[nodiscard]]
    constexpr const Vector2<T>& asVector2() const
    {
        return reinterpret_cast<const Vector2<T>&>(*this);
    }

    /**
     * @brief dot product
     *        only for real types (floating point and integers) not boolean.
     *        for integer types, overflow is possible.
     */
    [[nodiscard]]
    constexpr T dot(const Vector3<T>& other) const requires(Real<T>)
    {
        return static_cast<T>(x * other.x + y * other.y + z * other.z);
    }

    /**
     * @brief cross product (in 2D, it is a scalar)
     *        only for real types (floating point and integers) not boolean.
     *        for integer types, overflow is possible.
     */
    [[nodiscard]]
    constexpr Vector3<T> cross(const Vector3<T>& other) const requires(Real<T>)
    {
        return Vector3<T>(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }

    /**
     * @brief length of the vector
     *        only for real types (floating point and integers) not boolean.
     */
    [[nodiscard]]
    constexpr TypeF<T> length() const requires(Real<T>)
    {
        return safeLength(x, y, z);
    }

    [[nodiscard]]
    constexpr TypeF<T> length2() const requires(Real<T>)
    {
        return safeLengthSquared(x, y, z);
    }

    [[nodiscard]]
    constexpr TypeF<T> angleTo(const Vector3<T>& other) const requires(Real<T>)
    {
        /**
         * Algorithm: θ = atan2(|a||b|sinθ, a·b)
         * 
         * Derivation:
         *   cosθ = (a·b) / (|a||b|)
         *   sin²θ = 1 - cos²θ = 1 - (a·b)² / (|a|²|b|²)
         *        = (|a|²|b|² - (a·b)²) / (|a|²|b|²)
         *   
         *   Therefore: |a||b|sinθ = sqrt(|a|²|b|² - (a·b)²)
         *   
         *   θ = atan2(|a||b|sinθ, |a||b|cosθ)
         *     = atan2(sqrt(|a|²|b|² - (a·b)²), a·b)
         * 
         * Note: In 3D, this is equivalent to |a × b| = |a||b|sinθ
         * This approach is numerically stable compared to acos(dot/(|a||b|))
         * which suffers from precision loss when vectors are nearly parallel.
         */

        using ft = TypeF<T>;

        const ft x1 = static_cast<ft>(x);
        const ft y1 = static_cast<ft>(y);
        const ft z1 = static_cast<ft>(z);

        const ft x2 = static_cast<ft>(other.x);
        const ft y2 = static_cast<ft>(other.y);
        const ft z2 = static_cast<ft>(other.z);

        const ft dot = x1 * x2 + y1 * y2 + z1 * z2;
        const ft len1_sq = x1 * x1 + y1 * y1 + z1 * z1;
        const ft len2_sq = x2 * x2 + y2 * y2 + z2 * z2;

        const ft sin_theta_sq = len1_sq * len2_sq - dot * dot;
        const ft sin_theta    = std::sqrt(std::max(ft(0), sin_theta_sq));

        return std::atan2(sin_theta, dot);
    }

    /**
     * @brief normalize the vector to unit length.
     *        only for floating point types.
     */
    constexpr T normalize() requires(FP<T>)
    {
        auto len = length();

        if (len == T(0)) {
            x = T(0);
            y = T(0);
            z = T(0);
        }
        else {
            x /= len;
            y /= len;
            z /= len;
        }

        return len;
    }

    [[nodiscard]]
    constexpr bool isZero() const
    {
        return x == T() && y == T() && z == T();
    }

    [[nodiscard]]
    constexpr bool isZero(T eps) const requires(Real<T>)
    {
        return math::isZero<T>(x, eps) && math::isZero<T>(y, eps) && math::isZero<T>(z, eps);
    }

    [[nodiscard]]
    constexpr bool isEqual(const Vector3<T>& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    [[nodiscard]]
    constexpr bool isEqual(const Vector3<T>& other, T eps) const requires(Real<T>)
    {
        return math::isEqual<T>(x, other.x, eps) && math::isEqual<T>(y, other.y, eps) && math::isEqual<T>(z, other.z, eps);
    }

  public:
    [[nodiscard]]
    constexpr bool operator==(const Vector3<T>& right) const
    {
        return x == right.x && y == right.y && z == right.z;
    }

    [[nodiscard]]
    constexpr bool operator!=(const Vector3<T>& right) const
    {
        return !(*this == right);
    }

    [[nodiscard]]
    constexpr Vector3<T> operator+(const Vector3<T>& right) const
    {
        return Vector3<T>(arithmeticAdd(x, right.x), arithmeticAdd(y, right.y), arithmeticAdd(z, right.z));
    }

    [[nodiscard]]
    constexpr Vector3<T> operator-(const Vector3<T>& right) const
    {
        return Vector3<T>(arithmeticSub(x, right.x), arithmeticSub(y, right.y), arithmeticSub(z, right.z));
    }

    [[nodiscard]]
    constexpr Vector3<T> operator*(T scale) const
    {
        return Vector3<T>(arithmeticMultiply(x, scale), arithmeticMultiply(y, scale), arithmeticMultiply(z, scale));
    }

    [[nodiscard]]
    constexpr Vector3<T> operator/(T scale) const
    {
        return Vector3<T>(arithmeticDivision(x, scale), arithmeticDivision(y, scale), arithmeticDivision(z, scale));
    }

    constexpr Vector3<T>& operator+=(const Vector3<T>& right)
    {
        x = arithmeticAdd(x, right.x);
        y = arithmeticAdd(y, right.y);
        z = arithmeticAdd(z, right.z);

        return *this;
    }

    constexpr Vector3<T>& operator-=(const Vector3<T>& right)
    {
        x = arithmeticSub(x, right.x);
        y = arithmeticSub(y, right.y);
        z = arithmeticSub(z, right.z);

        return *this;
    }

    constexpr Vector3<T>& operator*=(T scale)
    {
        x = arithmeticMultiply(x, scale);
        y = arithmeticMultiply(y, scale);
        z = arithmeticMultiply(z, scale);

        return *this;
    }

    constexpr Vector3<T>& operator/=(T scale)
    {
        x = arithmeticDivision(x, scale);
        y = arithmeticDivision(y, scale);
        z = arithmeticDivision(z, scale);

        return *this;
    }

    [[nodiscard]]
    constexpr Vector3<T> operator-() const
    {
        return Vector3<T>(arithmeticNagate(x), arithmeticNagate(y), arithmeticNagate(z));
    }

    /**
     * @brief dot product
     */
    [[nodiscard]]
    constexpr T operator*(const Vector3<T>& other) const requires(Real<T>)
    {
        return dot(other);
    }

    /**
     * @brief cross product
     */
    [[nodiscard]]
    constexpr Vector3<T> operator^(const Vector3<T>& other) const requires(Real<T>)
    {
        return cross(other);
    }

    [[nodiscard]]
    constexpr T& operator[](size_t index)
    {
        // assert(index < 3);
        return data[index];
    }

    [[nodiscard]]
    constexpr const T& operator[](size_t index) const
    {
        // assert(index < 3);
        return data[index];
    }

  public:
    union
    {
        struct {
            T x, y, z;
        };

        T data[3];
    };
};

using Vec3b    = Vector3<bool>;
using Vec3i8   = Vector3<int8_t>;
using Vec3ui8  = Vector3<uint8_t>;
using Vec3i16  = Vector3<int16_t>;
using Vec3ui16 = Vector3<uint16_t>;
using Vec3i32  = Vector3<int32_t>;
using Vec3ui32 = Vector3<uint32_t>;
using Vec3i64  = Vector3<int64_t>;
using Vec3ui64 = Vector3<uint64_t>;
using Vec3i    = Vec3i32;
using Vec3ui   = Vec3ui32;
using Vec3f    = Vector3<float>;
using Vec3d    = Vector3<double>;

VI_MATH_NS_END
