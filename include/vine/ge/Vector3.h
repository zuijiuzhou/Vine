#pragma once
#include "ge_global.h"

#include <cstdint>

#include "Ge.h"

VI_GE_NS_BEGIN

template <typename T> class Point3;
template <typename T> class Vector2;
template <typename T> class Vector3 {
  public:
    using ValueType         = T;

  public:
    Vector3<T>();
    Vector3<T>(const Vector2<T>& vec2, T zz = 0.);
    Vector3<T>(T xx, T yy, T zz);

  public:
    void set(T xx, T yy, T zz) noexcept;
    void get(T& xx, T& yy, T& zz) const noexcept;

    // T    angleTo(const Vector3<T>& vec) const noexcept;
    // T    angleTo(const Vector3<T>& vec, const Vector3<T>& refVec) const noexcept;

    bool isZero(T eps = T()) const noexcept;

    Point3<T>         toPoint() const noexcept;
    const Point3<T>&  asPoint() const noexcept;
    const Vector2<T>& asVector2() const noexcept;
    bool              equals(const Vector3<T>& other, T eps = T()) const;

  public:
    bool operator==(const Vector3<T>& right) const noexcept;
    bool operator!=(const Vector3<T>& right) const noexcept;

    Vector3<T>  operator-(const Vector3<T>& right) const noexcept;
    Vector3<T>  operator+(const Vector3<T>& right) const noexcept;
    Vector3<T>& operator+=(const Vector3<T>& right) noexcept;
    Vector3<T>& operator-=(const Vector3<T>& right) noexcept;
    Vector3<T>  operator*(T scale) const noexcept;
    Vector3<T>& operator*=(T scale) noexcept;
    Vector3<T>  operator/(T scale) const;
    Vector3<T>& operator/=(T scale);

    T&       operator[](size_t index);
    const T& operator[](size_t index) const;

  public:
    T x;
    T y;
    T z;
};

using Vec3i  = Vector3<int32_t>;
using Vec3ui = Vector3<uint32_t>;
using Vec3f  = Vector3<float>;
using Vec3d  = Vector3<double>;

VI_GE_NS_END