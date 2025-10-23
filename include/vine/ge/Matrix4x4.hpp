#pragma once

#include "ge_global.hpp"

#include "Point3.hpp"
#include "Quaternion.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

VI_GE_NS_BEGIN

/*
COLUMN MAJOR MATRIX
M00 M01 M02 M03
M10 M11 M12 M13
M20 M21 M22 M23
M30 M31 M32 M33
 */
template <typename T>
class Matrix4x4 {
  public:
    using value_type  = T;

  public:
    Matrix4x4();
    Matrix4x4(const Quaternion<T>& quat);

  public:
    void makeIdentity();
    void makeRotation(const Vector3<T>& start, const Vector3<T>& end);
    void makeRotation(const Vector3<T>& axis, T angle);
    void makeTranslation(const Vector3<T>& offset);
    void makeTranslation(T x, T y, T z);
    void makeScale(const Vector3<T>& vec);
    void makeScale(T x, T y, T z);
    void makeScale(T factor);
    void makeLookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up);
    /**
     * @brief make an orthographic projection matrix
     * @param left the left clipping plane
     * @param right the right clipping plane
     * @param bottom the bottom clipping plane
     * @param top the top clipping plane
     * @param z_near the near clipping plane
     * @param z_far the far clipping plane
     */
    void makeOrtho(double left, double right, double bottom, double top, double z_near, double z_far);
    /**
     * @brief make a perspective matrix
     * @param fovy the field of view angle in the y direction
     * @param aspect_ratio the aspect ratio of the viewport
     * @param z_near the near clipping plane
     * @param z_far the far clipping plane
     */
    void makePerspective(double fovy, double aspect_ratio, double z_near, double z_far);
    /**
     * @brief set the coordinate system represented by this matrix
     * @param origin the origin point of the coordinate system
     * @param xAxis the x axis direction of the coordinate system
     * @param yAxis the y axis direction of the coordinate system
     * @param zAxis the z axis direction of the coordinate system
     */
    void
    setCoordSystem(const Point3<T>& origin, const Vector3<T>& x_axis, const Vector3<T>& y_axis, const Vector3<T>& z_axis);
    /**
     * @brief get the coordinate system represented by this matrix
     */
    void getCoordSystem(Point3<T>& o_origin, Vector3<T>& o_x_axis, Vector3<T>& o_y_axis, Vector3<T>& o_z_axis) const;
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
    static Matrix4x4<T> rotate(const Vector3<T>& start, const Vector3<T>& end);
    static Matrix4x4<T> rotate(const Vector3<T>& axis, T angle);
    static Matrix4x4<T> translate(const Vector3<T>& offset);
    static Matrix4x4<T> translate(T x, T y, T z);
    static Matrix4x4<T> scale(const Vector3<T>& vec);
    static Matrix4x4<T> scale(T x, T y, T z);
    static Matrix4x4<T> scale(T factor);
    static Matrix4x4<T> lookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up);

  public:
    union
    {
        // struct {
        //     T m00, m01, m02, m03;
        //     T m10, m11, m12, m13;
        //     T m20, m21, m22, m23;
        //     T m30, m31, m32, m33;
        // };

        struct {
            Vector4<T> vec0;
            Vector4<T> vec1;
            Vector4<T> vec2;
            Vector4<T> vec3;
        };

        Vector4<T> vecs[4];

        T data[16];
    };
};

template <typename T>
Vector3<T> operator*(const Matrix4x4<T>& m, const Vector3<T>& v);
template <typename T>
Point3<T> operator*(const Matrix4x4<T>& m, const Point3<T>& p);

using Mat4f = Matrix4x4<float>;
using Mat4d = Matrix4x4<double>;

VI_GE_NS_END