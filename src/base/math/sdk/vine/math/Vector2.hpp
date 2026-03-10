#pragma once

#include "math_global.hpp"

#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>

#include "Math.hpp"

VI_MATH_NS_BEGIN
template <typename T>
class Point2;

/**
 * @brief A class representing a vector in 2D space
 * @tparam T Only accepts float double and integers(include boolean)
 */
template <typename T>
class Vector2 {
  public:
    using value_type = T;

  public:
    constexpr Vector2()
      : x(T())
      , y(T())
    {}

    constexpr Vector2(T xx, T yy)
      : x(xx)
      , y(yy)
    {}

  public:
    void set(T xx, T yy)
    {
        x = xx;
        y = yy;
    }

    void get(T& xx, T& yy) const
    {
        xx = x;
        yy = y;
    }

    [[nodiscard]]
    Point2<T> toPoint() const;

    [[nodiscard]]
    constexpr const Point2<T>& asPoint() const
    {
        return reinterpret_cast<const Point2<T>&>(*this);
    }

    /**
     * @brief dot product
     *        only for real types (floating point and integers) not boolean.
     *        for integer types, overflow is possible.
     */
    [[nodiscard]]
    T dot(const Vector2<T>& other) const requires(Real<T>)
    {
        return static_cast<T>(x * other.x + y * other.y);
    }

    /**
     * @brief cross product (in 2D, it is a scalar)
     *        only for real types (floating point and integers) not boolean.
     *        for integer types, overflow is possible.
     */
    [[nodiscard]]
    T cross(const Vector2<T>& other) const requires(Real<T>)
    {
        return static_cast<T>(x * other.y - y * other.x);
    }

    /**
     * @brief length squared of the vector
     *        only for real types (floating point and integers) not boolean.
     *        for integer types, overflow is possible.
     */
    [[nodiscard]]
    constexpr TypeF<T> length2() const requires(Real<T>)
    {
        return safeLengthSquared(x, y);
    }

    /**
     * @brief length of the vector
     *        only for real types (floating point and integers) not boolean.
     */
    [[nodiscard]]
    constexpr TypeF<T> length() const requires(Real<T>)
    {
        return safeLength(x, y);
    }

