#pragma once

#include "ge_global.h"

VI_GE_NS_BEGIN
class Vector2d;
class Point3d;
class Matrix4x4;
class VI_GE_API Vector3d
{
public:
    Vector3d();
    Vector3d(double xx, double yy, double zz);

public:
    double dotProduct(const Vector3d &vec) const;
    Vector3d crossProduct(const Vector3d &vec) const;
    void normalize();
    Vector3d perpVector() const;

    double length() const;
    double lengthSqrd() const;

    void setLength(double len);
    void set(double xx, double yy, double zz);
    void get(double &xx, double &yy, double &zz) const;
    void rotate(const Matrix4x4 &mat);
    double angleTo(const Vector3d &vec);
    double angleTo(const Vector3d &vec, const Vector3d &refVec);

    bool isZeroLength() const;
    bool isParalleTo(const Vector3d &vec) const;
    bool isPerpendicularTo(const Vector3d &vec) const;

    Point3d toPoint() const;
    const Point3d &asPoint();
    Vector2d convert2d() const;

public:
    bool operator==(const Vector3d &right) const;
    bool operator!=(const Vector3d &right) const;
    Vector3d operator-(const Vector3d &right) const;
    Vector3d operator+(const Vector3d &right) const;
    Vector3d &operator+=(const Vector3d &right);
    Vector3d &operator-=(const Vector3d &right);

    Vector3d operator*(double scale) const;
    Vector3d &operator*=(double scale);
    Vector3d operator/(double scale) const;
    Vector3d &operator/=(double scale);

    Vector3d operator^(const Vector3d &vec) const;
    double operator*(const Vector3d &vec) const;

public:
    double x;
    double y;
    double z;
};
VI_GE_NS_END