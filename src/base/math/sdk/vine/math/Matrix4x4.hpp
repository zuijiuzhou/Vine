#pragma once

#include "math_global.hpp"

#include <cstring>

#include "Point3.hpp"
#include "Quaternion.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

VI_MATH_NS_BEGIN

/*
 * COLUMN MAJOR MATRIX
 *
 * | M00 M01 M02 M03 |
 * | M10 M11 M12 M13 |
 * | M20 M21 M22 M23 |
 * | M30 M31 M32 M33 |
 *
 * | Xx Yx Zx Tx |
 * | Xy Yy Zy Ty |
 * | Xz Yz Zz Tz |
 * |  0  0  0  1 |
 *
 * Memory order:
 * M00 M10 M20 M30 M01...
 */
template <typename T>
class Matrix4x4 {
  public:
    using value_type = T;

  public:
    constexpr Matrix4x4() noexcept
      : vecs{
          { 1, 0, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 0, 1, 0 },
          { 0, 0, 0, 1 }
    }
    {}

    Matrix4x4(const Quaternion<T>& quat)
    {
        makeRotation(quat);
    }

  public:
    void makeIdentity() noexcept
    {
        // std::fill((T*)vecs, ((T*)vecs)+16, 0.0);
        std::memset(data, 0, sizeof(data));
        vec0.x = vec1.y = vec2.z = vec3.w = T(1);
    }

    void makeRotation(const Vector3<T>& start, const Vector3<T>& end);
    void makeRotation(const Vector3<T>& axis, T angle);
    void makeRotation(const Quaternion<T>& quat);

    void makeTranslation(const Vector3<T>& offset) noexcept
    {
        makeIdentity();
        vecs[3][0] = offset.x;
        vecs[3][1] = offset.y;
        vecs[3][2] = offset.z;
    }

    void makeTranslation(T x, T y, T z) noexcept
    {
        makeIdentity();
        vecs[3][0] = x;
        vecs[3][1] = y;
        vecs[3][2] = z;
    }

    void makeScale(const Vector3<T>& vec) noexcept
    {
        makeIdentity();
        vecs[0][0] = vec.x;
        vecs[1][1] = vec.y;
        vecs[2][2] = vec.z;
    }

    void makeScale(T x, T y, T z) noexcept
    {
        makeIdentity();
        vecs[0][0] = x;
        vecs[1][1] = y;
        vecs[2][2] = z;
    }

    void makeScale(T factor) noexcept
    {
        makeIdentity();
        vecs[0][0] = factor;
        vecs[1][1] = factor;
        vecs[2][2] = factor;
    }

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
    void setBasis(const Point3<T>& origin, const Vector3<T>& x_axis, const Vector3<T>& y_axis, const Vector3<T>& z_axis);
    /**
     * @brief get the coordinate system represented by this matrix
     */
    void getBasis(Point3<T>& o_origin, Vector3<T>& o_x_axis, Vector3<T>& o_y_axis, Vector3<T>& o_z_axis) const;
    /**
     * @brief transpose the matrix
     */
    void transpose();

    /**
     * @brief return the transposed matrix without modifying the original one
     */
    Matrix4x4<T> transposed() const
    {
        Matrix4x4<T> m(*this);
        m.transpose();
        return m;
    }

    /**
     * @brief invert the matrix
     */
    void invert();

    /**
     * @brief return the inverted matrix without modifying the original one
     */
    Matrix4x4<T> inverted() const
    {
        Matrix4x4<T> m(*this);
        m.invert();
        return m;
    }

