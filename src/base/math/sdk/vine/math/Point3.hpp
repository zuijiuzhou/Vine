#pragma once

#include "math_global.hpp"

#include <cstddef>
#include <cstdint>

#include "Math.hpp"

VI_MATH_NS_BEGIN

template <typename T>
class Vector3;
template <typename T>
class Point2;

/**
 * @brief A class representing a point in 3D space
 * @tparam T Only accepts float double and integers
 */
template <typename T>
class Point3 {
  public:
    using value_type = T;

  public:
    constexpr Point3()
    {
        x = T();
        y = T();
        z = T();
    }

    constexpr Point3(const Point2<T>& pt2, T zz)
      : x(pt2.x)
      , y(pt2.y)
      , z(zz)
    {}

    constexpr Point3(T xx, T yy, T zz)
      : x(xx)
      , y(yy)
      , z(zz)
    {}

  public:
    [[nodiscard]]
    constexpr const Point2<T>& asPoint2() const
    {
        return reinterpret_cast<const Point2<T>&>(*this);
    }

    [[nodiscard]]
    constexpr const Vector3<T>& asVector() const
    {
        return reinterpret_cast<const Vector3<T>&>(*this);
    }

    [[nodiscard]]
    Vector3<T> toVector() const;

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
    constexpr bool isEqual(const Point3<T>& other) const
    {
        return *this == other;
    }

    [[nodiscard]]
    constexpr bool isEqual(const Point3<T>& other, T eps) const requires(Real<T>)
    {
        return math::isEqual<T>(x, other.x, eps) && math::isEqual<T>(y, other.y, eps) && math::isEqual<T>(z, other.z, eps);
    }

  public:
    [[nodiscard]]
    constexpr bool operator==(const Point3<T>& right) const
    {
        return x == right.x && y == right.y && z == right.z;
    }

    [[nodiscard]]
    constexpr bool operator!=(const Point3<T>& right) const
    {
        return !(*this == right);
    }

    [[nodiscard]]
    constexpr Vector3<T> operator-(const Point3<T>& right) const
    {
        return Vector3<T>(arithmeticSub(x, right.x), arithmeticSub(y, right.y), arithmeticSub(z, right.z));
    }

    [[nodiscard]]
    constexpr Point3<T> operator+(const Vector3<T>& right) const
    {
        return Point3<T>(arithmeticAdd(x, right.x), arithmeticAdd(y, right.y), arithmeticAdd(z, right.z));
    }

    constexpr Point3<T>& operator+=(const Vector3<T>& right)
    {
        x = arithmeticAdd(x, right.x);
        y = arithmeticAdd(y, right.y);
        z = arithmeticAdd(z, right.z);

        return *this;
    }

    constexpr Point3<T>& operator-=(const Vector3<T>& right)
    {
        x = arithmeticSub(x, right.x);
        y = arithmeticSub(y, right.y);
        z = arithmeticSub(z, right.z);

        return *this;
    }

    [[nodiscard]]
    constexpr Point3<T> operator-() const
    {
        return Point3<T>(arithmeticNagate(x), arithmeticNagate(y), arithmeticNagate(z));
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

using Point3b    = Point3<bool>;
using Point3i8   = Point3<int8_t>;
using Point3ui8  = Point3<uint8_t>;
using Point3i16  = Point3<int16_t>;
using Point3ui16 = Point3<uint16_t>;
using Point3i32  = Point3<int32_t>;
using Point3ui32 = Point3<uint32_t>;
using Point3i64  = Point3<int64_t>;
using Point3ui64 = Point3<uint64_t>;
using Point3i    = Point3i32;
using Point3ui   = Point3ui32;
using Point3f    = Point3<float>;
using Point3d    = Point3<double>;
VI_MATH_NS_END
