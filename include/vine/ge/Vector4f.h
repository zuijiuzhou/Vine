#pragma once

#include "Ge.h"
#include "ge_global.h"

VI_GE_NS_BEGIN
class Vector3f;
class VI_GE_API Vector4f {
  public:
    Vector4f();
    Vector4f(const Vector3f& v, float ww = 1.0f);
    Vector4f(float xx, float yy, float zz, float ww);

  public:
    /*
    >0 -- 夹角小于90
    =0 -- 正交
    <0 -- 夹角大于90
    */
    float dotProduct(const Vector4f& vec) const noexcept;
    float angleTo(const Vector4f& vec) const noexcept;
    float length() const noexcept;
    float lengthSqrd() const noexcept;

    float normalize() noexcept;
    bool  setLength(float len) noexcept;
    void  set(float xx, float yy, float zz, float ww) noexcept;
    void  get(float& xx, float& yy, float& zz, float& ww) const noexcept;

    bool isZero(float eps = EPSF) const noexcept;
    bool isParalleTo(const Vector4f& vec, float eps = EPSF) const noexcept;
    bool isPerpendicularTo(const Vector4f& vec, float eps = EPSF) const noexcept;
    bool isNormalized(float eps = EPSF) const noexcept;

    const Vector3f& asVector3f() const noexcept;
    bool            equals(const Vector4f& other, float eps = EPSF) const;

  public:
    bool operator==(const Vector4f& right) const noexcept;
    bool operator!=(const Vector4f& right) const noexcept;

    Vector4f  operator-(const Vector4f& right) const noexcept;
    Vector4f  operator+(const Vector4f& right) const noexcept;
    Vector4f& operator+=(const Vector4f& right) noexcept;
    Vector4f& operator-=(const Vector4f& right) noexcept;
    Vector4f  operator*(float scale) const noexcept;
    Vector4f& operator*=(float scale) noexcept;
    Vector4f  operator/(float scale) const;
    Vector4f& operator/=(float scale);

    float operator*(const Vector4f& vec) const noexcept;

    float& operator[](size_t index);

  public:
    float x;
    float y;
    float z;
    float w;
};
VI_GE_NS_END