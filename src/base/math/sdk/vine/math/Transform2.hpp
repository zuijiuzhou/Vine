#pragma once

#include "math_global.hpp"

#include "Matrix3x3.hpp"
#include "Point2.hpp"
#include "Vector2.hpp"

V_MATH_NS_BEGIN

/**
 * @brief 2D geometric transformation free functions.
 *
 * All functions operate on Matrix3x3<T> (3×3 homogeneous 2D transforms)
 * and follow column-major, column-vector-left-multiply convention.
 * Right-handed coordinate system; CCW rotation convention.
 */

/* ========================================================================= */
/*  1. Build  –  fill a matrix in-place                                      */
/* ========================================================================= */

/* ---- translation ---- */

/**
 * @brief Build translation matrix from offset vector.
 * @param m      Target matrix (overwritten).
 * @param offset Translation offset.
 */
template <typename T>
inline void makeTranslation(Matrix3x3<T>& m, const Vector2<T>& offset) noexcept
{
    m.makeIdentity();
    m.vecs[2][0] = offset.x;
    m.vecs[2][1] = offset.y;
}

/**
 * @brief Build translation matrix from components.
 * @param m Target matrix (overwritten).
 * @param x Translation along X axis.
 * @param y Translation along Y axis.
 */
template <typename T>
inline void makeTranslation(Matrix3x3<T>& m, T x, T y) noexcept
{
    m.makeIdentity();
    m.vecs[2][0] = x;
    m.vecs[2][1] = y;
}

/* ---- rotation ---- */

/**
 * @brief Build 2D rotation matrix (CCW).
 * @param m     Target matrix (overwritten).
 * @param angle Rotation angle in radians.
 */
template <typename T>
inline void makeRotation(Matrix3x3<T>& m, T angle)
{
    const auto c = std::cos(angle);
    const auto s = std::sin(angle);

    m.vecs[0][0] = c;
    m.vecs[0][1] = s;
    m.vecs[0][2] = T(0);

    m.vecs[1][0] = -s;
    m.vecs[1][1] = c;
    m.vecs[1][2] = T(0);

    m.vecs[2][0] = T(0);
    m.vecs[2][1] = T(0);
    m.vecs[2][2] = T(1);
}

/* ---- scale ---- */

/**
 * @brief Build non-uniform scale matrix from vector.
 * @param m   Target matrix (overwritten).
 * @param vec Scale factors for x/y.
 */
template <typename T>
inline void makeScale(Matrix3x3<T>& m, const Vector2<T>& vec) noexcept
{
    m.makeIdentity();
    m.vecs[0][0] = vec.x;
    m.vecs[1][1] = vec.y;
}

/**
 * @brief Build non-uniform scale matrix from components.
 * @param m Target matrix (overwritten).
 * @param x Scale factor along X axis.
 * @param y Scale factor along Y axis.
 */
template <typename T>
inline void makeScale(Matrix3x3<T>& m, T x, T y) noexcept
{
    m.makeIdentity();
    m.vecs[0][0] = x;
    m.vecs[1][1] = y;
}

/**
 * @brief Build uniform scale matrix.
 * @param m      Target matrix (overwritten).
 * @param factor Uniform scale factor.
 */
template <typename T>
inline void makeScale(Matrix3x3<T>& m, T factor) noexcept
{
    m.makeIdentity();
    m.vecs[0][0] = factor;
    m.vecs[1][1] = factor;
}

/* ========================================================================= */
/*  2. Get  –  extract properties from a matrix                              */
/* ========================================================================= */

/**
 * @brief Get the translation vector from column 2.
 * @param m Transform matrix.
 * @return Translation as Vector2<T>.
 */
template <typename T>
constexpr Vector2<T> getTranslation(const Matrix3x3<T>& m) noexcept
{
    return Vector2<T>(m.vecs[2][0], m.vecs[2][1]);
}

/**
 * @brief Get the 2D rotation angle from the upper-left 2×2 block.
 * @param m Transform matrix.
 * @return Rotation angle in radians (CCW).
 */
template <typename T>
inline T getRotation(const Matrix3x3<T>& m)
{
    return std::atan2(m.vecs[0][1], m.vecs[0][0]);
}

/**
 * @brief Get the diagonal of the upper-left 2×2 block.
 * @param m Transform matrix.
 * @return (m00, m11) as Vector2<T>.
 */
template <typename T>
constexpr Vector2<T> getScale(const Matrix3x3<T>& m) noexcept
{
    return Vector2<T>(m.vecs[0][0], m.vecs[1][1]);
}

/* ========================================================================= */
/*  3. Modify  –  pre/post-multiply in-place                                 */
/* ========================================================================= */

/**
 * @brief Apply a 2D rotation in world space: M := R * M.
 * @param m     Target matrix (modified in-place).
 * @param angle Rotation angle in radians (CCW).
 * @return Reference to m.
 */
template <typename T>
inline Matrix3x3<T>& preRotate(Matrix3x3<T>& m, T angle)
{
    Matrix3x3<T> rot;
    makeRotation(rot, angle);
    return m.preMulti(rot);
}

/**
 * @brief Apply a 2D rotation in local space: M := M * R.
 * @param m     Target matrix (modified in-place).
 * @param angle Rotation angle in radians (CCW).
 * @return Reference to m.
 */
template <typename T>
inline Matrix3x3<T>& postRotate(Matrix3x3<T>& m, T angle)
{
    Matrix3x3<T> rot;
    makeRotation(rot, angle);
    return m.postMulti(rot);
}

/**
 * @brief Apply a translation in world space: M := T * M.
 * @param m      Target matrix (modified in-place).
 * @param offset Translation offset (world-space).
 * @return Reference to m.
 */
