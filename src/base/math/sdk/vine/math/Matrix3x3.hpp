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

    /**
     * @brief Construct a matrix from raw elements in row-major order.
     *
     * Parameters follow the logical matrix layout:
     * | m00 m01 m02 |
     * | m10 m11 m12 |
     * | m20 m21 m22 |
     *
     * @param _m00 Row 0, col 0. @param _m01 Row 0, col 1. @param _m02 Row 0, col 2.
     * @param _m10 Row 1, col 0. @param _m11 Row 1, col 1. @param _m12 Row 1, col 2.
     * @param _m20 Row 2, col 0. @param _m21 Row 2, col 1. @param _m22 Row 2, col 2.
     */
    constexpr Matrix3x3(T _m00, T _m01, T _m02,
                        T _m10, T _m11, T _m12,
                        T _m20, T _m21, T _m22) noexcept
      : vecs{
          { _m00, _m10, _m20 },
          { _m01, _m11, _m21 },
          { _m02, _m12, _m22 }
    }
    {}

    /**
     * @brief Construct a matrix from a 9-element array in column-major order.
     *
     * elements[0..8] match the internal data[] layout:
     * col0: elements[0..2], col1: elements[3..5], col2: elements[6..8].
     *
     * @warning The pointer is not validated. Passing nullptr or fewer than
     *          9 elements results in undefined behavior. Prefer the array-
     *          reference overload `Matrix3x3(const T (&)[9])` when the
     *          array size is known at compile time.
     *
     * @param elements Pointer to 9 T values in column-major layout.
     */
    explicit Matrix3x3(const T* elements) noexcept
      : vecs{
          { elements[0], elements[1], elements[2] },
          { elements[3], elements[4], elements[5] },
          { elements[6], elements[7], elements[8] }
    }
    {}

    /**
     * @brief Construct a matrix from a 9-element array in column-major order (compile-time size check).
     *
     * @param elements Array of 9 T values in column-major layout.
     */
    explicit Matrix3x3(const T (&elements)[9]) noexcept
      : Matrix3x3(static_cast<const T*>(elements))
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
     * @brief Left-multiply this matrix: M := left * M.
     *
     * The incoming transform is applied in world space (before the existing
     * transform), equivalent to transforming around a fixed/world axis.
     *
     * @param left The transform to apply on the left side.
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& preMulti(const Matrix3x3<T>& left);

    /**
     * @brief Right-multiply this matrix: M := M * right.
     *
     * The incoming transform is applied in local space (after the existing
     * transform), equivalent to transforming around a local/moving axis.
     *
     * @param right The transform to apply on the right side.
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& postMulti(const Matrix3x3<T>& right);

    /**
     * @brief Apply a 2D rotation in world space: M := R * M.
     *
     * Equivalent to preMulti() with a pure rotation matrix.
     *
     * @param angle Rotation angle in radians (CCW).
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& preRotate(T angle);

    /**
     * @brief Apply a 2D rotation in local space: M := M * R.
     *
     * Equivalent to postMulti() with a pure rotation matrix.
     *
     * @param angle Rotation angle in radians (CCW).
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& postRotate(T angle);

    /**
     * @brief Apply a translation in world space: M := T * M.
     *
     * Equivalent to preMulti() with a pure translation matrix.
     *
     * @param offset Translation offset (world-space).
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& preTranslate(const Vector2<T>& offset);

    /**
     * @brief Apply a translation in local space: M := M * T.
     *
     * Equivalent to postMulti() with a pure translation matrix.
     *
     * @param offset Translation offset (local-space).
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& postTranslate(const Vector2<T>& offset);

    /**
     * @brief Apply a non-uniform scale in world space: M := S * M.
     *
     * Equivalent to preMulti() with a pure scale matrix.
     *
     * @param factor Scale factor per axis (world-space).
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& preScale(const Vector2<T>& factor);

    /**
     * @brief Apply a non-uniform scale in local space: M := M * S.
     *
     * Equivalent to postMulti() with a pure scale matrix.
     *
     * @param factor Scale factor per axis (local-space).
     * @return Reference to this matrix.
     */
    Matrix3x3<T>& postScale(const Vector2<T>& factor);

    /**
     * @brief Calculate the determinant of this 3x3 homogeneous matrix.
     *
     * For an affine matrix (last row = [0 0 1]), this equals the
     * determinant of the upper-left 2x2 submatrix, which reveals the
     * linear part's orientation and scaling behavior:
     *
     * - det > 0: orientation-preserving (e.g. rotation, uniform scale).
     * - det < 0: orientation-reversing (contains a reflection / mirror).
     * - det = 0: singular — at least one axis has collapsed (e.g. a
     *   zero-scale axis), making the matrix non-invertible.
     *
     * For general 3x3 (non-affine), the full determinant is returned.
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
        std::swap(vecs[1][2], vecs[2][1]);
    }

    /**
     * @brief Return a transposed copy without modifying the original matrix.
     * @return Transposed matrix.
     */
    constexpr Matrix3x3<T> transposed() const
    {
        Matrix3x3<T> m(*this);
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
     * - Basis matrix with linearly independent x/y axes.
     *
     * Typical non-invertible cases:
     * - Any axis scale is zero (matrix squashes to a line or point).
     * - Basis axes are linearly dependent (det == 0).
     * - Any transform that collapses 2D space into 1D or 0D.
     *
     * @note For singular matrices (det(M) == 0), the matrix is left unchanged.
     */
    void invert();

    /**
     * @brief Return an inverted copy without modifying the original matrix.
     * @return Inverted matrix.
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
    bool isIdentity(T eps = EPS<T>()) const{
        return math::isEqual(vecs[0][0], T(1), eps) && math::isZero(vecs[0][1], eps) && math::isZero(vecs[0][2], eps)
         && math::isZero(vecs[1][0], eps) && math::isEqual(vecs[1][1], T(1), eps) && math::isZero(vecs[1][2], eps) 
         && math::isZero(vecs[2][0], eps) && math::isZero(vecs[2][1], eps) && math::isEqual(vecs[2][2], T(1), eps);
    }

    /**
     * @brief Is this matrix an affine transformation matrix (last row is [0 0 1]).
     *
     * Affine matrices preserve the parallelism of straight lines and include
     * translation, scaling, rotation, shearing, and reflection. Non-affine
     * matrices include those where the last row deviates from [0 0 1], such
     * as some projection matrices.
     *
     * @param eps Tolerance for floating-point comparisons.
     * @return True if the matrix is affine, false otherwise.
     */
    constexpr bool isAffine(T eps = EPS<T>()) const
    {
        return math::isZero(vecs[0][2], eps) && math::isZero(vecs[1][2], eps) && math::isEqual(vecs[2][2], T(1), eps);
    }

    /**
     * @brief Is this matrix a rigid transformation matrix (only rotation and translation,
     *        no scaling, shearing, or reflection).
     * @param eps Tolerance for floating-point comparisons.
     * @return True if the matrix is rigid, false otherwise.
     */
    bool isRigid(T eps = EPS<T>()) const;

    /**
     * @brief Is this matrix approximately equal to another matrix within a certain tolerance (epsilon).
     * @param other The matrix to compare with.
     * @param eps Tolerance for floating-point comparisons.
     * @return True if the matrices are approximately equal, false otherwise.
     */
    bool isEqual(const Matrix3x3<T>& other, T eps = EPS<T>()) const;

    /**
     * @brief Check if all elements of the matrix are zero within a certain tolerance (epsilon).
     * @param eps tolerance for floating-point comparisons.
     * @return true if all elements are approximately zero, false otherwise.
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