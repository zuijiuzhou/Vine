#pragma once

#include "ge_global.h"

#include <cstdint>

VI_GE_NS_BEGIN

template <typename T> class Vector2;
/**
 * @brief
 * @tparam T Only accepts float double and integers
 */
template <typename T> class Point2 {
    // static_assert();

  public:
    using ValueType = T;

  public:
    Point2<T>();
    Point2<T>(T xx, T yy);

  public:
    const Vector2<T>& asVector() const;
    Vector2<T>        toVector() const;
    double            distanceTo(const Point2<T>& pt) const;

  public:
    bool       operator==(const Point2<T>& right) const;
    bool       operator!=(const Point2<T>& right) const;
    Vector2<T> operator-(const Point2<T>& right) const;
    Point2<T>  operator+(const Vector2<T>& right) const;
    Point2<T>& operator+=(const Vector2<T>& right);
    Point2<T>& operator-=(const Vector2<T>& right);

  public:
    T x, y;
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
VI_GE_NS_END