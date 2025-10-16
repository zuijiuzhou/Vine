#pragma once

#include "ge_global.hpp"

#include <cstddef>

#include "Types.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

VI_GE_NS_BEGIN

template <typename T>
class Quaternion {
  public:
    using value_type = T;

  public:
    Quaternion();
    Quaternion(T x, T y, T z, T w);

  public:
    T lenght() const;
    T lenght2() const;

    Quaternion<T> conj() const;
    void          invert();
    Quaternion<T> inverted() const;

    void makeRotate(T angle, const Vector3<T>& axis);
    void makeRotate(const Vector3<T>& from, const Vector3<T>& to);

    void getRotate(T& o_angle, Vector3<T>& o_axis) const;

    Vector4<T>        toVector() const;
    const Vector4<T>& asVector() const;

    // Vector3<T> toEuler() const;
    // void       fromEuler(const Vector3<T>& euler);

  public:
    bool operator==(const Quaternion& right) const;
    bool operator!=(const Quaternion& right) const;

    Quaternion<T>  operator*(T right) const;
    Quaternion<T>& operator*=(T right);

    Quaternion<T>  operator/(T right) const;
    Quaternion<T>& operator/=(T right);

    Quaternion<T>  operator+(const Quaternion& right) const;
    Quaternion<T>& operator+=(const Quaternion& right);

    Quaternion<T>  operator-(const Quaternion& right) const;
    Quaternion<T>& operator-=(const Quaternion& right);

    Quaternion<T>  operator*(const Quaternion& right) const;
    Quaternion<T>& operator*=(const Quaternion& right);

    Quaternion<T>  operator/(const Quaternion& right) const;
    Quaternion<T>& operator/=(const Quaternion& right);

    Quaternion<T> operator-() const;

    T& operator[](size_t i);
    T  operator[](size_t i) const;

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

template<typename T>
Vector3<T> operator * (const Quaternion<T>& left, const Vector3<T>& right);

VI_GE_NS_END