    /**
     * @brief is this matrix a rigid transformation matrix (only rotation and translation)
     * @param eps tolerance for floating-point comparisons
     */
    bool isRigid(T eps = EPS<T>()) const;
    /**
     * @brief is this matrix an affine transformation matrix (last row is [0 0 0 1]).
     *        affine matrix that preserve the parallelism of straight lines, such as translation, scaling, rotation,
     * shearing, and reflection. non-affine matrix includes projection matrix.
     * @param eps tolerance for floating-point comparisons
     */
    bool isAffine(T eps = EPS<T>()) const;
    /*
     * @brief is this matrix an identity matrix
     * @param eps tolerance for floating-point comparisons
     */
    bool isIdentity(T eps = EPS<T>()) const;

    /**
     * @brief is this matrix approximately equal to another matrix within a certain tolerance (epsilon).
     * @param other the matrix to compare with
     * @param eps tolerance for floating-point comparisons
     * @return true if the matrices are approximately equal, false otherwise
     */
    bool isEqual(const Matrix4x4<T>& other, T eps = EPS<T>()) const;

    /**
     * @brief check if all elements of the matrix are zero within a certain tolerance (epsilon).
     * @param eps tolerance for floating-point comparisons
     * @return true if all elements are approximately zero, false otherwise
     */
    bool isZero(T eps = EPS<T>()) const;

  public:
    [[nodiscard]]
    T operator()(int row, int col) const
    {
        return vecs[col][row];
    }

    [[nodiscard]]
    T& operator()(int row, int col)
    {
        return vecs[col][row];
    }

    Matrix4x4<T>  operator*(const Matrix4x4<T>& right) const;
    Matrix4x4<T>& operator*=(const Matrix4x4<T>& right);

  public:
    [[nodiscard]]
    static Matrix4x4<T> rotate(const Vector3<T>& start, const Vector3<T>& end)
    {
        Matrix4x4<T> m;
        m.makeRotation(start, end);
        return m;
    }

    [[nodiscard]]
    static Matrix4x4<T> rotate(const Vector3<T>& axis, T angle)
    {
        Matrix4x4<T> m;
        m.makeRotation(axis, angle);
        return m;
    }

    [[nodiscard]]
    static Matrix4x4<T> translate(const Vector3<T>& offset)
    {
        Matrix4x4<T> m;
        m.makeTranslation(offset);
        return m;
    }

    [[nodiscard]]
    static Matrix4x4<T> translate(T x, T y, T z)
    {
        Matrix4x4<T> m;
        m.makeTranslation(x, y, z);
        return m;
    }

    [[nodiscard]]
    static Matrix4x4<T> scale(const Vector3<T>& vec)
    {
        Matrix4x4<T> m;
        m.makeScale(vec);
        return m;
    }

    [[nodiscard]]
    static Matrix4x4<T> scale(T x, T y, T z)
    {
        Matrix4x4<T> m;
        m.makeScale(x, y, z);
        return m;
    }

    [[nodiscard]]
    static Matrix4x4<T> scale(T factor)
    {
        Matrix4x4<T> m;
        m.makeScale(factor);
        return m;
    }

    [[nodiscard]]
    static Matrix4x4<T> lookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up)
    {
        Matrix4x4<T> m;
        m.makeLookAt(eye, target, up);
        return m;
    }

    [[nodiscard]]
    static Matrix4x4<T> ortho(double left, double right, double bottom, double top, double z_near, double z_far)
    {
        Matrix4x4<T> m;
        m.makeOrtho(left, right, bottom, top, z_near, z_far);
        return m;
    }

    [[nodiscard]]
    static Matrix4x4<T> perspective(double fovy, double aspect_ratio, double z_near, double z_far)
    {
        Matrix4x4<T> m;
        m.makePerspective(fovy, aspect_ratio, z_near, z_far);
        return m;
    }

    [[nodiscard]]
    static Matrix4x4<T> fromBasis(const Point3<T>& origin, const Vector3<T>& x_axis, const Vector3<T>& y_axis, const Vector3<T>& z_axis)
    {
        Matrix4x4<T> m;
        m.setBasis(origin, x_axis, y_axis, z_axis);
        return m;
    }

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

VI_MATH_NS_END
