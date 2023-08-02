#pragma once

#include "ge_global.h"

VINE_GE_NS_BEGIN
class Point3d;
class Vector3d;
class VINE_GE_API Matrix4x4
{
public:
    double data[4][4];

    Matrix4x4();

public:
    void makeIdentity();
    void makeRotation(const Point3d &center, const Vector3d &axis, double angle);
    void makeTranslation(const Vector3d &offset);
    void makeTranslation(double x, double y, double z);
    void makeScale(const Vector3d &vec);
    void makeScale(double x, double y, double z);
    void makeScale(double factor);
    void makeLookAt(const Point3d &eye, const Point3d &target, const Vector3d &up);
    void setCoordSystem(const Point3d &origin, const Vector3d &xAxis, const Vector3d &yAxis, const Vector3d &zAxis);
    void transpose();
    void invert();

    bool isAffine() const;
    bool isIdentity() const;

public:
    double operator()(int row, int col) const;
    double &operator()(int row, int col);
    Matrix4x4 operator*(const Matrix4x4 &right) const;
    Matrix4x4 &operator*=(const Matrix4x4 &right) const;

public:
    static Matrix4x4 rotate(const Point3d &center, const Vector3d &axis, double angle);
    static Matrix4x4 translate(const Vector3d &offset);
    static Matrix4x4 translate(double x, double y, double z);
    static Matrix4x4 scale(const Vector3d &vec);
    static Matrix4x4 scale(double x, double y, double z);
    static Matrix4x4 scale(double factor);
    static Matrix4x4 lookAt(const Point3d &eye, const Point3d &target, const Vector3d &up);
};
VINE_GE_NS_END