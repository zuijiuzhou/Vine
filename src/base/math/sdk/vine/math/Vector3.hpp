#pragma once
#include "math_global.hpp"

#include <algorithm>
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
        /**
         * @brief Construct a zero vector.
         */
    constexpr Vector3()
      : x(T())
      , y(T())
      , z(T())
    {}

        /**
         * @brief Construct a 3D vector from a 2D vector and z component.
         * @param vec2 Source 2D vector.
         * @param zz Z component.
         */
    constexpr Vector3(const Vector2<T>& vec2, T zz = 0.)
      : x(vec2.x)
      , y(vec2.y)
      , z(zz)
    {}

        /**
         * @brief Construct a vector from components.
         * @param xx X component.
         * @param yy Y component.
         * @param zz Z component.
         */
    constexpr Vector3(T xx, T yy, T zz)
      : x(xx)
      , y(yy)
      , z(zz) {};

  public:
        /**
         * @brief Set x and y from a 2D vector.
         * @param vec2 Source 2D vector.
         */
    constexpr void set(const Vector2<T>& vec2)
    {
        x = vec2.x;
        y = vec2.y;
    }

    /**
     * @brief Set x/y from a 2D vector and z explicitly.
     * @param vec2 Source 2D vector.
     * @param zz Z component.
     */
    constexpr void set(const Vector2<T>& vec2, T zz)
    {
        x = vec2.x;
        y = vec2.y;
        z = zz;
    }

    /**
     * @brief Set all components.
     * @param xx X component.
     * @param yy Y component.
     * @param zz Z component.
     */
    constexpr void set(T xx, T yy, T zz)
    {
        x = xx;
        y = yy;
        z = zz;
    }

    /**
     * @brief Output all components.
     * @param xx Output X component.
     * @param yy Output Y component.
     * @param zz Output Z component.
     */
    constexpr void get(T& xx, T& yy, T& zz) const
    {
        xx = x;
        yy = y;
        zz = z;
    }

    /**
     * @brief Convert this vector to a 3D point.
     * @return Converted point.
     */
    [[nodiscard]]
    Point3<T> toPoint() const;

    /**
     * @brief View this vector as a point without copying.
     * @return Const reference to the same memory as a point.
     */
    [[nodiscard]]
    constexpr const Point3<T>& asPoint() const
    {
        return reinterpret_cast<const Point3<T>&>(*this);
    }

    /**
     * @brief View this vector as a 2D vector without copying.
     * @return Const reference to x/y part.
     */
    [[nodiscard]]
    constexpr const Vector2<T>& asVector2() const
    {
        return reinterpret_cast<const Vector2<T>&>(*this);
    }

    /**
     * @brief dot product
     *        only for real types (floating point and integers) not boolean.
     *        for integer types, overflow is possible.
        * @param other Right-hand vector.
        * @return Dot product value.
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
        * @param other Right-hand vector.
        * @return Cross product vector.
     */
    [[nodiscard]]
    constexpr Vector3<T> cross(const Vector3<T>& other) const requires(Real<T>)
    {
        return Vector3<T>(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }

    /**
     * @brief length of the vector
     *        only for real types (floating point and integers) not boolean.
        * @return Vector length.
     */
    [[nodiscard]]
    constexpr TypeF<T> length() const requires(Real<T>)
    {
        return safeLength(x, y, z);
    }

    /**
     * @brief Compute squared vector length.
     * @return Squared length value.
     */
    [[nodiscard]]
    constexpr TypeF<T> length2() const requires(Real<T>)
    {
        return safeLengthSquared(x, y, z);
    }

    /**
     * @brief Compute angle to another vector.
     * @param other Right-hand vector.
     * @return Angle in radians.
     */
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
        const ft sin_theta    = std::sqrt((std::max)(ft(0), sin_theta_sq));

        return std::atan2(sin_theta, dot);
    }

    /**
     * @brief normalize the vector to unit length.
     *        only for floating point types.
        * @return Original vector length before normalization.
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

    /**
     * @brief Check whether all components are zero.
     * @return True when x, y and z are exactly zero.
     */
    [[nodiscard]]
    constexpr bool isZero() const
    {
        return x == T() && y == T() && z == T();
    }

    /**
     * @brief Check whether all components are near zero.
     * @param eps Tolerance used for comparison.
     * @return True when all components are within tolerance of zero.
     */
    [[nodiscard]]
    constexpr bool isZero(T eps) const requires(Real<T>)
    {
        return math::isZero<T>(x, eps) && math::isZero<T>(y, eps) && math::isZero<T>(z, eps);
    }

    /**
     * @brief Compare with another vector using exact equality.
     * @param other Vector to compare.
     * @return True when components are equal.
     */
    [[nodiscard]]
    constexpr bool isEqual(const Vector3<T>& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    /**
     * @brief Compare with another vector using tolerance.
     * @param other Vector to compare.
     * @param eps Tolerance used for comparison.
     * @return True when components are equal within tolerance.
     */
    [[nodiscard]]
    constexpr bool isEqual(const Vector3<T>& other, T eps) const requires(Real<T>)
    {
        return math::isEqual<T>(x, other.x, eps) && math::isEqual<T>(y, other.y, eps) && math::isEqual<T>(z, other.z, eps);
    }

  public:
        /**
         * @brief Equality operator.
         * @param right Right-hand vector.
         * @return True when vectors are equal.
         */
    [[nodiscard]]
    constexpr bool operator==(const Vector3<T>& right) const
    {
        return x == right.x && y == right.y && z == right.z;
    }

    /**
     * @brief Inequality operator.
     * @param right Right-hand vector.
     * @return True when vectors are not equal.
     */
    [[nodiscard]]
    constexpr bool operator!=(const Vector3<T>& right) const
    {
        return !(*this == right);
    }

    /**
     * @brief Vector addition.
     * @param right Right-hand vector.
     * @return Sum vector.
     */
    [[nodiscard]]
    constexpr Vector3<T> operator+(const Vector3<T>& right) const
    {
        return Vector3<T>(arithmeticAdd(x, right.x), arithmeticAdd(y, right.y), arithmeticAdd(z, right.z));
    }

    /**
     * @brief Vector subtraction.
     * @param right Right-hand vector.
     * @return Difference vector.
     */
    [[nodiscard]]
    constexpr Vector3<T> operator-(const Vector3<T>& right) const
    {
        return Vector3<T>(arithmeticSub(x, right.x), arithmeticSub(y, right.y), arithmeticSub(z, right.z));
    }

    /**
     * @brief Scale this vector.
     * @param scale Scalar multiplier.
     * @return Scaled vector.
     */
    [[nodiscard]]
    constexpr Vector3<T> operator*(T scale) const
    {
        return Vector3<T>(arithmeticMultiply(x, scale), arithmeticMultiply(y, scale), arithmeticMultiply(z, scale));
    }

    /**
     * @brief Divide this vector by a scalar.
     * @param scale Scalar divisor.
     * @return Scaled vector.
     */
    [[nodiscard]]
    constexpr Vector3<T> operator/(T scale) const
    {
        return Vector3<T>(arithmeticDivision(x, scale), arithmeticDivision(y, scale), arithmeticDivision(z, scale));
    }

    /**
     * @brief Add another vector in-place.
     * @param right Right-hand vector.
     * @return Reference to this vector.
     */
    constexpr Vector3<T>& operator+=(const Vector3<T>& right)
    {
        x = arithmeticAdd(x, right.x);
        y = arithmeticAdd(y, right.y);
        z = arithmeticAdd(z, right.z);

        return *this;
    }

    /**
     * @brief Subtract another vector in-place.
     * @param right Right-hand vector.
     * @return Reference to this vector.
     */
    constexpr Vector3<T>& operator-=(const Vector3<T>& right)
    {
        x = arithmeticSub(x, right.x);
        y = arithmeticSub(y, right.y);
        z = arithmeticSub(z, right.z);

        return *this;
    }

    /**
     * @brief Multiply by scalar in-place.
     * @param scale Scalar multiplier.
     * @return Reference to this vector.
     */
    constexpr Vector3<T>& operator*=(T scale)
    {
        x = arithmeticMultiply(x, scale);
        y = arithmeticMultiply(y, scale);
        z = arithmeticMultiply(z, scale);

        return *this;
    }

    /**
     * @brief Divide by scalar in-place.
     * @param scale Scalar divisor.
     * @return Reference to this vector.
     */
    constexpr Vector3<T>& operator/=(T scale)
    {
        x = arithmeticDivision(x, scale);
        y = arithmeticDivision(y, scale);
        z = arithmeticDivision(z, scale);

        return *this;
    }

    /**
     * @brief Unary negation.
     * @return Vector with negated components.
     */
    [[nodiscard]]
    constexpr Vector3<T> operator-() const
    {
        return Vector3<T>(arithmeticNagate(x), arithmeticNagate(y), arithmeticNagate(z));
    }

    /**
     * @brief dot product
        * @param other Right-hand vector.
        * @return Dot product value.
     */
    [[nodiscard]]
    constexpr T operator*(const Vector3<T>& other) const requires(Real<T>)
    {
        return dot(other);
    }

    /**
     * @brief cross product
        * @param other Right-hand vector.
        * @return Cross product vector.
     */
    [[nodiscard]]
    constexpr Vector3<T> operator^(const Vector3<T>& other) const requires(Real<T>)
    {
        return cross(other);
    }

    /**
     * @brief Access component by index.
     * @param index Component index (0 for x, 1 for y, 2 for z).
     * @return Mutable component reference.
     */
    [[nodiscard]]
    constexpr T& operator[](size_t index)
    {
        // assert(index < 3);
        return data[index];
    }

    /**
     * @brief Access component by index.
     * @param index Component index (0 for x, 1 for y, 2 for z).
     * @return Const component reference.
     */
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
