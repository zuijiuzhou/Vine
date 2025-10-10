#pragma once
#include "ge_global.hpp"

#include <cstdint>
#include <cstddef>

#include "Types.hpp"

VI_GE_NS_BEGIN

template <typename T> class Point3;
template <typename T> class Vector2;
/**
 * @brief
 * @tparam T Only accepts float double and integers
 */
template <typename T> class Vector3 {
  public:
    using ValueType = T;

  public:
    Vector3();
    Vector3(const Vector2<T>& vec2, T zz = 0.);
    Vector3(T xx, T yy, T zz);

  public:
    void set(T xx, T yy, T zz);
    void get(T& xx, T& yy, T& zz) const;

    Point3<T>         toPoint() const;
    const Point3<T>&  asPoint() const;
    const Vector2<T>& asVector2() const;

    bool isZero() const;
    bool isZero(T eps) const
        requires(Real<T>);
    bool isEqual(const Vector3<T>& other) const;
    bool isEqual(const Vector3<T>& other, T eps) const
        requires(Real<T>);

  public:
    bool operator==(const Vector3<T>& right) const;
    bool operator!=(const Vector3<T>& right) const;

    Vector3<T> operator+(const Vector3<T>& right) const;
    Vector3<T> operator-(const Vector3<T>& right) const;
    Vector3<T> operator*(T scale) const;
    Vector3<T> operator/(T scale) const;

    Vector3<T>& operator+=(const Vector3<T>& right);
    Vector3<T>& operator-=(const Vector3<T>& right);
    Vector3<T>& operator*=(T scale);
    Vector3<T>& operator/=(T scale);

    T&       operator[](size_t index);
    const T& operator[](size_t index) const;

  public:
    union {
        struct {
            T x, y, z;
        };
        T data[3];
    };
};

using Vec3b    = Vector3<bool>;
using Vec3i8   = Vector3<int8_t>;
using Vec3ui8  = Vector3<uint8_t>;
using Vec3i16  = Vector3<int16_t>;
using Vec3ui16 = Vector3<uint16_t>;
using Vec3i32  = Vector3<int32_t>;
using Vec3ui32 = Vector3<uint32_t>;
using Vec3i64  = Vector3<int64_t>;
using Vec3ui64 = Vector3<uint64_t>;
using Vec3i    = Vec3i32;
using Vec3ui   = Vec3ui32;
using Vec3f    = Vector3<float>;
using Vec3d    = Vector3<double>;

VI_GE_NS_END