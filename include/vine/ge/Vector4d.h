#pragma once

#include "Ge.h"
#include "ge_global.h"

VI_GE_NS_BEGIN
class Vector3d;
class VI_GE_API Vector4d {
  public:
    Vector4d();
    Vector4d(const Vector3d& v, double ww = 1.0);
    Vector4d(double xx, double yy, double zz, double ww);

  public:
    /*
    >0 -- 夹角小于90
    =0 -- 正交
    <0 -- 夹角大于90
    */
    double dotProduct(const Vector4d& vec) const noexcept;
    double angleTo(const Vector4d& vec) const noexcept;
    double length() const noexcept;
    double lengthSqrd() const noexcept;

    double normalize() noexcept;
    bool   setLength(double len) noexcept;
    void   set(double xx, double yy, double zz, double ww) noexcept;
    void   get(double& xx, double& yy, double& zz, double& ww) const noexcept;

    bool isZero(double eps = EPSD) const noexcept;
    bool isParalleTo(const Vector4d& vec, double eps = EPSD) const noexcept;
    bool isPerpendicularTo(const Vector4d& vec, double eps = EPSD) const noexcept;
    bool isNormalized(double eps = EPSD) const noexcept;

    const Vector3d& asVector3d() const noexcept;
    bool            equals(const Vector4d& other, double eps = EPSD) const;

  public:
    bool operator==(const Vector4d& right) const noexcept;
    bool operator!=(const Vector4d& right) const noexcept;

    Vector4d  operator-(const Vector4d& right) const noexcept;
    Vector4d  operator+(const Vector4d& right) const noexcept;
    Vector4d& operator+=(const Vector4d& right) noexcept;
    Vector4d& operator-=(const Vector4d& right) noexcept;
    Vector4d  operator*(double scale) const noexcept;
    Vector4d& operator*=(double scale) noexcept;
    Vector4d  operator/(double scale) const;
    Vector4d& operator/=(double scale);

    double operator*(const Vector4d& vec) const noexcept;

    double& operator[](size_t index);

  public:
    double x;
    double y;
    double z;
    double w;
};
VI_GE_NS_END