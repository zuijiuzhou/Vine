#pragma once

#include "math_global.hpp"

#include <cstddef>

#include "Math.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

VI_MATH_NS_BEGIN

/**
 * @brief A class representing a quaternion for 3D rotations
 * @tparam T Only accepts float and double
 */
template <FP T>
class Quaternion {
  public:
    using value_type = T;

  public:
    constexpr Quaternion()
      : x(T())
      , y(T())
      , z(T())
      , w(T())
    {}

    constexpr Quaternion(T x, T y, T z, T w)
      : x(x)
      , y(y)
      , z(z)
      , w(w)
    {}

    Quaternion(T angle, const Vector3<T>& axis)
    {
        makeRotate(angle, axis);
    }

    Quaternion(const Vector3<T>& from, const Vector3<T>& to)
    {
        makeRotate(from, to);
    }

  public:
    [[nodiscard]]
    constexpr Vector4<T> toVector() const
    {
        return Vector4<T>(x, y, z, w);
    }

    [[nodiscard]]
    constexpr const Vector4<T>& asVector() const
    {
        static_assert(sizeof(Quaternion<T>) == sizeof(Vector4<T>));
        static_assert(std::is_standard_layout_v<Quaternion<T>>);
        static_assert(std::is_standard_layout_v<Vector4<T>>);

        return reinterpret_cast<const Vector4<T>&>(*this);
    }

    [[nodiscard]]
    constexpr T lenght() const
    {
        return safeLength(x, y, z, w);
    }

    [[nodiscard]]
    constexpr T lenght2() const
    {
        return safeLengthSquared(x, y, z, w);
    }

    [[nodiscard]]
    constexpr Quaternion<T> conj() const
    {
        return Quaternion<T>(-x, -y, -z, w);
    }

    constexpr void invert()
    {
        // q-1 = conj / len2
        auto len2 = lenght2();
        auto rcp  = T(1) / len2;

        x = -x * rcp;
        y = -y * rcp;
        z = -z * rcp;
        w = w * rcp;
    }

    [[nodiscard]]
    constexpr Quaternion<T> inverted() const
    {
        return conj() / lenght2();
    }

    void makeRotate(T angle, const Vector3<T>& axis);
    void makeRotate(const Vector3<T>& from, const Vector3<T>& to);

    void getRotate(T& o_angle, Vector3<T>& o_axis) const;

    // Vector3<T> toEuler() const;
    // void       fromEuler(const Vector3<T>& euler);

    static Quaternion<T> slerp(const Quaternion<T>& from, const Quaternion<T>& to, T t);

  public:
    [[nodiscard]]
    constexpr bool operator==(const Quaternion& right) const
    {
        return x == right.x && y == right.y && z == right.z && w == right.w;
    }

    [[nodiscard]]
    constexpr bool operator!=(const Quaternion& right) const
    {
        return !(*this == right);
    }

    constexpr Quaternion<T> operator*(T right) const
    {
        Quaternion<T> q;
        q.x = x * right;
        q.y = y * right;
        q.z = z * right;
        q.w = w * right;
        return q;
    }

    constexpr Quaternion<T>& operator*=(T right)
    {
        x *= right;
        y *= right;
        z *= right;
        w *= right;
        return *this;
    }

    [[nodiscard]]
    constexpr Quaternion<T> operator/(T right) const
    {
        auto          rcp = T(1) / right;
        Quaternion<T> q;
        q.x = x * rcp;
        q.y = y * rcp;
        q.z = z * rcp;
        q.w = w * rcp;
        return q;
    }

    constexpr Quaternion<T>& operator/=(T right)
    {
        auto rcp = T(1) / right;
        x *= rcp;
        y *= rcp;
        z *= rcp;
        w *= rcp;
        return *this;
    }

    [[nodiscard]]
    constexpr Quaternion<T> operator+(const Quaternion& right) const
    {
        Quaternion<T> q;
        q.x = x + right.x;
        q.y = y + right.y;
        q.z = z + right.z;
        q.w = w + right.w;
        return q;
    }

    constexpr Quaternion<T>& operator+=(const Quaternion& right)
    {
        x += right.x;
        y += right.y;
        z += right.z;
        w += right.w;
        return *this;
    }

    [[nodiscard]]
    constexpr Quaternion<T> operator-(const Quaternion& right) const
    {
        Quaternion<T> q;
        q.x = x - right.x;
        q.y = y - right.y;
        q.z = z - right.z;
        q.w = w - right.w;
        return q;
    }

    constexpr Quaternion<T>& operator-=(const Quaternion& right)
    {
        x -= right.x;
        y -= right.y;
        z -= right.z;
        w -= right.w;
        return *this;
    }

    [[nodiscard]]
    Quaternion<T>  operator*(const Quaternion& right) const;
    Quaternion<T>& operator*=(const Quaternion& right);

    [[nodiscard]]
    Quaternion<T>  operator/(const Quaternion& right) const;
    Quaternion<T>& operator/=(const Quaternion& right);

    [[nodiscard]]
    constexpr Quaternion<T> operator-() const
    {
        return Quaternion<T>(-x, -y, -z, -w);
    }

    [[nodiscard]]
    constexpr T& operator[](size_t i)
    {
        return data[i];
    }

    [[nodiscard]]
    constexpr T operator[](size_t i) const
    {
        return data[i];
    }

  public:
    union
    {
        struct {
            T x, y, z, w;
        };

        T data[4];
    };
};

using Quatf = Quaternion<float>;
using Quatd = Quaternion<double>;

template <typename T>
Vector3<T> operator*(const Quaternion<T>& left, const Vector3<T>& right);

VI_MATH_NS_END
