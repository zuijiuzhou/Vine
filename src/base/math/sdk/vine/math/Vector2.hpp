#pragma once

#include "math_global.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cmath>

#include "Types.hpp"

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

    constexpr Point2<T> toPoint() const
    {
        return Point2<T>(x, y);
    }

    constexpr const Point2<T>& asPoint() const
    {
        return reinterpret_cast<const Point2<T>&>(*this);
    }

    constexpr T length2() const requires(Real<T>)
    {
        return static_cast<T>(x * x + y * y);
    }

    constexpr float length2f() const requires(Real<T>)
    {
        return static_cast<float>(x) * static_cast<float>(x) + static_cast<float>(y) * static_cast<float>(y);
    }

    constexpr double length2d() const requires(Real<T>)
    {
        return static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(y) * static_cast<double>(y);
    }

    /**
     * @brief length of the vector
     *        only for real types (floating point and integers) not boolean.
     */
    T length() const requires(Real<T>)
    {
        return static_cast<T>(std::sqrt(length2()));
    }

    float lengthf() const requires(Real<T>)
    {
        return sqrtf(length2f());
    }

    double lengthd() const requires(Real<T>)
    {
        return std::sqrt(length2d());
    }

    TypeF<T> angleTo(const Vector2<T>& other) const requires(Real<T>);

    /**
     * @brief normalize the vector to unit length.
     *        only for floating point types.
     */
    T normalize() requires(FP<T>);

    /**
     * @brief dot product
     *        only for real types (floating point and integers) not boolean.
     *        for integer types, overflow is possible.
     */
    T dot(const Vector2<T>& other) const requires(Real<T>)
    {
        return static_cast<T>(x * other.x + y * other.y);
    }

    /**
     * @brief cross product (in 2D, it is a scalar)
     *        only for real types (floating point and integers) not boolean.
     *        for integer types, overflow is possible.
     */
    T cross(const Vector2<T>& other) const requires(Real<T>)
    {
        return static_cast<T>(x * other.y - y * other.x);
    }

    bool isZero() const;
    bool isZero(T eps) const requires(Real<T>);
    bool isEqual(const Vector2<T>& other) const;
    bool isEqual(const Vector2<T>& other, T eps) const requires(Real<T>);

  public:
    bool operator==(const Vector2<T>& right) const;
    bool operator!=(const Vector2<T>& right) const;

    Vector2<T> operator+(const Vector2<T>& right) const;
    Vector2<T> operator-(const Vector2<T>& right) const;
    Vector2<T> operator*(T scale) const;
    Vector2<T> operator/(T scale) const;

    Vector2<T>& operator+=(const Vector2<T>& right);
    Vector2<T>& operator-=(const Vector2<T>& right);
    Vector2<T>& operator*=(T scale);
    Vector2<T>& operator/=(T scale);

    Vector2<T> operator-() const;

    /**
     * @brief dot product
     */
    T operator*(const Vector2<T>& other) const requires(Real<T>);

    /**
     * @brief cross product
     */
    T operator^(const Vector2<T>& other) const requires(Real<T>);

    T&       operator[](size_t index);
    const T& operator[](size_t index) const;

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
