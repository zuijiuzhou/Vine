#pragma once

#include "Ge.h"
#include "ge_global.h"

VI_GE_NS_BEGIN
class Vector2d;
class Point3d;
class Matrix4x4;
class VI_GE_API Vector3d {
  public:
    Vector3d();
    Vector3d(double xx, double yy, double zz);

  public:
    /*
    >0 -- 夹角小于90
    =0 -- 正交
    <0 -- 夹角大于90
    */
    double   dotProduct(const Vector3d& vec) const noexcept;
	/*
    =(0,0,0) -- 共线或平行
    */
    Vector3d crossProduct(const Vector3d& vec) const noexcept;
    double   normalize() noexcept;
    Vector3d normalized() const noexcept;
    Vector3d perpVector() const noexcept;

    double length() const noexcept;
    double lengthSqrd() const noexcept;

    bool   setLength(double len) noexcept;
    void   set(double xx, double yy, double zz) noexcept;
    void   get(double& xx, double& yy, double& zz) const noexcept;
    void   rotate(const Matrix4x4& mat);
    double angleTo(const Vector3d& vec) const noexcept;
    double angleTo(const Vector3d& vec, const Vector3d& refVec) const noexcept;

    bool isZero(double eps = EPS) const noexcept;
    bool isParalleTo(const Vector3d& vec, double eps = EPS) const noexcept;
    bool isPerpendicularTo(const Vector3d& vec, double eps = EPS) const noexcept;
    bool isNormalized(double eps = EPS) const noexcept;

    Point3d         toPoint() const noexcept;
    const Point3d&  asPoint() const noexcept;
    const Vector2d& asVector2d() const noexcept;
    bool            equals(const Vector3d& other, double eps = EPS) const;

  public:
    bool operator==(const Vector3d& right) const noexcept;
    bool operator!=(const Vector3d& right) const noexcept;

    Vector3d  operator-(const Vector3d& right) const noexcept;
    Vector3d  operator+(const Vector3d& right) const noexcept;
    Vector3d& operator+=(const Vector3d& right) noexcept;
    Vector3d& operator-=(const Vector3d& right) noexcept;
    Vector3d  operator*(double scale) const noexcept;
    Vector3d& operator*=(double scale) noexcept;
    Vector3d  operator/(double scale) const;
    Vector3d& operator/=(double scale);

    Vector3d operator^(const Vector3d& vec) const noexcept;
    double   operator*(const Vector3d& vec) const noexcept;

    double& operator[](size_t index);

  public:
    double x;
    double y;
    double z;
};
VI_GE_NS_END