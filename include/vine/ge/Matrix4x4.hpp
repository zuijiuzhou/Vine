#pragma once

#include "ge_global.hpp"

#include "Vector4.hpp"

VI_GE_NS_BEGIN

template <typename T>
class Point3;
template <typename T>
class Vector3;

/*
COLUMN MAJOR MATRIX
M00 M10 M20 M30
M01 M11 M21 M31
M02 M12 M22 M32
M03 M13 M23 M33
 */
template <typename T>
class Matrix4x4 {
  public:
    using value_type = T;

  public:
    Matrix4x4();

  public:
    void makeIdentity();
    void makeRotation(const Vector3<T>& start, const Vector3<T>& end);
    void makeRotation(const Vector3<T>& axis, T angle);
    void makeRotation(const Point3<T>& center, const Vector3<T>& axis, T angle);
    void makeTranslation(const Vector3<T>& offset);
    void makeTranslation(T x, T y, T z);
    void makeScale(const Vector3<T>& vec);
    void makeScale(T x, T y, T z);
    void makeScale(T factor);
    void makeLookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up);
    /**
     * @brief set the coordinate system represented by this matrix
     * @param origin the origin point of the coordinate system
     * @param xAxis the x axis direction of the coordinate system
     * @param yAxis the y axis direction of the coordinate system
     * @param zAxis the z axis direction of the coordinate system
     */
    void
    setCoordSystem(const Point3<T>& origin, const Vector3<T>& xAxis, const Vector3<T>& yAxis, const Vector3<T>& zAxis);
    /**
     * @brief get the coordinate system represented by this matrix
     */
    void getCoordSystem(Point3<T>& o_origin, Vector3<T>& o_xAxis, Vector3<T>& o_yAxis, Vector3<T>& o_zAxis) const;
    /**
     * @brief transpose the matrix
     */
    void transpose();
    /**
     * @brief return the transposed matrix without modifying the original one
     */
    Matrix4x4<T> transposed() const;
    /**
     * @brief invert the matrix
     */
    void invert();

    /**
     * @brief return the inverted matrix without modifying the original one
     */
    Matrix4x4<T> inverted() const;

    /**
     * @brief is this matrix a rigid transformation matrix (only rotation and translation)
     */
    bool isRigid() const;
    /**
     * @brief is this matrix an affine transformation matrix (last row is [0 0 0 1]).
     *        affine matrix that preserve the parallelism of straight lines, such as translation, scaling, rotation,
     * shearing, and reflection. non-affine matrix includes projection matrix.
     */
    bool isAffine() const;
    /*
     * @brief is this matrix an identity matrix
     */
    bool isIdentity() const;

  public:
    T             operator()(int row, int col) const;
    T&            operator()(int row, int col);
    Matrix4x4<T>  operator*(const Matrix4x4<T>& right) const;
    Matrix4x4<T>& operator*=(const Matrix4x4<T>& right);

  public:
    static Matrix4x4<T> rotate(const Point3<T>& center, const Vector3<T>& axis, T angle);
    static Matrix4x4<T> translate(const Vector3<T>& offset);
    static Matrix4x4<T> translate(T x, T y, T z);
    static Matrix4x4<T> scale(const Vector3<T>& vec);
    static Matrix4x4<T> scale(T x, T y, T z);
    static Matrix4x4<T> scale(T factor);
    static Matrix4x4<T> lookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up);

  public:
    union
    {
        struct {
            T m00, m01, m02, m03;
            T m10, m11, m12, m13;
            T m20, m21, m22, m23;
            T m30, m31, m32, m33;
        };

        Vector4<T> cols[4];
    };
};

template <typename T>
Vector3<T> operator*(const Matrix4x4<T>& m, const Vector3<T>& v);
template <typename T>
Point3<T> operator*(const Matrix4x4<T>& m, const Point3<T>& p);

using Mat4f = Matrix4x4<float>;
using Mat4d = Matrix4x4<double>;

VI_GE_NS_END