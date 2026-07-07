#pragma once

#include "math_global.hpp"

#include <cstring>

#include "Math.hpp"
#include "Point2.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"

V_MATH_NS_BEGIN

/**
 * @brief 3x3 column-major matrix used for 2D homogeneous transforms.
 *
 * Layout (visualized by rows):
 * | m00 m01 m02 |
 * | m10 m11 m12 |
 * | m20 m21 m22 |
 *
 * Column interpretation (columns contain basis vectors and translation):
 * - Column 0 = X axis (Xx, Xy, 0)
 * - Column 1 = Y axis (Yx, Yy, 0)
 * - Column 2 = Translation (Tx, Ty, 1)
 *
 * Memory order: M00, M10, M20, M01, ... (column-major contiguous)
 *
 * Conventions:
 * - Right-handed coordinate system is assumed for consistency with other
 *   matrix classes in this library. Positive rotation angles are counter-
 *   clockwise (CCW) when looking along the positive Z axis (i.e. standard
 *   mathematical convention for right-handed 2D).
 * - Matrices operate on column vectors: p' = M * p (use homogeneous
 *   coordinates for points, i.e. (x,y,1)).
 * - Rotation helper functions expect angles in radians. Example 2D rotation
 *   matrix (column-major storage, acts on column vectors):
 *
 *   R(θ) = | cosθ -sinθ 0 |
 *          | sinθ  cosθ 0 |
 *          |  0     0   1 |
 *
 * - You can detect a handedness/mirror flip by checking the determinant of
 *   the upper-left 2x2 (or full 3x3 for homogeneous): det > 0 preserves
 *   orientation, det < 0 indicates a reflection (mirror) component.
 * - This class represents 2D affine transforms when using homogeneous coords
 *   (rotation, translation, scale, shear). Use 3x3 for 2D calculations and
 *   4x4 for full 3D transforms.
 *
 * @tparam T floating-point type (typically float or double).
 */
template <typename T>
class Matrix3x3 {
  public:
    using value_type = T;

  public:
    /**
     * @brief Construct an identity matrix.
     */
    constexpr Matrix3x3() noexcept
      : vecs{
          { 1, 0, 0 },
          { 0, 1, 0 },
          { 0, 0, 1 }
    }
    {}

  public:
    /**
     * @brief Reset this matrix to identity.
     */
    void makeIdentity() noexcept;

    /**
     * @brief Build a 2D rotation matrix.
     * @param angle Rotation angle in radians (CCW).
     */
    void makeRotation(T angle);

    /**
     * @brief Build translation matrix from offset vector.
     * @param offset Translation offset.
     */
    void makeTranslation(const Vector2<T>& offset) noexcept;

    /**
     * @brief Build translation matrix from components.
     * @param x Translation along X axis.
     * @param y Translation along Y axis.
     */
    void makeTranslation(T x, T y) noexcept;

    /**
     * @brief Build non-uniform scale matrix from vector.
     * @param vec Scale factors for x/y.
     */
    void makeScale(const Vector2<T>& vec) noexcept;

    /**
     * @brief Build non-uniform scale matrix from components.
     * @param x Scale factor along X axis.
     * @param y Scale factor along Y axis.
     */
    void makeScale(T x, T y) noexcept;

    /**
     * @brief Build uniform scale matrix.
     * @param factor Uniform scale factor.
     */
    void makeScale(T factor) noexcept;

    /**
     * @brief Left-multiply this matrix by another matrix.
     * @param left Left matrix in M := left * M.
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& preMulti(const Matrix3x3<T>& left);

    /**
     * @brief Right-multiply this matrix by another matrix.
     * @param right Right matrix in M := M * right.
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& postMulti(const Matrix3x3<T>& right);

    /**
     * @brief Prepend a 2D rotation.
     * @param angle Rotation angle in radians (CCW).
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& preRotate(T angle);

    /**
     * @brief Append a 2D rotation.
     * @param angle Rotation angle in radians (CCW).
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& postRotate(T angle);

    /**
     * @brief Prepend a translation.
     * @param offset Translation offset.
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& preTranslate(const Vector2<T>& offset);

    /**
     * @brief Append a translation.
     * @param offset Translation offset.
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& postTranslate(const Vector2<T>& offset);

    /**
     * @brief Prepend a non-uniform scale.
     * @param factor Scale factor per axis.
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& preScale(const Vector2<T>& factor);

    /**
     * @brief Append a non-uniform scale.
     * @param factor Scale factor per axis.
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& postScale(const Vector2<T>& factor);

    /**
     * @brief Calculate the determinant of the matrix.
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
        std::swap(vecs[1][2], vecs[2][1]);
    }

    /**
     * @brief Return the transposed matrix without modifying the original one.
     */
    constexpr Matrix3x3<T> transposed() const
    {
        Matrix3x3<T> m(*this);
        m.transpose();
        return m;
    }

