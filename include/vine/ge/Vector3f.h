#pragma once

#include "Ge.h"
#include "ge_global.h"

VI_GE_NS_BEGIN
class Vector2f;
class Point3f;
class Matrixd4x4;
class VI_GE_API Vector3f {
  public:
    Vector3f();
    Vector3f(const Vector2f& v, float zz = 0.f);
    Vector3f(float xx, float yy, float zz);

  public:
    /*
    >0 -- 夹角小于90
    =0 -- 正交
    <0 -- 夹角大于90
    */
    float dotProduct(const Vector3f& vec) const noexcept;
    /*
    =(0,0,0) -- 共线或平行
    */
    Vector3f crossProduct(const Vector3f& vec) const noexcept;
    float    normalize() noexcept;
    Vector3f normalized() const noexcept;
    Vector3f perpVector() const noexcept;

    float length() const noexcept;
    float lengthSqrd() const noexcept;

    bool  setLength(float len) noexcept;
    void  set(float xx, float yy, float zz) noexcept;
    void  get(float& xx, float& yy, float& zz) const noexcept;
    void  rotate(const Matrixd4x4& mat);
    float angleTo(const Vector3f& vec) const noexcept;
    float angleTo(const Vector3f& vec, const Vector3f& refVec) const noexcept;

    bool isZero(float eps = EPSF) const noexcept;
    bool isParalleTo(const Vector3f& vec, float eps = EPSF) const noexcept;
    bool isPerpendicularTo(const Vector3f& vec, float eps = EPSF) const noexcept;
    bool isNormalized(float eps = EPSF) const noexcept;

    Point3f         toPoint() const noexcept;
    const Point3f&  asPoint() const noexcept;
    const Vector2f& asVector2f() const noexcept;
    bool            equals(const Vector3f& other, float eps = EPSF) const;

  public:
    bool operator==(const Vector3f& right) const noexcept;
    bool operator!=(const Vector3f& right) const noexcept;

    Vector3f  operator-(const Vector3f& right) const noexcept;
    Vector3f  operator+(const Vector3f& right) const noexcept;
    Vector3f& operator+=(const Vector3f& right) noexcept;
    Vector3f& operator-=(const Vector3f& right) noexcept;
    Vector3f  operator*(float scale) const noexcept;
    Vector3f& operator*=(float scale) noexcept;
    Vector3f  operator/(float scale) const;
    Vector3f& operator/=(float scale);

    Vector3f operator^(const Vector3f& vec) const noexcept;
    float    operator*(const Vector3f& vec) const noexcept;

    float& operator[](size_t index);

  public:
    float x;
    float y;
    float z;
};
VI_GE_NS_END