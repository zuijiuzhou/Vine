#pragma once

#include "ge_global.h"

VI_GE_NS_BEGIN
class Point2i;
class VI_GE_API Vector2i {
  public:
    Vector2i();
    Vector2i(int xx, int yy);

  public:
    /*
    >0 -- 夹角小于90
    =0 -- 正交
    <0 -- 夹角大于90
    */
    int dotProduct(const Vector2i& vec) const;
    /*
    >0 -- 逆时针排列
    =0 -- 共线或平行
    <0 -- 顺时针排列
     */
    int      crossProduct(const Vector2i& vec) const;
    Vector2i perpVector() const;

    double length() const;
    int    lengthSqrd() const;

    void set(int xx, int yy);
    void get(int& xx, int& yy) const;

    /*
    0 < angle < pi
    */
    double angleTo(const Vector2i& vec) const;

    bool isZero() const;
    bool isParalleTo(const Vector2i& vec) const;
    bool isPerpendicularTo(const Vector2i& vec) const;

    Point2i        toPoint() const;
    const Point2i& asPoint() const;

  public:
    bool      operator==(const Vector2i& right) const;
    bool      operator!=(const Vector2i& right) const;
    Vector2i  operator-(const Vector2i& right) const;
    Vector2i  operator+(const Vector2i& right) const;
    Vector2i& operator+=(const Vector2i& right);
    Vector2i& operator-=(const Vector2i& right);

    Vector2i  operator*(int scale) const;
    Vector2i& operator*=(int scale);
    Vector2i  operator/(int scale) const;
    Vector2i& operator/=(int scale);

    int operator^(const Vector2i& vec) const;
    int operator*(const Vector2i& vec) const;

  public:
    int x;
    int y;
};
VI_GE_NS_END