template <typename T>
inline Matrix3x3<T>& preTranslate(Matrix3x3<T>& m, const Vector2<T>& offset)
{
    const auto tx = offset.x, ty = offset.y;
    for (size_t col = 0; col < 3; ++col) {
        const auto w = m.vecs[col][2];
        m.vecs[col][0] += tx * w;
        m.vecs[col][1] += ty * w;
    }
    return m;
}

/**
 * @brief Apply a translation in local space: M := M * T.
 * @param m      Target matrix (modified in-place).
 * @param offset Translation offset (local-space).
 * @return Reference to m.
 */
template <typename T>
inline Matrix3x3<T>& postTranslate(Matrix3x3<T>& m, const Vector2<T>& offset)
{
    const auto tx = offset.x, ty = offset.y;
    for (size_t row = 0; row < 3; ++row) m.vecs[2][row] += m.vecs[0][row] * tx + m.vecs[1][row] * ty;
    return m;
}

/**
 * @brief Apply a non-uniform scale in world space: M := S * M.
 * @param m      Target matrix (modified in-place).
 * @param factor Scale factor per axis (world-space).
 * @return Reference to m.
 */
template <typename T>
inline Matrix3x3<T>& preScale(Matrix3x3<T>& m, const Vector2<T>& factor)
{
    const auto sx = factor.x, sy = factor.y;
    for (size_t col = 0; col < 3; ++col) {
        m.vecs[col][0] *= sx;
        m.vecs[col][1] *= sy;
    }
    return m;
}

/**
 * @brief Apply a non-uniform scale in local space: M := M * S.
 * @param m      Target matrix (modified in-place).
 * @param factor Scale factor per axis (local-space).
 * @return Reference to m.
 */
template <typename T>
inline Matrix3x3<T>& postScale(Matrix3x3<T>& m, const Vector2<T>& factor)
{
    const auto sx = factor.x, sy = factor.y;
    for (size_t row = 0; row < 3; ++row) {
        m.vecs[0][row] *= sx;
        m.vecs[1][row] *= sy;
    }
    return m;
}

/* ========================================================================= */
/*  4. Apply  –  transform points and vectors                                */
/* ========================================================================= */

/**
 * @brief Transform a direction vector (w = 0, translation ignored).
 * @param m Transform matrix.
 * @param v Input vector.
 * @return Transformed vector.
 */
template <typename T>
inline Vector2<T> operator*(const Matrix3x3<T>& m, const Vector2<T>& v)
{
    return Vector2<T>(m.vecs[0][0] * v.x + m.vecs[1][0] * v.y, m.vecs[0][1] * v.x + m.vecs[1][1] * v.y);
}

/**
 * @brief Transform a point (w = 1, full affine with homogeneous divide).
 * @param m Transform matrix.
 * @param p Input point.
 * @return Transformed point.
 */
template <typename T>
inline Point2<T> operator*(const Matrix3x3<T>& m, const Point2<T>& p)
{
    const auto x = m.vecs[0][0] * p.x + m.vecs[1][0] * p.y + m.vecs[2][0];
    const auto y = m.vecs[0][1] * p.x + m.vecs[1][1] * p.y + m.vecs[2][1];
    const auto w = m.vecs[0][2] * p.x + m.vecs[1][2] * p.y + m.vecs[2][2];

    if (math::isEqual(w, T(1), T(1e-12)))
        return Point2<T>(x, y);
    if (math::isZero(w, T(1e-12)))
        return Point2<T>(x, y);
    return Point2<T>(x / w, y / w);
}

/* ========================================================================= */
/*  5. Create  –  factory functions (return Matrix3x3 by value)              */
/* ========================================================================= */

/**
 * @brief Create a 2D rotation matrix.
 * @param angle Rotation angle in radians (CCW).
 * @return Rotation matrix.
 */
template <typename T>
[[nodiscard]] inline Matrix3x3<T> rotate(T angle)
{
    Matrix3x3<T> m;
    makeRotation(m, angle);
    return m;
}

/**
 * @brief Create a translation matrix from vector.
 * @param offset Translation offset.
 * @return Translation matrix.
 */
template <typename T>
[[nodiscard]] inline Matrix3x3<T> translate(const Vector2<T>& offset)
{
    Matrix3x3<T> m;
    makeTranslation(m, offset);
    return m;
}

/**
 * @brief Create a translation matrix from components.
 * @param x Translation along X axis.
 * @param y Translation along Y axis.
 * @return Translation matrix.
 */
template <typename T>
[[nodiscard]] inline Matrix3x3<T> translate(T x, T y)
{
    Matrix3x3<T> m;
    makeTranslation(m, x, y);
    return m;
}

/**
 * @brief Create a non-uniform scale matrix from vector.
 * @param vec Scale factors for x/y.
 * @return Scale matrix.
 */
template <typename T>
[[nodiscard]] inline Matrix3x3<T> scale(const Vector2<T>& vec)
{
    Matrix3x3<T> m;
    makeScale(m, vec);
    return m;
}

/**
 * @brief Create a non-uniform scale matrix from components.
 * @param x Scale factor along X axis.
 * @param y Scale factor along Y axis.
 * @return Scale matrix.
 */
template <typename T>
[[nodiscard]] inline Matrix3x3<T> scale(T x, T y)
{
    Matrix3x3<T> m;
    makeScale(m, x, y);
    return m;
}

/**
 * @brief Create a uniform scale matrix.
 * @param factor Uniform scale factor.
 * @return Scale matrix.
 */
template <typename T>
[[nodiscard]] inline Matrix3x3<T> scale(T factor)
{
    Matrix3x3<T> m;
    makeScale(m, factor);
    return m;
}

V_MATH_NS_END
