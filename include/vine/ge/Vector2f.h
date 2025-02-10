#pragma once

#include "Ge.h"
#include "ge_global.h"

VI_GE_NS_BEGIN
class Point2f;
class VI_GE_API Vector2f {
  public:
    Vector2f();
    Vector2f(float xx, float yy);

  public:
    /*
    >0 -- �н�С��90
    =0 -- ����
    <0 -- �нǴ���90
    */
    float dotProduct(const Vector2f& vec) const;
    /*
    >0 -- ��ʱ������
    =0 -- ���߻�ƽ��
    <0 -- ˳ʱ������
    */
    float    crossProduct(const Vector2f& vec) const;
    void     normalize();
    Vector2f perpVector() const;

    float length() const;
    float lengthSqrd() const;

    bool setLength(float len);
    void set(float xx, float yy);
    void get(float& xx, float& yy) const;
    // void rotate(const Matrixd3x3& mat);
    float angleTo(const Vector2f& vec) const;

    bool isZero(float eps = EPSF) const;
    bool isParalleTo(const Vector2f& vec, float eps = EPSF) const;
    bool isPerpendicularTo(const Vector2f& vec, float eps = EPSF) const;
    bool isNormalized(float eps = EPSF) const noexcept;

    Point2f        toPoint() const;
    const Point2f& asPoint();
    bool           equals(const Vector2f& other, float eps = EPSF) const;

  public:
    bool      operator==(const Vector2f& right) const;
    bool      operator!=(const Vector2f& right) const;
    Vector2f  operator-(const Vector2f& right) const;
    Vector2f  operator+(const Vector2f& right) const;
    Vector2f& operator+=(const Vector2f& right);
    Vector2f& operator-=(const Vector2f& right);

    Vector2f  operator*(float scale) const;
    Vector2f& operator*=(float scale);
    Vector2f  operator/(float scale) const;
    Vector2f& operator/=(float scale);

    float operator^(const Vector2f& vec) const;
    float operator*(const Vector2f& vec) const;

  public:
    float x;
    float y;
};
VI_GE_NS_END