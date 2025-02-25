#pragma once

#include "ge_global.h"

#include <cstdint>

VI_GE_NS_BEGIN
template <typename T> class Point2;
template <typename T> class Vector2 {

  public:
    using ValueType = T;
    // using ValueTypePromoted = decltype(T() * T());

  public:
    Vector2();
    Vector2(T xx, T yy);

  public:
    void set(T xx, T yy);
    void get(T& xx, T& yy) const;

    /**
     * @brief 每分量是否小于误差值
     * @param eps
     * @return
     */
    bool isZero(T eps = T()) const;

    Point2<T>        toPoint() const;
    const Point2<T>& asPoint() const;
    bool             equals(const Vector2<T>& other, T eps = T()) const;

  public:
    bool        operator==(const Vector2<T>& right) const;
    bool        operator!=(const Vector2<T>& right) const;
    Vector2<T>  operator-(const Vector2<T>& right) const;
    Vector2<T>  operator+(const Vector2<T>& right) const;
    Vector2<T>& operator+=(const Vector2<T>& right);
    Vector2<T>& operator-=(const Vector2<T>& right);

    Vector2<T>  operator*(T scale) const;
    Vector2<T>& operator*=(T scale);
    Vector2<T>  operator/(T scale) const;
    Vector2<T>& operator/=(T scale);

  public:
    T x;
    T y;
};

using Vec2i  = Vector2<int32_t>;
using Vec2ui = Vector2<uint32_t>;
using Vec2f  = Vector2<float>;
using Vec2d  = Vector2<double>;

VI_GE_NS_END