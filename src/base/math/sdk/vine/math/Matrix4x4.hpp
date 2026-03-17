#pragma once

#include "math_global.hpp"

#include <cstring>

#include "Point3.hpp"
#include "Quaternion.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

V_MATH_NS_BEGIN

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
    /**
     * @brief Construct an identity matrix.
     */
    constexpr Matrix4x4() noexcept
      : vecs{
          { 1, 0, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 0, 1, 0 },
          { 0, 0, 0, 1 }
    }
    {}

    /**
     * @brief Construct a rotation matrix from quaternion.
     * @param quat Rotation quaternion.
     */
    Matrix4x4(const Quaternion<T>& quat)
    {
        makeRotation(quat);
    }

  public:
    /**
     * @brief Reset this matrix to identity.
     */
    void makeIdentity() noexcept
    {
        // std::fill((T*)vecs, ((T*)vecs)+16, 0.0);
        std::memset(data, 0, sizeof(data));
        vec0.x = vec1.y = vec2.z = vec3.w = T(1);
    }

    /**
     * @brief Build rotation from start vector to end vector.
     * @param start Source direction vector.
     * @param end Target direction vector.
     */
    void makeRotation(const Vector3<T>& start, const Vector3<T>& end);
    /**
     * @brief Build axis-angle rotation matrix.
     * @param axis Rotation axis.
     * @param angle Rotation angle in radians.
     */
    void makeRotation(const Vector3<T>& axis, T angle);
    /**
     * @brief Build rotation matrix from quaternion.
     * @param quat Rotation quaternion.
     */
    void makeRotation(const Quaternion<T>& quat);

    /**
     * @brief Build translation matrix from offset vector.
     * @param offset Translation offset.
     */
    void makeTranslation(const Vector3<T>& offset) noexcept
    {
        makeIdentity();
        vecs[3][0] = offset.x;
        vecs[3][1] = offset.y;
        vecs[3][2] = offset.z;
    }

    /**
     * @brief Build translation matrix from components.
     * @param x Translation along X axis.
     * @param y Translation along Y axis.
     * @param z Translation along Z axis.
     */
    void makeTranslation(T x, T y, T z) noexcept
    {
        makeIdentity();
        vecs[3][0] = x;
        vecs[3][1] = y;
        vecs[3][2] = z;
    }

    /**
     * @brief Build non-uniform scale matrix from vector.
     * @param vec Scale factors for x/y/z.
     */
    void makeScale(const Vector3<T>& vec) noexcept
    {
        makeIdentity();
        vecs[0][0] = vec.x;
        vecs[1][1] = vec.y;
        vecs[2][2] = vec.z;
    }

    /**
     * @brief Build non-uniform scale matrix from components.
     * @param x Scale factor along X axis.
     * @param y Scale factor along Y axis.
     * @param z Scale factor along Z axis.
     */
    void makeScale(T x, T y, T z) noexcept
    {
        makeIdentity();
        vecs[0][0] = x;
        vecs[1][1] = y;
        vecs[2][2] = z;
    }

    /**
     * @brief Build uniform scale matrix.
     * @param factor Uniform scale factor.
     */
    void makeScale(T factor) noexcept
    {
        makeIdentity();
        vecs[0][0] = factor;
        vecs[1][1] = factor;
        vecs[2][2] = factor;
    }

    /**
     * @brief Build a look-at view matrix.
     * @param eye Camera position.
     * @param target Camera target point.
     * @param up Up direction reference.
     * @note Uses backward-axis convention: backward = eye - target.
     */
    void makeLookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up);
    /**
     * @brief Make an orthographic projection matrix.
     * @param left the left clipping plane.
     * @param right the right clipping plane.
     * @param bottom the bottom clipping plane.
     * @param top the top clipping plane.
     * @param z_near the near clipping plane.
     * @param z_far the far clipping plane.
     */
    void makeOrtho(double left, double right, double bottom, double top, double z_near, double z_far);
    /**
     * @brief Make a perspective matrix.
     * @param fovy the field of view angle in the y direction.
     * @param aspect_ratio the aspect ratio of the viewport.
     * @param z_near the near clipping plane.
     * @param z_far the far clipping plane.
     */
    void makePerspective(double fovy, double aspect_ratio, double z_near, double z_far);

    /**
     * @brief Make a reflection matrix across a plane defined by its normal and offset.
     * @param plane_normal Normal vector of the mirror plane.
     * @param plane_offset Offset of the mirror plane from the origin.
     */
    void makeReflection(const Vector3<T>& plane_normal, T plane_offset);

    /**
     * @brief Set the coordinate system represented by this matrix.
     * @param origin the origin point of the coordinate system.
     * @param xAxis the x axis direction of the coordinate system.
     * @param yAxis the y axis direction of the coordinate system.
     * @param zAxis the z axis direction of the coordinate system.
     */
    void setBasis(const Point3<T>& origin, const Vector3<T>& x_axis, const Vector3<T>& y_axis, const Vector3<T>& z_axis);
    /**
     * @brief Get the coordinate system represented by this matrix.
     * @param o_origin Output origin point.
     * @param o_x_axis Output x-axis direction.
     * @param o_y_axis Output y-axis direction.
     * @param o_z_axis Output z-axis direction.
     */
    void getBasis(Point3<T>& o_origin, Vector3<T>& o_x_axis, Vector3<T>& o_y_axis, Vector3<T>& o_z_axis) const;

    /**
     * @brief Left-multiply this matrix by another matrix.
     * @param left Left matrix in M := left * M.
     * @return Reference to this matrix.
     * @note Pre-operations are commonly used for world/fixed-axis composition.
     */
    Matrix4x4<T>& preMulti(const Matrix4x4<T>& left);
    /**
     * @brief Right-multiply this matrix by another matrix.
     * @param right Right matrix in M := M * right.
     * @return Reference to this matrix.
     * @note Post-operations are commonly used for local/moving-axis composition.
     */
    Matrix4x4<T>& postMulti(const Matrix4x4<T>& right);
    /**
     * @brief Prepend an axis-angle rotation.
     * @param axis Rotation axis.
     * @param angle Rotation angle in radians.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& preRotate(const Vector3<T>& axis, T angle);
    /**
     * @brief Append an axis-angle rotation.
     * @param axis Rotation axis.
     * @param angle Rotation angle in radians.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& postRotate(const Vector3<T>& axis, T angle);
    /**
     * @brief Prepend a quaternion rotation.
     * @param quat Rotation quaternion.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& preRotate(const Quaternion<T>& quat);
    /**
     * @brief Append a quaternion rotation.
     * @param quat Rotation quaternion.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& postRotate(const Quaternion<T>& quat);
    /**
     * @brief Prepend a translation.
     * @param offset Translation offset.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& preTranslate(const Vector3<T>& offset);
    /**
     * @brief Append a translation.
     * @param offset Translation offset.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& postTranslate(const Vector3<T>& offset);
    /**
     * @brief Prepend a non-uniform scale.
     * @param factor Scale factor per axis.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& preScale(const Vector3<T>& factor);
    /**
     * @brief Append a non-uniform scale.
     * @param factor Scale factor per axis.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& postScale(const Vector3<T>& factor);

    /**
     * @brief Calculate the determinant of the matrix.
     * @return Determinant value.
     */
    T determinant() const;

    /**
     * @brief Transpose the matrix.
     */
    void transpose();

    /**
     * @brief Return the transposed matrix without modifying the original one.
     */
    Matrix4x4<T> transposed() const
    {
        Matrix4x4<T> m(*this);
        m.transpose();
        return m;
    }

    /**
     * @brief Invert the matrix.
     * @note Inverse exists only when determinant is non-zero (det(M) != 0).
     * @note Typical invertible cases:
     *       - Rigid transforms (rotation + translation).
     *       - Affine transforms with non-zero scale on all axes.
     *       - Basis matrix with linearly independent x/y/z axes.
     * @note Typical non-invertible cases:
     *       - Any axis scale is zero (matrix squashes dimension).
     *       - Basis axes are linearly dependent (determinant becomes zero).
     *       - Any transform that collapses 3D space into lower dimension.
     * @note For singular matrices (det(M) == 0), current implementation returns an unchanged copy.
     */
    void invert();

    /**
     * @brief Return the inverted matrix without modifying the original one.
     */
    Matrix4x4<T> inverted() const
    {
        Matrix4x4<T> m(*this);
        m.invert();
        return m;
    }

    /**
     * @brief Get the translation component of this matrix.
     * @return Vector3<T>.
     */
    Vector3<T> translation() const
    {
        return Vector3<T>(vecs[3][0], vecs[3][1], vecs[3][2]);
    }

    /**
     * @brief Get the rotation component of this matrix as a quaternion.
     * @return Quaternion<T>.
     */
    Quaternion<T> rotation() const;

    /*
     * @brief Get the non-uniform scaling factors of this matrix.
     * @return Vector3<T> where x/y/z components represent scale along respective axes.
     * @note If the matrix contains rotation, the extracted scaling factors may not be accurate.
     *       For pure scale or uniform scale, the returned vector will be correct.
     */
    Vector3<T> scaleFactors() const;

    /*
     * @brief Is this matrix an identity matrix.
     * @param eps tolerance for floating-point comparisons.
     */
    bool isIdentity(T eps = EPS<T>()) const;
    /**
     * @brief Is this matrix an affine transformation matrix (last row is [0 0 0 1]).
     *        affine matrix that preserve the parallelism of straight lines, such as translation, scaling, rotation,
     *        shearing, and reflection. non-affine matrix includes projection matrix.
     * @param eps Tolerance for floating-point comparisons.
     */
    bool isAffine(T eps = EPS<T>()) const;
    /**
     * @brief Is this matrix a rigid transformation matrix (only rotation and translation, no scaling or shearing or reflection).
     * @param eps Tolerance for floating-point comparisons.
     */
    bool isRigid(T eps = EPS<T>()) const;

    /**
     * @brief Is this matrix approximately equal to another matrix within a certain tolerance (epsilon).
     * @param other The matrix to compare with.
     * @param eps Tolerance for floating-point comparisons.
     * @return True if the matrices are approximately equal, false otherwise.
     */
    bool isEqual(const Matrix4x4<T>& other, T eps = EPS<T>()) const;

    /**
     * @brief Check if all elements of the matrix are zero within a certain tolerance (epsilon).
     * @param eps tolerance for floating-point comparisons.
     * @return true if all elements are approximately zero, false otherwise.
     */
    bool isZero(T eps = EPS<T>()) const;

  public:
    /**
     * @brief Read matrix element by row and column.
     * @param row Row index.
     * @param col Column index.
     * @return Element value at (row, col).
     */
    [[nodiscard]]
    T operator()(int row, int col) const
    {
        return vecs[col][row];
    }

    /**
     * @brief Access matrix element by row and column.
     * @param row Row index.
     * @param col Column index.
     * @return Mutable element reference at (row, col).
     */
    [[nodiscard]]
    T& operator()(int row, int col)
    {
        return vecs[col][row];
    }

    /**
     * @brief Matrix multiplication.
     * @param right Right-hand matrix.
     * @return Product matrix.
     */
    Matrix4x4<T> operator*(const Matrix4x4<T>& right) const;
    /**
     * @brief Matrix multiplication assignment.
     * @param right Right-hand matrix.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& operator*=(const Matrix4x4<T>& right);

  public:
    /**
     * @brief Create a rotation matrix from start and end vectors.
     * @param start Source direction vector.
     * @param end Target direction vector.
     * @return Rotation matrix.
     */
    [[nodiscard]]
    static Matrix4x4<T> rotate(const Vector3<T>& start, const Vector3<T>& end)
    {
        Matrix4x4<T> m;
        m.makeRotation(start, end);
        return m;
    }

    /**
     * @brief Create an axis-angle rotation matrix.
     * @param axis Rotation axis.
     * @param angle Rotation angle in radians.
     * @return Rotation matrix.
     */
    [[nodiscard]]
    static Matrix4x4<T> rotate(const Vector3<T>& axis, T angle)
    {
        Matrix4x4<T> m;
        m.makeRotation(axis, angle);
        return m;
    }

    /**
     * @brief Create a translation matrix from vector.
     * @param offset Translation offset.
     * @return Translation matrix.
     */
    [[nodiscard]]
    static Matrix4x4<T> translate(const Vector3<T>& offset)
    {
        Matrix4x4<T> m;
        m.makeTranslation(offset);
        return m;
    }

    /**
     * @brief Create a translation matrix from components.
     * @param x Translation along X axis.
     * @param y Translation along Y axis.
     * @param z Translation along Z axis.
     * @return Translation matrix.
     */
    [[nodiscard]]
    static Matrix4x4<T> translate(T x, T y, T z)
    {
        Matrix4x4<T> m;
        m.makeTranslation(x, y, z);
        return m;
    }

    /**
     * @brief Create a non-uniform scale matrix from vector.
     * @param vec Scale factors for x/y/z.
     * @return Scale matrix.
     */
    [[nodiscard]]
    static Matrix4x4<T> scale(const Vector3<T>& vec)
    {
        Matrix4x4<T> m;
        m.makeScale(vec);
        return m;
    }

    /**
     * @brief Create a non-uniform scale matrix from components.
     * @param x Scale factor along X axis.
     * @param y Scale factor along Y axis.
     * @param z Scale factor along Z axis.
     * @return Scale matrix.
     */
    [[nodiscard]]
    static Matrix4x4<T> scale(T x, T y, T z)
    {
        Matrix4x4<T> m;
        m.makeScale(x, y, z);
        return m;
    }

    /**
     * @brief Create a uniform scale matrix.
     * @param factor Uniform scale factor.
     * @return Scale matrix.
     */
    [[nodiscard]]
    static Matrix4x4<T> scale(T factor)
    {
        Matrix4x4<T> m;
        m.makeScale(factor);
        return m;
    }

    /**
     * @brief Create a look-at view matrix.
     * @param eye Camera position.
     * @param target Camera target point.
     * @param up Up direction reference.
     * @return View matrix.
     */
    [[nodiscard]]
    static Matrix4x4<T> lookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up)
    {
        Matrix4x4<T> m;
        m.makeLookAt(eye, target, up);
        return m;
    }

    /**
     * @brief Create an orthographic projection matrix.
     * @param left Left clipping plane.
     * @param right Right clipping plane.
     * @param bottom Bottom clipping plane.
     * @param top Top clipping plane.
     * @param z_near Near clipping plane.
     * @param z_far Far clipping plane.
     * @return Orthographic projection matrix.
     */
    [[nodiscard]]
    static Matrix4x4<T> ortho(double left, double right, double bottom, double top, double z_near, double z_far)
    {
        Matrix4x4<T> m;
        m.makeOrtho(left, right, bottom, top, z_near, z_far);
        return m;
    }

    /**
     * @brief Create a perspective projection matrix.
     * @param fovy Vertical field of view in radians.
     * @param aspect_ratio Viewport aspect ratio.
     * @param z_near Near clipping plane.
     * @param z_far Far clipping plane.
     * @return Perspective projection matrix.
     */
    [[nodiscard]]
    static Matrix4x4<T> perspective(double fovy, double aspect_ratio, double z_near, double z_far)
    {
        Matrix4x4<T> m;
        m.makePerspective(fovy, aspect_ratio, z_near, z_far);
        return m;
    }

    /**
     * @brief Create a matrix from basis vectors and origin.
     * @param origin Coordinate system origin.
     * @param x_axis X-axis direction.
     * @param y_axis Y-axis direction.
     * @param z_axis Z-axis direction.
     * @return Transform matrix composed from basis.
     */
    [[nodiscard]]
    static Matrix4x4<T> fromBasis(const Point3<T>& origin, const Vector3<T>& x_axis, const Vector3<T>& y_axis, const Vector3<T>& z_axis)
    {
        Matrix4x4<T> m;
        m.setBasis(origin, x_axis, y_axis, z_axis);
        return m;
    }

    /**
     * @brief Create a reflection matrix across a plane.
     * @param plane_normal Normal vector of the plane.
     * @param plane_offset Offset of the plane from the origin.
     * @return Reflection matrix.
     */
    [[nodiscard]]
    static Matrix4x4<T> reflect(const Vector3<T>& plane_normal, T plane_offset)
    {
        Matrix4x4<T> m;
        m.makeReflection(plane_normal, plane_offset);
        return m;
    }

  public:
    union
    {
        // struct {
        //     T m00, m01, m02, m03; vec0
        //     T m10, m11, m12, m13; vec1
        //     T m20, m21, m22, m23; vec2
        //     T m30, m31, m32, m33; vec3
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

V_MATH_NS_END
