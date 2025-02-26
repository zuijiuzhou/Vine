#pragma once

#include "ge_global.h"

#include <cstdint>

VI_GE_NS_BEGIN
template <typename T> class Vector3;
/**
 * @brief
 * @tparam T Only accepts float double and integers
 */
template <typename T> class Vector4 {
  public:
    using ValueType = T;

  public:
    Vector4<T>();
    Vector4<T>(const Vector3<T>& vec3, T ww = 1.0);
    Vector4<T>(T xx, T yy, T zz, T ww);

  public:
    void set(T xx, T yy, T zz, T ww);
    void get(T& xx, T& yy, T& zz, T& ww) const;

    const Vector3<T>& asVector3() const;

    bool isZero() const;

  public:
    bool operator==(const Vector4<T>& right) const;
    bool operator!=(const Vector4<T>& right) const;

    Vector4<T> operator+(const Vector4<T>& right) const;
    Vector4<T> operator-(const Vector4<T>& right) const;
    Vector4<T> operator*(T scale) const;
    Vector4<T> operator/(T scale) const;

    Vector4<T>& operator+=(const Vector4<T>& right);
    Vector4<T>& operator-=(const Vector4<T>& right);
    Vector4<T>& operator*=(T scale);
    Vector4<T>& operator/=(T scale);

    T&       operator[](size_t index);
    const T& operator[](size_t index) const;

  public:
    T x, y, z, w;
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

VI_GE_NS_END