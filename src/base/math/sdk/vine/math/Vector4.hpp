#pragma once

#include "math_global.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include "Math.hpp"

V_MATH_NS_BEGIN
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
    /**
     * @brief Construct a zero vector.
     */
    constexpr Vector4()
      : x(T())
      , y(T())
      , z(T())
      , w(T())
    {}

    /**
     * @brief Construct a 4D vector from a 3D vector and w component.
     * @param vec3 Source 3D vector.
     * @param ww W component.
     */
    constexpr Vector4(const Vector3<T>& vec3, T ww = 1.0)
      : x(vec3.x)
      , y(vec3.y)
      , z(vec3.z)
      , w(ww)
    {}

    /**
     * @brief Construct a vector from components.
     * @param xx X component.
     * @param yy Y component.
     * @param zz Z component.
     * @param ww W component.
     */
    constexpr Vector4(T xx, T yy, T zz, T ww)
      : x(xx)
      , y(yy)
      , z(zz)
      , w(ww)
    {}

  public:
    /**
     * @brief Set x/y/z from a 3D vector.
     * @param vec3 Source 3D vector.
     */
    constexpr void set(const Vector3<T>& vec3)
    {
        x = vec3.x;
        y = vec3.y;
        z = vec3.z;
    }

    /**
     * @brief Set x/y/z from a 3D vector and w explicitly.
     * @param vec3 Source 3D vector.
     * @param ww W component.
     */
    constexpr void set(const Vector3<T>& vec3, T ww)
    {
        x = vec3.x;
        y = vec3.y;
        z = vec3.z;
        w = ww;
    }

    /**
     * @brief Set all components.
     * @param xx X component.
     * @param yy Y component.
     * @param zz Z component.
     * @param ww W component.
     */
    constexpr void set(T xx, T yy, T zz, T ww)
    {
        x = xx;
        y = yy;
        z = zz;
        w = ww;
    }

    /**
     * @brief Output all components.
     * @param xx Output X component.
     * @param yy Output Y component.
     * @param zz Output Z component.
     * @param ww Output W component.
     */
    constexpr void get(T& xx, T& yy, T& zz, T& ww) const
    {
        xx = x;
        yy = y;
        zz = z;
        ww = w;
    }

    /**
     * @brief View this vector as a 3D vector without copying.
     * @return Const reference to x/y/z part.
     */
    [[nodiscard]]
    constexpr const Vector3<T>& asVector3() const
    {
        return reinterpret_cast<const Vector3<T>&>(*this);
    }

    /**
     * @brief Dot product.
     *        only for real types (floating point and integers) not boolean.
     *        for integer types, overflow is possible.
     * @param other Right-hand vector.
     * @return Dot product value.
     */

    [[nodiscard]]
    constexpr T dot(const Vector4<T>& other) const requires(Real<T>)
    {
        return static_cast<T>(x * other.x + y * other.y + z * other.z + w * other.w);
    }

    /**
     * @brief Compute squared vector length.
     * @return Squared length value.
     */
    [[nodiscard]]
    constexpr TypeF<T> length2() const requires(Real<T>)
    {
        return safeLengthSquared(x, y, z, w);
    }

    /**
     * @brief Length of the vector.
     *        only for real types (floating point and integers) not boolean.
     * @return Vector length.
     */
    [[nodiscard]]
    constexpr TypeF<T> length() const requires(Real<T>)
    {
        return safeLength(x, y, z, w);
    }

    /**
     * @brief Compute angle to another vector.
     * @param other Right-hand vector.
     * @return Angle in radians.
     */
    [[nodiscard]]
    constexpr TypeF<T> angleTo(const Vector4<T>& other) const requires(Real<T>)
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
         * This approach is numerically stable compared to acos(dot/(|a||b|))
         * which suffers from precision loss when vectors are nearly parallel.
         */

        using ft = TypeF<T>;

        const ft x1 = static_cast<ft>(x);
        const ft y1 = static_cast<ft>(y);
        const ft z1 = static_cast<ft>(z);
        const ft w1 = static_cast<ft>(w);

        const ft x2 = static_cast<ft>(other.x);
        const ft y2 = static_cast<ft>(other.y);
        const ft z2 = static_cast<ft>(other.z);
        const ft w2 = static_cast<ft>(other.w);

        const ft dot     = x1 * x2 + y1 * y2 + z1 * z2 + w1 * w2;
        const ft len1_sq = x1 * x1 + y1 * y1 + z1 * z1 + w1 * w1;
        const ft len2_sq = x2 * x2 + y2 * y2 + z2 * z2 + w2 * w2;

        // Calculate sin²θ using |a|²|b|² - (a·b)², then take square root to get sinθ (non-negative)
        // This avoids precision issues with acos when dot product approaches ±1
        const ft sin_theta_sq = len1_sq * len2_sq - dot * dot;
        const ft sin_theta    = std::sqrt((std::max)(ft(0), sin_theta_sq));

        // Result range [0, π]:
        //   dot > 0 (same direction)     → angle in [0, π/2)
        //   dot = 0 (orthogonal)         → angle = π/2
        //   dot < 0 (opposite direction) → angle in (π/2, π]
        return std::atan2(sin_theta, dot);
    }

    /**
     * @brief Normalize the vector to unit length.
     *        only for floating point types.
     * @return Original vector length before normalization.
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

    /**
     * @brief Check whether all components are zero.
     * @return True when x/y/z/w are exactly zero.
     */
    [[nodiscard]]
    constexpr bool isZero() const
    {
        return x == T() && y == T() && z == T() && w == T();
    }

    /**
     * @brief Check whether all components are near zero.
     * @param eps Tolerance used for comparison.
     * @return True when all components are within tolerance of zero.
     */
    [[nodiscard]]
    constexpr bool isZero(T eps) const requires(Real<T>)
    {
        return math::isZero<T>(x, eps) && math::isZero<T>(y, eps) && math::isZero<T>(z, eps) && math::isZero<T>(w, eps);
    }

    /**
     * @brief Compare with another vector using exact equality.
     * @param other Vector to compare.
     * @return True when components are equal.
     */
    [[nodiscard]]
    constexpr bool isEqual(const Vector4<T>& other) const
    {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    /**
     * @brief Compare with another vector using tolerance.
     * @param other Vector to compare.
     * @param eps Tolerance used for comparison.
     * @return True when components are equal within tolerance.
     */
    [[nodiscard]]
    constexpr bool isEqual(const Vector4<T>& other, T eps) const requires(Real<T>)
    {
        return math::isEqual<T>(x, other.x, eps) && math::isEqual<T>(y, other.y, eps) && math::isEqual<T>(z, other.z, eps) && math::isEqual<T>(w, other.w, eps);
    }

  public:
    /**
     * @brief Equality operator.
     * @param right Right-hand vector.
     * @return True when vectors are equal.
     */
    [[nodiscard]]
    constexpr bool operator==(const Vector4<T>& right) const
    {
        return x == right.x && y == right.y && z == right.z && w == right.w;
    }

    /**
     * @brief Inequality operator.
     * @param right Right-hand vector.
     * @return True when vectors are not equal.
     */
    [[nodiscard]]
    constexpr bool operator!=(const Vector4<T>& right) const
    {
        return !(*this == right);
    }

    /**
     * @brief Vector addition.
     * @param right Right-hand vector.
     * @return Sum vector.
     */
    [[nodiscard]]
    constexpr Vector4<T> operator+(const Vector4<T>& right) const
    {
        return Vector4<T>(arithmeticAdd(x, right.x), arithmeticAdd(y, right.y), arithmeticAdd(z, right.z), arithmeticAdd(w, right.w));
    }

    /**
     * @brief Vector subtraction.
     * @param right Right-hand vector.
     * @return Difference vector.
     */
    [[nodiscard]]
    constexpr Vector4<T> operator-(const Vector4<T>& right) const
    {
        return Vector4<T>(arithmeticSub(x, right.x), arithmeticSub(y, right.y), arithmeticSub(z, right.z), arithmeticSub(w, right.w));
    }

    /**
     * @brief Scale this vector.
     * @param scale Scalar multiplier.
     * @return Scaled vector.
     */
    [[nodiscard]]
    constexpr Vector4<T> operator*(T scale) const
    {
        return Vector4<T>(arithmeticMultiply(x, scale), arithmeticMultiply(y, scale), arithmeticMultiply(z, scale), arithmeticMultiply(w, scale));
    }

    /**
     * @brief Divide this vector by a scalar.
     * @param scale Scalar divisor.
     * @return Scaled vector.
     */
    [[nodiscard]]
    constexpr Vector4<T> operator/(T scale) const
    {
        return Vector4<T>(arithmeticDivision(x, scale), arithmeticDivision(y, scale), arithmeticDivision(z, scale), arithmeticDivision(w, scale));
    }

    /**
     * @brief Add another vector in-place.
     * @param right Right-hand vector.
     * @return Reference to this vector.
     */
    constexpr Vector4<T>& operator+=(const Vector4<T>& right)
    {
        x = arithmeticAdd(x, right.x);
        y = arithmeticAdd(y, right.y);
        z = arithmeticAdd(z, right.z);
        w = arithmeticAdd(w, right.w);

        return *this;
    }

    /**
     * @brief Subtract another vector in-place.
     * @param right Right-hand vector.
     * @return Reference to this vector.
     */
    constexpr Vector4<T>& operator-=(const Vector4<T>& right)
    {
        x = arithmeticSub(x, right.x);
        y = arithmeticSub(y, right.y);
        z = arithmeticSub(z, right.z);
        w = arithmeticSub(w, right.w);

        return *this;
    }

    /**
     * @brief Multiply by scalar in-place.
     * @param scale Scalar multiplier.
     * @return Reference to this vector.
     */
    constexpr Vector4<T>& operator*=(T scale)
    {
        x = arithmeticMultiply(x, scale);
        y = arithmeticMultiply(y, scale);
        z = arithmeticMultiply(z, scale);
        w = arithmeticMultiply(w, scale);

        return *this;
    }

    /**
     * @brief Divide by scalar in-place.
     * @param scale Scalar divisor.
     * @return Reference to this vector.
     */
    constexpr Vector4<T>& operator/=(T scale)
    {
        x = arithmeticDivision(x, scale);
        y = arithmeticDivision(y, scale);
        z = arithmeticDivision(z, scale);
        w = arithmeticDivision(w, scale);

        return *this;
    }

    /**
     * @brief Unary negation.
     * @return Vector with negated components.
     */
    [[nodiscard]]
    constexpr Vector4<T> operator-() const
    {
        return Vector4<T>(arithmeticNagate(x), arithmeticNagate(y), arithmeticNagate(z), arithmeticNagate(w));
    }

    /**
     * @brief Dot product.
     * @param other Right-hand vector.
     * @return Dot product value.
     */
    [[nodiscard]]
    constexpr T operator*(const Vector4<T>& other) const requires(Real<T>)
    {
        return dot(other);
    }

    /**
     * @brief Access component by index.
     * @param index Component index in [0, 3].
     * @return Mutable component reference.
     */
    [[nodiscard]]
    constexpr T& operator[](size_t index)
    {
        // assert(index < 4);
        return data[index];
    }

    /**
     * @brief Access component by index.
     * @param index Component index in [0, 3].
     * @return Const component reference.
     */
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

V_MATH_NS_END
