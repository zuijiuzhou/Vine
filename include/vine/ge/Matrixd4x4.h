#pragma once

#include "ge_global.h"

VI_GE_NS_BEGIN
class Point3d;
class Point3f;
class Vector3d;
class Vector3f;
class Matrixd3x3;
/*
ROW MAJOR MATRIX
M00 M01 M02 M03
M10 M11 M12 M13
M20 M21 M22 M23
M30 M31 M32 M33
 */
class VI_GE_API Matrixd4x4 {
  public:
    Matrixd4x4();

  public:
    void makeIdentity();
    void makeRotation(const Vector3d& start, const Vector3d& end);
    void makeRotation(const Vector3d& axis, double angle);
    void makeRotation(const Point3d& center, const Vector3d& axis, double angle);
    void makeTranslation(const Vector3d& offset);
    void makeTranslation(double x, double y, double z);
    void makeScale(const Vector3d& vec);
    void makeScale(double x, double y, double z);
    void makeScale(double factor);
    void makeLookAt(const Point3d& eye, const Point3d& target, const Vector3d& up);
    void setCoordSystem(const Point3d& origin, const Vector3d& xAxis, const Vector3d& yAxis, const Vector3d& zAxis);
    void getCoordSystem(Point3d& o_origin, Vector3d& o_xAxis, Vector3d& o_yAxis, Vector3d& o_zAxis) const;
    void transpose();
    Matrixd4x4 transposed() const;
    void       invert();
    Matrixd4x4 inverted() const;

    bool isRigid() const;
    bool isAffine() const;
    bool isIdentity() const;

  public:
    double      operator()(int row, int col) const;
    double&     operator()(int row, int col);
    Matrixd4x4  operator*(const Matrixd4x4& right) const;
    Matrixd4x4& operator*=(const Matrixd4x4& right);

  public:
    static Matrixd4x4 rotate(const Point3d& center, const Vector3d& axis, double angle);
    static Matrixd4x4 translate(const Vector3d& offset);
    static Matrixd4x4 translate(double x, double y, double z);
    static Matrixd4x4 scale(const Vector3d& vec);
    static Matrixd4x4 scale(double x, double y, double z);
    static Matrixd4x4 scale(double factor);
    static Matrixd4x4 lookAt(const Point3d& eye, const Point3d& target, const Vector3d& up);

  public:
    double data[4][4];
};

Vector3d operator*(const Matrixd4x4& m, const Vector3d& v);
Vector3f operator*(const Matrixd4x4& m, const Vector3f& v);
Point3d  operator*(const Matrixd4x4& m, const Point3d& p);
Point3f  operator*(const Matrixd4x4& m, const Point3f& p);

VI_GE_NS_END