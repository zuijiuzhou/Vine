#pragma once

#include "math_global.hpp"

#include <cstring>

#include "Point3.hpp"
#include "Quaternion3.hpp"
#include "Transform3.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

V_MATH_NS_BEGIN

/**
 * @brief 4x4 column-major matrix used for 3D transforms and projections.
 *
 * Layout (visualized by rows):
 * | m00 m01 m02 m03 |
 * | m10 m11 m12 m13 |
 * | m20 m21 m22 m23 |
 * | m30 m31 m32 m33 |
 *
 * Column interpretation (columns contain basis vectors and translation):
 * - Column 0 = X axis (Xx, Xy, Xz, 0)
 * - Column 1 = Y axis (Yx, Yy, Yz, 0)
 * - Column 2 = Z axis (Zx, Zy, Zz, 0)
 * - Column 3 = Translation (Tx, Ty, Tz, 1)
 *
 * Memory order: M00, M10, M20, M30, M01, ... (column-major contiguous)
 *
 * Conventions:
 * - Right-handed coordinate system is assumed.
 * - Matrices are column-major and operate on column vectors: p' = M * p.
 *   For an affine transform M = [R | t; 0 1], this means p' = R * p + t.
 * - Angles are in radians unless otherwise documented (e.g. rotation and perspective).
 * - This class supports affine and projection matrices. Rigid-transform helpers
 *   create/expect no scaling/shearing unless explicitly using the scale APIs.
 *
 * @tparam T floating-point type (typically `float` or `double`).
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
     * @brief Construct a matrix from raw elements in row-major order.
     *
     * Parameters follow the logical matrix layout:
     * | m00 m01 m02 m03 |
     * | m10 m11 m12 m13 |
     * | m20 m21 m22 m23 |
     * | m30 m31 m32 m33 |
     *
     * @param _m00 Row 0, col 0. @param _m01 Row 0, col 1. @param _m02 Row 0, col 2. @param _m03 Row 0, col 3.
     * @param _m10 Row 1, col 0. @param _m11 Row 1, col 1. @param _m12 Row 1, col 2. @param _m13 Row 1, col 3.
     * @param _m20 Row 2, col 0. @param _m21 Row 2, col 1. @param _m22 Row 2, col 2. @param _m23 Row 2, col 3.
     * @param _m30 Row 3, col 0. @param _m31 Row 3, col 1. @param _m32 Row 3, col 2. @param _m33 Row 3, col 3.
     */
    constexpr Matrix4x4(T _m00, T _m01, T _m02, T _m03, T _m10, T _m11, T _m12, T _m13, T _m20, T _m21, T _m22, T _m23, T _m30, T _m31, T _m32, T _m33) noexcept
      : vecs{
          { _m00, _m10, _m20, _m30 },
          { _m01, _m11, _m21, _m31 },
          { _m02, _m12, _m22, _m32 },
          { _m03, _m13, _m23, _m33 }
    }
    {}

    /**
     * @brief Construct a matrix from a 16-element array in column-major order.
     *
     * elements[0..15] match the internal data[] layout:
     * col0: elements[0..3], col1: elements[4..7],
     * col2: elements[8..11], col3: elements[12..15].
     *
     * @warning The pointer is not validated. Passing nullptr or fewer than
     *          16 elements results in undefined behavior. Prefer the array-
     *          reference overload `Matrix4x4(const T (&)[16])` when the
     *          array size is known at compile time.
     *
     * @param elements Pointer to 16 T values in column-major layout.
     */
    explicit Matrix4x4(const T* elements) noexcept
      : vecs{
          { elements[0],  elements[1],  elements[2],  elements[3]  },
          { elements[4],  elements[5],  elements[6],  elements[7]  },
          { elements[8],  elements[9],  elements[10], elements[11] },
          { elements[12], elements[13], elements[14], elements[15] }
    }
    {}

    /**
     * @brief Construct a matrix from a 16-element array in column-major order (compile-time size check).
     *
     * @param elements Array of 16 T values in column-major layout.
     */
    explicit Matrix4x4(const T (&elements)[16]) noexcept
      : Matrix4x4(static_cast<const T*>(elements))
    {}

    /**
     * @brief Construct a rotation matrix from quaternion.
     * @param quat Rotation quaternion.
     */
    Matrix4x4(const Quaternion3<T>& quat)
    {
        makeRotation(quat);
    }

    /**
     * @brief Construct a matrix from basis vectors and origin.
     * @param origin Coordinate system origin.
     * @param x_axis X-axis direction.
     * @param y_axis Y-axis direction.
     * @param z_axis Z-axis direction.
     */
    Matrix4x4(const Point3<T>& origin, const Vector3<T>& x_axis, const Vector3<T>& y_axis, const Vector3<T>& z_axis)
    {
        setBasis(origin, x_axis, y_axis, z_axis);
    }

    /**
     * @brief Construct a matrix from a Transform3 object.
     * @param transform Transform3 object containing translation and rotation.
     */
    Matrix4x4(const Transform3<T>& transform)
    {
        setBasis(transform.translation, transform.rotation.vec0, transform.rotation.vec1, transform.rotation.vec2);
    }

  public:
    /**
     * @brief Reset this matrix to identity.
     */
    constexpr void makeIdentity() noexcept
    {
        // std::fill((T*)vecs, ((T*)vecs)+16, 0.0);
        // std::memset(data, 0, sizeof(data));
        // vec0.x = vec1.y = vec2.z = vec3.w = T(1);
        data[0] = T(1);
        data[1] = T(0);
        data[2] = T(0);
        data[3] = T(0);

        data[4] = T(0);
        data[5] = T(1);
        data[6] = T(0);
        data[7] = T(0);

        data[8]  = T(0);
        data[9]  = T(0);
        data[10] = T(1);
        data[11] = T(0);

        data[12] = T(0);
        data[13] = T(0);
        data[14] = T(0);
        data[15] = T(1);
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
    void makeRotation(const Quaternion3<T>& quat);

    /**
     * @brief Build translation matrix from offset vector.
     * @param offset Translation offset.
     */
    constexpr void makeTranslation(const Vector3<T>& offset) noexcept
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
    constexpr void makeTranslation(T x, T y, T z) noexcept
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
    constexpr void makeScale(const Vector3<T>& vec) noexcept
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
    constexpr void makeScale(T x, T y, T z) noexcept
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
    constexpr void makeScale(T factor) noexcept
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
     * @param up Up direction reference (does not need to be orthogonal).
     * @note Uses backward-axis convention: backward = eye - target. Produces a
     *       view matrix suitable for right-handed coordinates where the camera
     *       looks along the negative Z direction in view space.
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
     * @brief Make a perspective projection matrix.
     * @param fovy the vertical field of view angle in radians.
     * @param aspect_ratio the viewport aspect ratio (width / height).
     * @param z_near the near clipping plane (positive, > 0).
     * @param z_far the far clipping plane.
     *
     * @note Produces a right-handed projection matrix compatible with column-major
     *       conventions used throughout this class. NDC conventions follow the
     *       framework's existing usage (check consumer code if unsure).
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
     * @brief Left-multiply this matrix: M := left * M.
     *
     * The incoming transform is applied in world space (before the existing
     * transform), equivalent to transforming around a fixed/world axis.
     *
     * @param left The transform to apply on the left side.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& preMulti(const Matrix4x4<T>& left);
    /**
     * @brief Right-multiply this matrix: M := M * right.
     *
     * The incoming transform is applied in local space (after the existing
     * transform), equivalent to transforming around a local/moving axis.
     *
     * @param right The transform to apply on the right side.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& postMulti(const Matrix4x4<T>& right);
    /**
     * @brief Apply an axis-angle rotation in world space: M := R * M.
     *
     * Equivalent to preMulti() with a pure rotation matrix.
     *
     * @param axis   Rotation axis (world-space).
     * @param angle  Rotation angle in radians.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& preRotate(const Vector3<T>& axis, T angle);
    /**
     * @brief Apply an axis-angle rotation in local space: M := M * R.
     *
     * Equivalent to postMulti() with a pure rotation matrix.
     *
     * @param axis   Rotation axis (local-space).
     * @param angle  Rotation angle in radians.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& postRotate(const Vector3<T>& axis, T angle);
    /**
     * @brief Apply a quaternion rotation in world space: M := R * M.
     *
     * Equivalent to preMulti() with a pure rotation matrix.
     *
     * @param quat Rotation quaternion.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& preRotate(const Quaternion3<T>& quat);
    /**
     * @brief Apply a quaternion rotation in local space: M := M * R.
     *
     * Equivalent to postMulti() with a pure rotation matrix.
     *
     * @param quat Rotation quaternion.
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& postRotate(const Quaternion3<T>& quat);
    /**
     * @brief Apply a translation in world space: M := T * M.
     *
     * Equivalent to preMulti() with a pure translation matrix.
     *
     * @param offset Translation offset (world-space).
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& preTranslate(const Vector3<T>& offset);
    /**
     * @brief Apply a translation in local space: M := M * T.
     *
     * Equivalent to postMulti() with a pure translation matrix.
     *
     * @param offset Translation offset (local-space).
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& postTranslate(const Vector3<T>& offset);
    /**
     * @brief Apply a non-uniform scale in world space: M := S * M.
     *
     * Equivalent to preMulti() with a pure scale matrix.
     *
     * @param factor Scale factor per axis (world-space).
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& preScale(const Vector3<T>& factor);
    /**
     * @brief Apply a non-uniform scale in local space: M := M * S.
     *
     * Equivalent to postMulti() with a pure scale matrix.
     *
     * @param factor Scale factor per axis (local-space).
     * @return Reference to this matrix.
     */
    Matrix4x4<T>& postScale(const Vector3<T>& factor);

    /**
     * @brief Calculate the determinant of this 4x4 matrix.
     *
     * For affine matrices (last row = [0 0 0 1]), this equals the
     * determinant of the upper-left 3x3 submatrix, indicating the
     * linear part's orientation and volume scaling:
     *
     * - det > 0: orientation-preserving (no reflection).
     * - det < 0: orientation-reversing (reflection / mirror flip).
     * - det = 0: singular — the linear part loses dimension and is non-invertible.
     *
     * For projection matrices, the determinant still indicates invertibility,
     * but is generally not interpreted as a direct geometric volume scaling factor.
     *
     * For other matrices, the determinant is still mathematically defined
     * and indicates whether the matrix is invertible, but may not have
     * a direct geometric interpretation.
     *
     * @return Determinant value.
     */
    T determinant() const;

    /**
     * @brief Transpose the matrix.
     */
    constexpr void transpose()
    {
        std::swap(vecs[0][1], vecs[1][0]);
        std::swap(vecs[0][2], vecs[2][0]);
        std::swap(vecs[0][3], vecs[3][0]);
        std::swap(vecs[1][2], vecs[2][1]);
        std::swap(vecs[1][3], vecs[3][1]);
        std::swap(vecs[2][3], vecs[3][2]);
    }

    /**
     * @brief Return a transposed copy without modifying the original matrix.
     * @return Transposed matrix.
     */
    constexpr Matrix4x4<T> transposed() const
    {
        Matrix4x4<T> m(*this);
        m.transpose();
        return m;
    }

    /**
     * @brief Invert the matrix in place.
     *
     * Uses the adjugate method: M⁻¹ = adj(M) / det(M).
     * Inverse exists only when the determinant is non-zero (det(M) != 0).
     *
     * Typical invertible cases:
     * - Rigid transforms (rotation + translation).
     * - Affine transforms with non-zero scale on all axes.
     * - Basis matrix with linearly independent x/y/z axes.
     *
     * Typical non-invertible cases:
     * - Any axis scale is zero (matrix squashes dimension).
     * - Basis axes are linearly dependent (determinant becomes zero).
     * - Any transform that collapses 3D space into lower dimension.
     *
     * @note For singular matrices (det(M) == 0), the matrix is left unchanged.
     * @return true if inversion succeeded (det != 0), false for singular matrices.
     */
    bool invert();

    /**
     * @brief Return an inverted copy without modifying the original matrix.
     *
     * If the matrix is invertible (det != 0), returns the inverse.
     * If the matrix is singular (det == 0), returns a copy of the original matrix unchanged.
     *
     * @note This method cannot report failure. If you need to detect singular matrices,
     *       either check determinant() beforehand or use invert() which returns a bool.
     *
     * @return Inverted matrix, or a copy of the original matrix if singular.
     */
    constexpr Matrix4x4<T> inverted() const
    {
        Matrix4x4<T> m(*this);
        m.invert();
        return m;
    }

    /**
     * @brief Get the translation component of this matrix.
     * @return Vector3<T>.
     */
    constexpr Vector3<T> translation() const
    {
        return Vector3<T>(vecs[3][0], vecs[3][1], vecs[3][2]);
    }

    /**
     * @brief Get the rotation component of this matrix as a quaternion.
     *
     * Extracts rotation from the upper-left 3×3 block by normalizing each
     * column to unit length, then converting to a quaternion.
     *
     * This method assumes the matrix decomposes as scale × rotation × translation
     * (no shear, no reflection). If columns after normalization are not orthogonal,
     * the rotation is ill-defined and the method safely returns identity.
     *
     * Falls back to identity quaternion when:
     * - The matrix is not affine (e.g. projection).
     * - Any basis column has zero length.
     * - Normalized columns are not pairwise orthogonal (shear / reflection).
     *
     * @return Quaternion3<T> representing the rotation, or identity if not applicable.
     */
    Quaternion3<T> rotation() const;

    /**
     * @brief Get the non-uniform scaling factors of this matrix.
     * @return Vector3<T> where x/y/z components represent scale along respective axes.
     * @note If the matrix contains rotation, the extracted scaling factors may not be accurate.
     *       For pure scale or uniform scale, the returned vector will be correct.
     */
    constexpr Vector3<T> scaleFactors() const
    {
        return Vector3<T>(vecs[0][0], vecs[1][1], vecs[2][2]);
    }

    /**
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
