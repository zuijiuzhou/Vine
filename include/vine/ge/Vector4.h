#pragma once

#include "ge_global.h"

#include <cstdint>

VI_GE_NS_BEGIN
template <typename T> class Vector3;

template <typename T> class Vector4 {
  public:
    using ValueType         = T;

  public:
    Vector4<T>();
    Vector4<T>(const Vector3<T>& vec3, T ww = 1.0);
    Vector4<T>(T xx, T yy, T zz, T ww);

  public:
    void set(T xx, T yy, T zz, T ww) noexcept;
    void get(T& xx, T& yy, T& zz, T& ww) const noexcept;

    bool isZero(T eps = T()) const noexcept;

    const Vector3<T>& asVector3() const noexcept;
    bool              equals(const Vector4<T>& other, T eps = T()) const;

  public:
    bool operator==(const Vector4<T>& right) const noexcept;
    bool operator!=(const Vector4<T>& right) const noexcept;

    Vector4<T>  operator-(const Vector4<T>& right) const noexcept;
    Vector4<T>  operator+(const Vector4<T>& right) const noexcept;
    Vector4<T>& operator+=(const Vector4<T>& right) noexcept;
    Vector4<T>& operator-=(const Vector4<T>& right) noexcept;
    Vector4<T>  operator*(T scale) const noexcept;
    Vector4<T>& operator*=(T scale) noexcept;
    Vector4<T>  operator/(T scale) const;
    Vector4<T>& operator/=(T scale);

    T&       operator[](size_t index);
    const T& operator[](size_t index) const;

  public:
    T x, y, z, w;
};

using Vec4i  = Vector4<int32_t>;
using Vec4ui = Vector4<uint32_t>;
using Vec4f  = Vector4<float>;
using Vec4d  = Vector4<double>;

VI_GE_NS_END