    /**
     * @brief calculate the angle between this vector and another vector
     * @param other the other vector
     * @return the angle in radians
     *         only for real types (floating point and integers) not boolean.
     */
    [[nodiscard]]
    constexpr TypeF<T> angleTo(const Vector2<T>& other) const requires(Real<T>)
    {
        /**
         * Algorithm: θ = atan2(|a × b|, a·b)
         *
         * In 2D, the cross product is a scalar: a × b = x₁y₂ - y₁x₂
         * The magnitude |a × b| = |a||b|sinθ
         *
         * Therefore:
         *   θ = atan2(|a||b|sinθ, |a||b|cosθ)
         *     = atan2(|a × b|, a·b)
         *
         * Result range [0, π]:
         *   dot > 0 (same direction)     → angle in [0, π/2)
         *   dot = 0 (orthogonal)         → angle = π/2
         *   dot < 0 (opposite direction) → angle in (π/2, π]
         *
         * This approach is numerically stable compared to acos(dot/(|a||b|))
         * which suffers from precision loss when vectors are nearly parallel.
         */

        using ft   = TypeF<T>;
        auto dot   = static_cast<ft>(x) * static_cast<ft>(other.x) + static_cast<ft>(y) * static_cast<ft>(other.y);
        auto cross = static_cast<ft>(x) * static_cast<ft>(other.y) - static_cast<ft>(y) * static_cast<ft>(other.x);

        return std::atan2(std::abs(cross), dot);
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
        }
        else {
            x /= len;
            y /= len;
        }
        return len;
    }

    [[nodiscard]]
    constexpr bool isZero() const
    {
        return x == T(0) && y == T(0);
    }

    [[nodiscard]]
    constexpr bool isZero(T eps) const requires(Real<T>)
    {
        return math::isZero<T>(x, eps) && math::isZero<T>(y, eps);
    }

    [[nodiscard]]
    constexpr bool isEqual(const Vector2<T>& other) const
    {
        return x == other.x && y == other.y;
    }

    [[nodiscard]]
    constexpr bool isEqual(const Vector2<T>& other, T eps) const requires(Real<T>)
    {
        return math::isEqual<T>(x, other.x, eps) && math::isEqual<T>(y, other.y, eps);
    }

  public:
    [[nodiscard]]
    constexpr bool operator==(const Vector2<T>& right) const
    {
        return x == right.x && y == right.y;
    }

    [[nodiscard]]
    constexpr bool operator!=(const Vector2<T>& right) const
    {
        return x != right.x || y != right.y;
    }

    [[nodiscard]]
    constexpr Vector2<T> operator+(const Vector2<T>& right) const
    {
        return Vector2<T>(arithmeticAdd(x, right.x), arithmeticAdd(y, right.y));
    }

    [[nodiscard]]
    constexpr Vector2<T> operator-(const Vector2<T>& right) const
    {
        return Vector2<T>(arithmeticSub(x, right.x), arithmeticSub(y, right.y));
    }

    [[nodiscard]]
    constexpr Vector2<T> operator*(T scale) const
    {
        Vector2<T> v;

        v.x = arithmeticMultiply(x, scale);
        v.y = arithmeticMultiply(y, scale);

        return v;
    }

    [[nodiscard]]
    constexpr Vector2<T> operator/(T scale) const
    {
        Vector2<T> v;

        v.x = arithmeticDivision(x, scale);
        v.y = arithmeticDivision(y, scale);

        return v;
    }

    constexpr Vector2<T>& operator+=(const Vector2<T>& right)
    {
        x = arithmeticAdd(x, right.x);
        y = arithmeticAdd(y, right.y);

        return *this;
    }

    constexpr Vector2<T>& operator-=(const Vector2<T>& right)
    {
        x = arithmeticSub(x, right.x);
        y = arithmeticSub(y, right.y);

        return *this;
    }

    constexpr Vector2<T>& operator*=(T scale)
    {
        x = arithmeticMultiply(x, scale);
        y = arithmeticMultiply(y, scale);

        return *this;
    }

    Vector2<T>& operator/=(T scale)
    {
        x = arithmeticDivision(x, scale);
        y = arithmeticDivision(y, scale);

        return *this;
    }

    [[nodiscard]]
    constexpr Vector2<T> operator-() const
    {
        return Vector2<T>(arithmeticNagate(x), arithmeticNagate(y));
    }

    /**
     * @brief dot product
     */
    [[nodiscard]]
    constexpr T operator*(const Vector2<T>& other) const requires(Real<T>)
    {
        return dot(other);
    }

    /**
     * @brief cross product
     */
    [[nodiscard]]
    constexpr T operator^(const Vector2<T>& other) const requires(Real<T>)
    {
        return cross(other);
    }

    [[nodiscard]]
    constexpr T& operator[](size_t index)
    {
        // assert(index < 2);
        return data[index];
    }

    [[nodiscard]]
    constexpr const T& operator[](size_t index) const
    {
        // assert(index < 2);
        return data[index];
    }

  public:
    union
    {
        struct {
            T x, y;
        };

        T data[2];
    };
};

using Vec2b    = Vector2<bool>;
using Vec2i8   = Vector2<int8_t>;
using Vec2ui8  = Vector2<uint8_t>;
using Vec2i16  = Vector2<int16_t>;
using Vec2ui16 = Vector2<uint16_t>;
using Vec2i32  = Vector2<int32_t>;
using Vec2ui32 = Vector2<uint32_t>;
using Vec2i64  = Vector2<int64_t>;
using Vec2ui64 = Vector2<uint64_t>;
using Vec2i    = Vec2i32;
using Vec2ui   = Vec2ui32;
using Vec2f    = Vector2<float>;
using Vec2d    = Vector2<double>;

VI_MATH_NS_END
