#pragma once

#include "ge_global.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>

#include "Types.hpp"

VI_GE_NS_BEGIN
template <typename T> class Point2;
/**
 * @brief
 * @tparam T Only accepts float double and integers
 */
template <typename T> class Vector2 {

  public:
    using ValueType = T;

  public:
    Vector2();
    Vector2(T xx, T yy);

  public:
    void set(T xx, T yy);
    void get(T& xx, T& yy) const;

    Point2<T>        toPoint() const;
    const Point2<T>& asPoint() const;

    T length() const
        requires(FP<T>);
    T length2() const
        requires(FP<T>);
    T normalize()
        requires(FP<T>);

    T dot(const Vector2<T>& other)
        requires(FP<T>);
    T cross(const Vector2<T>& other)
        requires(FP<T>);

    bool isZero() const;
    bool isZero(T eps) const
        requires(FP<T>);
    bool isEqual(const Vector2<T>& other) const;
    bool isEqual(const Vector2<T>& other, T eps) const
        requires(FP<T>);

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

    T&       operator[](size_t index);
    const T& operator[](size_t index) const;

  public:
    union {
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

VI_GE_NS_END