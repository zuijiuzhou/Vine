#pragma once
#include "math_global.hpp"

#include <cstddef>
#include <cstdint>

#include "Types.hpp"

VI_MATH_NS_BEGIN

template <typename T>
class Point3;
template <typename T>
class Vector2;

/**
 * @brief
 * @tparam T Only accepts float double and integers
 */
template <typename T>
class Vector3 {
  public:
    using value_type = T;

  public:
    Vector3();
    Vector3(const Vector2<T>& vec2, T zz = 0.);
    Vector3(T xx, T yy, T zz);

  public:
    void set(const Vector2<T>& vec2);
    void set(const Vector2<T>& vec2, T zz);
    void set(T xx, T yy, T zz);
    void get(T& xx, T& yy, T& zz) const;

    Point3<T>         toPoint() const;
    const Point3<T>&  asPoint() const;
    const Vector2<T>& asVector2() const;

    /**
     * @brief length of the vector
     *        only for real types (floating point and integers) not boolean.
     */
    TypeF<T> length() const requires(Real<T>);
    TypeF<T> length2() const requires(Real<T>);
    TypeF<T> angleTo(const Vector3<T>& other) const requires(Real<T>);

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
    T dot(const Vector3<T>& other) const requires(Real<T>);

    /**
     * @brief cross product (in 2D, it is a scalar)
     *        only for real types (floating point and integers) not boolean.
     *        for integer types, overflow is possible.
     */
    Vector3<T> cross(const Vector3<T>& other) const requires(Real<T>);

    bool isZero() const;
    bool isZero(T eps) const requires(Real<T>);
    bool isEqual(const Vector3<T>& other) const;
    bool isEqual(const Vector3<T>& other, T eps) const requires(Real<T>);

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

    Vector3<T> operator-() const;

    /**
     * @brief dot product
     */
    T operator*(const Vector3<T>& other) const requires(Real<T>);

    /**
     * @brief cross product
     */
    Vector3<T> operator^(const Vector3<T>& other) const requires(Real<T>);

    T&       operator[](size_t index);
    const T& operator[](size_t index) const;

  public:
    union
    {
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

VI_MATH_NS_END