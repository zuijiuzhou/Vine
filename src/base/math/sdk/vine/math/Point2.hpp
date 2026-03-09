#pragma once

#include "math_global.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>

#include "Math.hpp"
#include "Types.hpp"

VI_MATH_NS_BEGIN

template <typename T>
class Vector2;

/**
 * @brief A class representing a point in 2D space
 * @tparam T Only accepts float double and integers
 */
template <typename T>
class Point2 {
  public:
    using value_type = T;

  public:
    constexpr Point2()
      : x(T())
      , y(T())
    {}

    constexpr Point2(T xx, T yy)
      : x(xx)
      , y(yy)
    {}

  public:
    [[nodiscard]]
    constexpr const Vector2<T>& asVector() const
    {
        static_assert(sizeof(Point2<T>) == sizeof(Vector2<T>));
        static_assert(std::is_standard_layout_v<Point2<T>>);
        static_assert(std::is_standard_layout_v<Vector2<T>>);
        return reinterpret_cast<const Vector2<T>&>(*this);
    }

    [[nodiscard]]
    Vector2<T> toVector() const;

    [[nodiscard]]
    constexpr TypeF<T> distanceTo(const Point2<T>& pt) const
    {
        return std::sqrt((x - pt.x) * (x - pt.x) + (y - pt.y) * (y - pt.y));
    }

    [[nodiscard]]
    constexpr bool isZero() const
    {
        return x == T() && y == T();
    }

    [[nodiscard]]
    constexpr bool isZero(T eps) const requires(Real<T>)
    {
        return math::isZero<T>(x, eps) && math::isZero<T>(y, eps);
    }

    [[nodiscard]]
    constexpr bool isEqual(const Point2<T>& other) const
    {
        return x == other.x && y == other.y;
    }

    [[nodiscard]]
    constexpr bool isEqual(const Point2<T>& other, T eps) const requires(Real<T>)
    {
        return math::isEqual<T>(x, other.x, eps) && math::isEqual<T>(y, other.y, eps);
    }

  public:
    [[nodiscard]]
    constexpr bool operator==(const Point2<T>& right) const
    {
        return x == right.x && y == right.y;
    }

    [[nodiscard]]
    constexpr bool operator!=(const Point2<T>& right) const
    {
        return !(*this == right);
    }

    [[nodiscard]]
    constexpr Vector2<T> operator-(const Point2<T>& right) const
    {
        return Vector2<T>(arithmeticSub(x, right.x), arithmeticSub(y, right.y));
    }

    [[nodiscard]]
    constexpr Point2<T> operator+(const Vector2<T>& right) const
    {
        return Point2<T>(arithmeticAdd(x, right.x), arithmeticAdd(y, right.y));
    }

    constexpr Point2<T>& operator+=(const Vector2<T>& right)
    {
        x = arithmeticAdd(x, right.x);
        y = arithmeticAdd(y, right.y);

        return *this;
    }

    constexpr Point2<T>& operator-=(const Vector2<T>& right)
    {
        x = arithmeticSub(x, right.x);
        y = arithmeticSub(y, right.y);

        return *this;
    }

    [[nodiscard]]
    constexpr Point2<T> operator-() const
    {
        return Point2<T>(arithmeticNagate(x), arithmeticNagate(y));
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

using Point2b    = Point2<bool>;
using Point2i8   = Point2<int8_t>;
using Point2ui8  = Point2<uint8_t>;
using Point2i16  = Point2<int16_t>;
using Point2ui16 = Point2<uint16_t>;
using Point2i32  = Point2<int32_t>;
using Point2ui32 = Point2<uint32_t>;
using Point2i64  = Point2<int64_t>;
using Point2ui64 = Point2<uint64_t>;
using Point2i    = Point2i32;
using Point2ui   = Point2ui32;
using Point2f    = Point2<float>;
using Point2d    = Point2<double>;

VI_MATH_NS_END
