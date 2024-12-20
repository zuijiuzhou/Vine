#pragma once

#include "Ge.h"
#include "ge_global.h"

VI_GE_NS_BEGIN
class Point2d;
class VI_GE_API Vector2d {
  public:
    Vector2d();
    Vector2d(double xx, double yy);

  public:
    /*
    >0 -- 夹角小于90
    =0 -- 正交
    <0 -- 夹角大于90
    */
    double dotProduct(const Vector2d& vec) const;
    /*
    >0 -- 逆时针排列
    =0 -- 共线或平行
    <0 -- 顺时针排列
    */
    double   crossProduct(const Vector2d& vec) const;
    void     normalize();
    Vector2d perpVector() const;

    double length() const;
    double lengthSqrd() const;

    bool setLength(double len);
    void set(double xx, double yy);
    void get(double& xx, double& yy) const;
    // void rotate(const Matrix3x3& mat);
    double angleTo(const Vector2d& vec) const;

    bool isZero(double eps = EPS) const;
    bool isParalleTo(const Vector2d& vec, double eps = EPS) const;
    bool isPerpendicularTo(const Vector2d& vec, double eps = EPS) const;
    bool isNormalized(double eps = EPS) const noexcept;

    Point2d        toPoint() const;
    const Point2d& asPoint();
    bool           equals(const Vector2d& other, double eps = EPS) const;

  public:
    bool      operator==(const Vector2d& right) const;
    bool      operator!=(const Vector2d& right) const;
    Vector2d  operator-(const Vector2d& right) const;
    Vector2d  operator+(const Vector2d& right) const;
    Vector2d& operator+=(const Vector2d& right);
    Vector2d& operator-=(const Vector2d& right);

    Vector2d  operator*(double scale) const;
    Vector2d& operator*=(double scale);
    Vector2d  operator/(double scale) const;
    Vector2d& operator/=(double scale);

    double operator^(const Vector2d& vec) const;
    double operator*(const Vector2d& vec) const;

  public:
    double x;
    double y;
};
VI_GE_NS_END