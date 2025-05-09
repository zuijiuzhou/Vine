﻿#pragma once

#include "ge_global.h"

#include <cstdint>

VI_GE_NS_BEGIN

template <typename T> class Vector3;
template <typename T> class Point2;
/**
 * @brief
 * @tparam T Only accepts float double and integers
 */
template <typename T> class Point3 {
    // static_assert();

  public:
    using ValueType = T;

  public:
    Point3<T>();
    Point3<T>(const Point2<T>& pt2, T zz);
    Point3<T>(T xx, T yy, T zz);

  public:
    const Point2<T>&  asPoint2() const;
    const Vector3<T>& asVector() const;
    Vector3<T>        toVector() const;

  public:
    bool       operator==(const Point3<T>& right) const;
    bool       operator!=(const Point3<T>& right) const;
    Vector3<T> operator-(const Point3<T>& right) const;
    Point3<T>  operator+(const Vector3<T>& right) const;
    Point3<T>& operator+=(const Vector3<T>& right);
    Point3<T>& operator-=(const Vector3<T>& right);

  public:
    T x, y, z;
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
VI_GE_NS_END