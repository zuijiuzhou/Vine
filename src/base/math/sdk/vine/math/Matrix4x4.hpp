#pragma once

#include "math_global.hpp"

#include <cstring>

#include "Vector4.hpp"

V_MATH_NS_BEGIN

/**
 * @brief 4x4 column-major matrix.
 *
 * Layout (visualized by rows):
 * | m00 m01 m02 m03 |
 * | m10 m11 m12 m13 |
 * | m20 m21 m22 m23 |
 * | m30 m31 m32 m33 |
 *
 * Memory order: M00, M10, M20, M30, M01, ... (column-major contiguous).
 * Matrices operate on column vectors: y = M * x.
 *
 * Geometric transforms (rotation, translation, scale, projection, etc.)
 * are provided as free functions in Transform3.hpp.
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
        assert(row < 4);
        assert(col < 4);
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
        assert(row < 4);
        assert(col < 4);
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

using Mat4f = Matrix4x4<float>;
using Mat4d = Matrix4x4<double>;

V_MATH_NS_END