    /**
     * @brief Invert the matrix.
     * @note For singular matrices (det == 0), the matrix is left unchanged.
     */
    void invert();

    /**
     * @brief Return the inverted matrix without modifying the original one.
     */
    constexpr Matrix3x3<T> inverted() const
    {
        Matrix3x3<T> m(*this);
        m.invert();
        return m;
    }

    /**
     * @brief Get the translation component of this matrix.
     * @return Vector2<T>.
     */
    constexpr Vector2<T> translation() const
    {
        return Vector2<T>(vecs[2][0], vecs[2][1]);
    }

    /**
     * @brief Get the 2D rotation angle from this matrix.
     * @return Rotation angle in radians.
     */
    T rotation() const;

    /**
     * @brief Get the non-uniform scaling factors of this matrix.
     * @return Vector2<T> where x/y components represent scale along respective axes.
     * @note If the matrix contains rotation, the extracted scaling factors may not be accurate.
     */
    constexpr Vector2<T> scaleFactors() const
    {
        return Vector2<T>(vecs[0][0], vecs[1][1]);
    }

    /**
     * @brief Is this matrix an identity matrix.
     * @param eps tolerance for floating-point comparisons.
     */
    bool isIdentity(T eps = EPS<T>()) const;

    /**
     * @brief Is this matrix an affine transformation matrix (last row is [0 0 1]).
     * @param eps Tolerance for floating-point comparisons.
     */
    constexpr bool isAffine(T eps = EPS<T>()) const
    {
        return math::isZero(vecs[0][2], eps) && math::isZero(vecs[1][2], eps) && math::isEqual(vecs[2][2], T(1), eps);
    }

    /**
     * @brief Is this matrix a rigid transformation matrix (only rotation and translation).
     * @param eps Tolerance for floating-point comparisons.
     */
    bool isRigid(T eps = EPS<T>()) const;

    /**
     * @brief Is this matrix approximately equal to another matrix within a certain tolerance.
     * @param other The matrix to compare with.
     * @param eps Tolerance for floating-point comparisons.
     */
    bool isEqual(const Matrix3x3<T>& other, T eps = EPS<T>()) const;

    /**
     * @brief Check if all elements of the matrix are zero within a certain tolerance.
     * @param eps tolerance for floating-point comparisons.
     */
    bool isZero(T eps = EPS<T>()) const;

  public:
    /**
     * @brief Read matrix element by row and column.
     * @param row Row index [0, 2].
     * @param col Column index [0, 2].
     * @return Element value at (row, col).
     */
    [[nodiscard]]
    T operator()(int row, int col) const
    {
        return vecs[col][row];
    }

    /**
     * @brief Access matrix element by row and column.
     * @param row Row index [0, 2].
     * @param col Column index [0, 2].
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
    Matrix3x3<T> operator*(const Matrix3x3<T>& right) const;

    /**
     * @brief Matrix multiplication assignment.
     * @param right Right-hand matrix.
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& operator*=(const Matrix3x3<T>& right);

  public:
    /**
     * @brief Create a 2D rotation matrix.
     * @param angle Rotation angle in radians (CCW).
     * @return Rotation matrix.
     */
    [[nodiscard]]
    static Matrix3x3<T> rotate(T angle);

    /**
     * @brief Create a translation matrix from vector.
     * @param offset Translation offset.
     * @return Translation matrix.
     */
    [[nodiscard]]
    static Matrix3x3<T> translate(const Vector2<T>& offset);

    /**
     * @brief Create a translation matrix from components.
     * @param x Translation along X axis.
     * @param y Translation along Y axis.
     * @return Translation matrix.
     */
    [[nodiscard]]
    static Matrix3x3<T> translate(T x, T y);

    /**
     * @brief Create a non-uniform scale matrix from vector.
     * @param vec Scale factors for x/y.
     * @return Scale matrix.
     */
    [[nodiscard]]
    static Matrix3x3<T> scale(const Vector2<T>& vec);

    /**
     * @brief Create a non-uniform scale matrix from components.
     * @param x Scale factor along X axis.
     * @param y Scale factor along Y axis.
     * @return Scale matrix.
     */
    [[nodiscard]]
    static Matrix3x3<T> scale(T x, T y);

    /**
     * @brief Create a uniform scale matrix.
     * @param factor Uniform scale factor.
     * @return Scale matrix.
     */
    [[nodiscard]]
    static Matrix3x3<T> scale(T factor);

  public:
    union
    {
        struct {
            Vector3<T> vec0;
            Vector3<T> vec1;
            Vector3<T> vec2;
        };

        Vector3<T> vecs[3];

        T data[9];
    };
};

template <typename T>
Vector2<T> operator*(const Matrix3x3<T>& m, const Vector2<T>& v);

template <typename T>
Point2<T> operator*(const Matrix3x3<T>& m, const Point2<T>& p);

using Mat3f = Matrix3x3<float>;
using Mat3d = Matrix3x3<double>;

V_MATH_NS_END