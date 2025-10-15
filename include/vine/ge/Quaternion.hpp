#pragma once

#include "ge_global.hpp"

#include <cstddef>

VI_GE_NS_BEGIN

template <typename T>
class Quaternion {
  public:
    Quaternion();
    Quaternion(T x, T y, T z, T w);

  public:
    bool operator==(const Quaternion& right) const;
    bool operator!=(const Quaternion& right) const;

    Quaternion<T>  operator+(const Quaternion& right) const;
    Quaternion<T>& operator+=(const Quaternion& right);

    Quaternion<T>  operator-(const Quaternion& right) const;
    Quaternion<T>& operator-=(const Quaternion& right);

    Quaternion<T>  operator*(const Quaternion& right) const;
    Quaternion<T>& operator*=(const Quaternion& right);

    Quaternion<T>  operator/(const Quaternion& right) const;
    Quaternion<T>& operator/=(const Quaternion& right);

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

VI_GE_NS_END