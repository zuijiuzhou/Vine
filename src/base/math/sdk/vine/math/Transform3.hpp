#pragma once

#include "math_global.hpp"

#include <cstring>

#include "Isometry3.hpp"
#include "Matrix3x3.hpp"
#include "Matrix4x4.hpp"
#include "Point3.hpp"
#include "Quaternion.hpp"
#include "Vector3.hpp"

V_MATH_NS_BEGIN
/**
 * @brief 3D geometric transformation free functions.
 *
 * All functions operate on Matrix4x4<T> and follow the standard
 * column-major, column-vector-left-multiply convention:
 *
 *   point'  = M * point    (homogeneous w = 1)
 *   vector' = M * vector   (homogeneous w = 0)
 *
 * Right-handed coordinate system throughout.
 *
 * These functions were extracted from Matrix4x4 member methods to keep
 * Matrix4x4 a pure linear-algebra type.  Include this header when you
 * need rotation, translation, scale, look-at, projection, or point/vector
 * transformation helpers.
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
constexpr void makeTranslation(Matrix4x4<T>& m, const Vector3<T>& offset) noexcept;

/**
 * @brief Build translation matrix from components.
 * @param m Target matrix (overwritten).
 * @param x Translation along X axis.
 * @param y Translation along Y axis.
 * @param z Translation along Z axis.
 */
template <typename T>
constexpr void makeTranslation(Matrix4x4<T>& m, T x, T y, T z) noexcept;

/* ---- rotation ---- */

/**
 * @brief Build rotation matrix from quaternion.
 * @param m    Target matrix (overwritten).
 * @param quat Rotation quaternion.
 */
template <typename T>
void makeRotation(Matrix4x4<T>& m, const Quaternion<T>& quat);

/**
 * @brief Build axis-angle rotation matrix.
 * @param m     Target matrix (overwritten).
 * @param axis  Rotation axis.
 * @param angle Rotation angle in radians.
 */
template <typename T>
void makeRotation(Matrix4x4<T>& m, const Vector3<T>& axis, T angle);

/**
 * @brief Build rotation from start vector to end vector.
 * @param m     Target matrix (overwritten).
 * @param start Source direction vector.
 * @param end   Target direction vector.
 */
template <typename T>
void makeRotation(Matrix4x4<T>& m, const Vector3<T>& start, const Vector3<T>& end);

/* ---- scale ---- */

/**
 * @brief Build non-uniform scale matrix from vector.
 * @param m   Target matrix (overwritten).
 * @param vec Scale factors for x/y/z.
 */
template <typename T>
constexpr void makeScale(Matrix4x4<T>& m, const Vector3<T>& vec) noexcept;

/**
 * @brief Build non-uniform scale matrix from components.
 * @param m Target matrix (overwritten).
 * @param x Scale factor along X axis.
 * @param y Scale factor along Y axis.
 * @param z Scale factor along Z axis.
 */
template <typename T>
constexpr void makeScale(Matrix4x4<T>& m, T x, T y, T z) noexcept;

/**
 * @brief Build uniform scale matrix.
 * @param m      Target matrix (overwritten).
 * @param factor Uniform scale factor.
 */
template <typename T>
constexpr void makeScale(Matrix4x4<T>& m, T factor) noexcept;

/* ---- look-at ---- */

/**
 * @brief Build a look-at view matrix.
 * @param m      Target matrix (overwritten).
 * @param eye    Camera position.
 * @param target Camera target point.
 * @param up     Up direction reference (does not need to be orthogonal).
 * @note Uses backward-axis convention: backward = eye - target. Produces a
 *       view matrix suitable for right-handed coordinates where the camera
 *       looks along the negative Z direction in view space.
 */
template <typename T>
void makeLookAt(Matrix4x4<T>& m, const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up);

/* ---- projection ---- */

/**
 * @brief Make an orthographic projection matrix.
 * @param m      Target matrix (overwritten).
 * @param left   the left clipping plane.
 * @param right  the right clipping plane.
 * @param bottom the bottom clipping plane.
 * @param top    the top clipping plane.
 * @param z_near the near clipping plane.
 * @param z_far  the far clipping plane.
 */
template <typename T>
void makeOrtho(Matrix4x4<T>& m, double left, double right, double bottom, double top, double z_near, double z_far);

/**
 * @brief Make a perspective projection matrix.
 * @param m            Target matrix (overwritten).
 * @param fovy         the vertical field of view angle in radians.
 * @param aspect_ratio the viewport aspect ratio (width / height).
 * @param z_near       the near clipping plane (positive, > 0).
 * @param z_far        the far clipping plane.
 *
 * @note Produces a right-handed projection matrix compatible with column-major
 *       conventions used throughout this class. NDC conventions follow the
 *       framework's existing usage (check consumer code if unsure).
 */
template <typename T>
void makePerspective(Matrix4x4<T>& m, double fovy, double aspect_ratio, double z_near, double z_far);

/* ---- reflection ---- */

/**
 * @brief Make a reflection matrix across a plane defined by its normal and offset.
 * @param m            Target matrix (overwritten).
 * @param plane_normal Normal vector of the mirror plane.
 * @param plane_offset Offset of the mirror plane from the origin.
 */
template <typename T>
void makeReflection(Matrix4x4<T>& m, const Vector3<T>& plane_normal, T plane_offset);

/* ---- basis ---- */

/**
 * @brief Set the coordinate system represented by this matrix.
 * @param m      Target matrix (overwritten).
 * @param origin the origin point of the coordinate system.
 * @param x_axis the x axis direction of the coordinate system.
 * @param y_axis the y axis direction of the coordinate system.
 * @param z_axis the z axis direction of the coordinate system.
 */
template <typename T>
void setBasis(Matrix4x4<T>& m, const Point3<T>& origin, const Vector3<T>& x_axis, const Vector3<T>& y_axis, const Vector3<T>& z_axis);

/* ========================================================================= */
/*  2. Get  –  extract properties from a matrix                              */
/* ========================================================================= */

/**
 * @brief Get the translation vector from column 3 of a 4x4 matrix.
 * @param m Transform matrix.
 * @return Column 3 xyz as Vector3<T>.
 */
template <typename T>
constexpr Vector3<T> getTranslation(const Matrix4x4<T>& m) noexcept;

/**
 * @brief Get the rotation quaternion from the upper-left 3x3 block.
 *
 * Normalizes columns to remove scale, then converts to quaternion.
 * Returns identity if the matrix is non-affine, has zero-length columns,
 * or has non-orthogonal columns (shear / reflection).
 *
 * @param m Transform matrix.
 * @return Rotation quaternion, or identity if ill-defined.
 */
template <typename T>
Quaternion<T> getRotation(const Matrix4x4<T>& m);

/**
 * @brief Get the diagonal of the upper-left 3x3 block.
 * @param m Transform matrix.
 * @return (m00, m11, m22) as Vector3<T>.
 */
template <typename T>
constexpr Vector3<T> getScale(const Matrix4x4<T>& m) noexcept;

/**
 * @brief Get the coordinate system represented by this matrix.
 * @param m        Source matrix.
 * @param o_origin Output origin point.
 * @param o_x_axis Output x-axis direction.
 * @param o_y_axis Output y-axis direction.
 * @param o_z_axis Output z-axis direction.
 */
template <typename T>
void getBasis(const Matrix4x4<T>& m, Point3<T>& o_origin, Vector3<T>& o_x_axis, Vector3<T>& o_y_axis, Vector3<T>& o_z_axis);

/* ========================================================================= */
/*  3. Modify  –  pre/post-multiply in-place                                 */
/* ========================================================================= */

/**
 * @brief Apply an axis-angle rotation in world space: M := R * M.
 *
 * Equivalent to preMulti() with a pure rotation matrix.
 *
 * @param m     Target matrix (modified in-place).
 * @param axis  Rotation axis (world-space).
 * @param angle Rotation angle in radians.
 * @return Reference to m.
 */
template <typename T>
Matrix4x4<T>& preRotate(Matrix4x4<T>& m, const Vector3<T>& axis, T angle);

/**
 * @brief Apply an axis-angle rotation in local space: M := M * R.
 *
 * Equivalent to postMulti() with a pure rotation matrix.
 *
 * @param m     Target matrix (modified in-place).
 * @param axis  Rotation axis (local-space).
 * @param angle Rotation angle in radians.
 * @return Reference to m.
 */
template <typename T>
Matrix4x4<T>& postRotate(Matrix4x4<T>& m, const Vector3<T>& axis, T angle);

/**
 * @brief Apply a quaternion rotation in world space: M := R * M.
 *
 * Equivalent to preMulti() with a pure rotation matrix.
 *
 * @param m    Target matrix (modified in-place).
 * @param quat Rotation quaternion.
 * @return Reference to m.
 */
template <typename T>
Matrix4x4<T>& preRotate(Matrix4x4<T>& m, const Quaternion<T>& quat);

/**
 * @brief Apply a quaternion rotation in local space: M := M * R.
 *
 * Equivalent to postMulti() with a pure rotation matrix.
 *
 * @param m    Target matrix (modified in-place).
 * @param quat Rotation quaternion.
 * @return Reference to m.
 */
template <typename T>
Matrix4x4<T>& postRotate(Matrix4x4<T>& m, const Quaternion<T>& quat);

/**
 * @brief Apply a translation in world space: M := T * M.
 *
 * Equivalent to preMulti() with a pure translation matrix.
 *
 * @param m      Target matrix (modified in-place).
 * @param offset Translation offset (world-space).
 * @return Reference to m.
 */
template <typename T>
Matrix4x4<T>& preTranslate(Matrix4x4<T>& m, const Vector3<T>& offset);

/**
 * @brief Apply a translation in local space: M := M * T.
 *
 * Equivalent to postMulti() with a pure translation matrix.
 *
 * @param m      Target matrix (modified in-place).
 * @param offset Translation offset (local-space).
 * @return Reference to m.
 */
template <typename T>
Matrix4x4<T>& postTranslate(Matrix4x4<T>& m, const Vector3<T>& offset);

/**
 * @brief Apply a non-uniform scale in world space: M := S * M.
 *
 * Equivalent to preMulti() with a pure scale matrix.
 *
 * @param m      Target matrix (modified in-place).
 * @param factor Scale factor per axis (world-space).
 * @return Reference to m.
 */
template <typename T>
Matrix4x4<T>& preScale(Matrix4x4<T>& m, const Vector3<T>& factor);

/**
 * @brief Apply a non-uniform scale in local space: M := M * S.
 *
 * Equivalent to postMulti() with a pure scale matrix.
 *
 * @param m      Target matrix (modified in-place).
 * @param factor Scale factor per axis (local-space).
 * @return Reference to m.
 */
template <typename T>
Matrix4x4<T>& postScale(Matrix4x4<T>& m, const Vector3<T>& factor);

/* ========================================================================= */
/*  4. Apply  –  transform points and vectors                                */
/* ========================================================================= */

/**
 * @brief Transform a direction vector (w = 0, translation ignored).
 *        Uses the upper-left 3x3 linear block only.
 * @param m Transform matrix.
 * @param v Input vector.
 * @return Transformed vector.
 */
template <typename T>
Vector3<T> operator*(const Matrix4x4<T>& m, const Vector3<T>& v);

/**
 * @brief Transform a point (w = 1, full affine with perspective divide).
 * @param m Transform matrix.
 * @param p Input point.
 * @return Transformed point (perspective-divided if w != 1).
 */
template <typename T>
Point3<T> operator*(const Matrix4x4<T>& m, const Point3<T>& p);

/**
 * @brief 3×3 linear transform of a vector (no translation).
 * @param m 3×3 matrix.
 * @param v Input vector.
 * @return Transformed vector.
 */
template <typename T>
inline Vector3<T> operator*(const Matrix3x3<T>& m, const Vector3<T>& v)
{
    return Vector3<T>(m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z,
                      m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z,
                      m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z);
}

/**
 * @brief 3×3 linear transform of a point (no translation).
 * @param m 3×3 matrix.
 * @param p Input point.
 * @return Transformed point.
 */
template <typename T>
inline Point3<T> operator*(const Matrix3x3<T>& m, const Point3<T>& p)
{
    return Point3<T>(m(0, 0) * p.x + m(0, 1) * p.y + m(0, 2) * p.z,
                     m(1, 0) * p.x + m(1, 1) * p.y + m(1, 2) * p.z,
                     m(2, 0) * p.x + m(2, 1) * p.y + m(2, 2) * p.z);
}

/* ========================================================================= */
/*  5. Create  –  factory functions (return Matrix4x4 by value)              */
/* ========================================================================= */

/**
 * @brief Create a rotation matrix from a unit quaternion.
 * @param quat Rotation quaternion.
 * @return Rotation matrix.
 */
template <typename T>
[[nodiscard]] Matrix4x4<T> rotate(const Quaternion<T>& quat);

/**
 * @brief Create an axis-angle rotation matrix.
 * @param axis  Rotation axis.
 * @param angle Rotation angle in radians.
 * @return Rotation matrix.
 */
template <typename T>
[[nodiscard]] Matrix4x4<T> rotate(const Vector3<T>& axis, T angle);

/**
 * @brief Create a rotation matrix from start and end vectors.
 * @param start Source direction vector.
 * @param end   Target direction vector.
 * @return Rotation matrix.
 */
template <typename T>
[[nodiscard]] Matrix4x4<T> rotate(const Vector3<T>& start, const Vector3<T>& end);

/**
 * @brief Create a translation matrix from vector.
 * @param offset Translation offset.
 * @return Translation matrix.
 */
template <typename T>
[[nodiscard]] Matrix4x4<T> translate(const Vector3<T>& offset);

/**
 * @brief Create a translation matrix from components.
 * @param x Translation along X axis.
 * @param y Translation along Y axis.
 * @param z Translation along Z axis.
 * @return Translation matrix.
 */
template <typename T>
[[nodiscard]] Matrix4x4<T> translate(T x, T y, T z);

/**
 * @brief Create a non-uniform scale matrix from vector.
 * @param vec Scale factors for x/y/z.
 * @return Scale matrix.
 */
template <typename T>
[[nodiscard]] Matrix4x4<T> scale(const Vector3<T>& vec);

/**
 * @brief Create a non-uniform scale matrix from components.
 * @param x Scale factor along X axis.
 * @param y Scale factor along Y axis.
 * @param z Scale factor along Z axis.
 * @return Scale matrix.
 */
template <typename T>
[[nodiscard]] Matrix4x4<T> scale(T x, T y, T z);

/**
 * @brief Create a uniform scale matrix.
 * @param factor Uniform scale factor.
 * @return Scale matrix.
 */
template <typename T>
[[nodiscard]] Matrix4x4<T> scale(T factor);

/**
 * @brief Create a look-at view matrix.
 * @param eye    Camera position.
 * @param target Camera target point.
 * @param up     Up direction reference.
 * @return View matrix.
 */
template <typename T>
[[nodiscard]] Matrix4x4<T> lookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up);

/**
 * @brief Create an orthographic projection matrix.
 * @param left   Left clipping plane.
 * @param right  Right clipping plane.
 * @param bottom Bottom clipping plane.
 * @param top    Top clipping plane.
 * @param z_near Near clipping plane.
 * @param z_far  Far clipping plane.
 * @return Orthographic projection matrix.
 */
template <typename T>
[[nodiscard]] Matrix4x4<T> ortho(double left, double right, double bottom, double top, double z_near, double z_far);

/**
 * @brief Create a perspective projection matrix.
 * @param fovy         Vertical field of view in radians.
 * @param aspect_ratio Viewport aspect ratio.
 * @param z_near       Near clipping plane.
 * @param z_far        Far clipping plane.
 * @return Perspective projection matrix.
 */
template <typename T>
[[nodiscard]] Matrix4x4<T> perspective(double fovy, double aspect_ratio, double z_near, double z_far);

/**
 * @brief Create a reflection matrix across a plane.
 * @param plane_normal Normal vector of the plane.
 * @param plane_offset Offset of the plane from the origin.
 * @return Reflection matrix.
 */
template <typename T>
[[nodiscard]] Matrix4x4<T> reflect(const Vector3<T>& plane_normal, T plane_offset);

/**
 * @brief Create a matrix from basis vectors and origin.
 * @param origin Coordinate system origin.
 * @param x_axis X-axis direction.
 * @param y_axis Y-axis direction.
 * @param z_axis Z-axis direction.
 * @return Transform matrix composed from basis.
 */
template <typename T>
[[nodiscard]] Matrix4x4<T> fromBasis(const Point3<T>& origin, const Vector3<T>& x_axis, const Vector3<T>& y_axis, const Vector3<T>& z_axis);

/* ========================================================================= */
/*  6. Convert  –  type conversion                                           */
/* ========================================================================= */

/**
 * @brief Convert a unit quaternion to a 3x3 rotation matrix.
 * @param quat Rotation quaternion.
 * @return 3x3 rotation matrix (upper-left block of the equivalent 4x4).
 */
template <typename T>
Matrix3x3<T> toRotationMatrix(const Quaternion<T>& quat);

/**
 * @brief Construct a 4x4 rigid transform matrix from an Isometry3 object.
 * @param tf Isometry3 object containing translation and rotation.
 * @return 4x4 rigid transform matrix.
 */
template <typename T>
Matrix4x4<T> toMatrix4x4(const Isometry3<T>& tf);

/* ========================================================================= */
/*  Inline implementations (≤ 20 lines each)                                 */
/* ========================================================================= */

/* ---- makeTranslation ---- */

template <typename T>
constexpr void makeTranslation(Matrix4x4<T>& m, const Vector3<T>& offset) noexcept
{
    m.makeIdentity();
    m.vecs[3][0] = offset.x;
    m.vecs[3][1] = offset.y;
    m.vecs[3][2] = offset.z;
}

template <typename T>
constexpr void makeTranslation(Matrix4x4<T>& m, T x, T y, T z) noexcept
{
    m.makeIdentity();
    m.vecs[3][0] = x;
    m.vecs[3][1] = y;
    m.vecs[3][2] = z;
}

/* ---- makeScale ---- */

template <typename T>
constexpr void makeScale(Matrix4x4<T>& m, const Vector3<T>& vec) noexcept
{
    m.makeIdentity();
    m.vecs[0][0] = vec.x;
    m.vecs[1][1] = vec.y;
    m.vecs[2][2] = vec.z;
}

template <typename T>
constexpr void makeScale(Matrix4x4<T>& m, T x, T y, T z) noexcept
{
    m.makeIdentity();
    m.vecs[0][0] = x;
    m.vecs[1][1] = y;
    m.vecs[2][2] = z;
}

template <typename T>
constexpr void makeScale(Matrix4x4<T>& m, T factor) noexcept
{
    m.makeIdentity();
    m.vecs[0][0] = factor;
    m.vecs[1][1] = factor;
    m.vecs[2][2] = factor;
}

/* ---- makeOrtho / makePerspective ---- */

template <typename T>
inline void makeOrtho(Matrix4x4<T>& m, double left, double right, double bottom, double top, double z_near, double z_far)
{
    auto tx = -(right + left) / (right - left);
    auto ty = -(top + bottom) / (top - bottom);
    auto tz = -(z_far + z_near) / (z_far - z_near);

    m.vecs[0] = Vector4<T>(T(2) / (right - left), T(0), T(0), T(0));
    m.vecs[1] = Vector4<T>(T(0), T(2) / (top - bottom), T(0), T(0));
    m.vecs[2] = Vector4<T>(T(0), T(0), T(-2) / (z_far - z_near), T(0));
    m.vecs[3] = Vector4<T>(tx, ty, tz, T(1));
}

template <typename T>
inline void makePerspective(Matrix4x4<T>& m, double fovy, double aspect_ratio, double z_near, double z_far)
{
    const auto f = T(1) / std::tan(fovy / 2.0);
    std::memset(m.data, 0, sizeof(m.data));
    m.vecs[0][0] = T(f / aspect_ratio);
    m.vecs[1][1] = T(f);
    m.vecs[2][2] = T((z_far + z_near) / (z_near - z_far));
    m.vecs[2][3] = T(-1);
    m.vecs[3][2] = T(2 * z_far * z_near / (z_near - z_far));
}

/* ---- setBasis / getBasis ---- */

template <typename T>
inline void setBasis(Matrix4x4<T>& m, const Point3<T>& origin, const Vector3<T>& x_axis, const Vector3<T>& y_axis, const Vector3<T>& z_axis)
{
    m.vecs[0].set(x_axis, T(0));
    m.vecs[1].set(y_axis, T(0));
    m.vecs[2].set(z_axis, T(0));
    m.vecs[3].set(origin.x, origin.y, origin.z, T(1));
}

template <typename T>
inline void getBasis(const Matrix4x4<T>& m, Point3<T>& o_origin, Vector3<T>& o_x_axis, Vector3<T>& o_y_axis, Vector3<T>& o_z_axis)
{
    o_origin = m.vecs[3].asVector3().asPoint();
    o_x_axis = m.vecs[0].asVector3();
    o_y_axis = m.vecs[1].asVector3();
    o_z_axis = m.vecs[2].asVector3();
}

/* ---- getTranslation / getScale ---- */

template <typename T>
constexpr Vector3<T> getTranslation(const Matrix4x4<T>& m) noexcept
{
    return Vector3<T>(m.vecs[3][0], m.vecs[3][1], m.vecs[3][2]);
}

template <typename T>
constexpr Vector3<T> getScale(const Matrix4x4<T>& m) noexcept
{
    return Vector3<T>(m.vecs[0][0], m.vecs[1][1], m.vecs[2][2]);
}

/* ---- preRotate / postRotate (axis -> quat adapters) ---- */

template <typename T>
inline Matrix4x4<T>& preRotate(Matrix4x4<T>& m, const Vector3<T>& axis, T angle)
{
    return preRotate(m, Quaternion<T>(angle, axis));
}

template <typename T>
inline Matrix4x4<T>& postRotate(Matrix4x4<T>& m, const Vector3<T>& axis, T angle)
{
    return postRotate(m, Quaternion<T>(angle, axis));
}

/* ---- preTranslate / postTranslate / preScale / postScale ---- */

template <typename T>
inline Matrix4x4<T>& preTranslate(Matrix4x4<T>& m, const Vector3<T>& offset)
{
    const auto tx = offset.x, ty = offset.y, tz = offset.z;
    for (size_t col = 0; col < 4; ++col) {
        const auto w = m.vecs[col][3];
        m.vecs[col][0] += tx * w;
        m.vecs[col][1] += ty * w;
        m.vecs[col][2] += tz * w;
    }
    return m;
}

template <typename T>
inline Matrix4x4<T>& postTranslate(Matrix4x4<T>& m, const Vector3<T>& offset)
{
    const auto tx = offset.x, ty = offset.y, tz = offset.z;
    for (size_t row = 0; row < 4; ++row) {
        m.vecs[3][row] += m.vecs[0][row] * tx + m.vecs[1][row] * ty + m.vecs[2][row] * tz;
    }
    return m;
}

template <typename T>
inline Matrix4x4<T>& preScale(Matrix4x4<T>& m, const Vector3<T>& factor)
{
    const auto sx = factor.x, sy = factor.y, sz = factor.z;
    for (size_t col = 0; col < 4; ++col) {
        m.vecs[col][0] *= sx;
        m.vecs[col][1] *= sy;
        m.vecs[col][2] *= sz;
    }
    return m;
}

template <typename T>
inline Matrix4x4<T>& postScale(Matrix4x4<T>& m, const Vector3<T>& factor)
{
    const auto sx = factor.x, sy = factor.y, sz = factor.z;
    for (size_t row = 0; row < 4; ++row) {
        m.vecs[0][row] *= sx;
        m.vecs[1][row] *= sy;
        m.vecs[2][row] *= sz;
    }
    return m;
}

/* ---- operator*(Vector3) / operator*(Point3) ---- */

template <typename T>
inline Vector3<T> operator*(const Matrix4x4<T>& m, const Vector3<T>& v)
{
    return Vector3<T>(m.vecs[0][0] * v.x + m.vecs[1][0] * v.y + m.vecs[2][0] * v.z,
                      m.vecs[0][1] * v.x + m.vecs[1][1] * v.y + m.vecs[2][1] * v.z,
                      m.vecs[0][2] * v.x + m.vecs[1][2] * v.y + m.vecs[2][2] * v.z);
}

template <typename T>
inline Point3<T> operator*(const Matrix4x4<T>& m, const Point3<T>& p)
{
    const auto x = m.vecs[0][0] * p.x + m.vecs[1][0] * p.y + m.vecs[2][0] * p.z + m.vecs[3][0];
    const auto y = m.vecs[0][1] * p.x + m.vecs[1][1] * p.y + m.vecs[2][1] * p.z + m.vecs[3][1];
    const auto z = m.vecs[0][2] * p.x + m.vecs[1][2] * p.y + m.vecs[2][2] * p.z + m.vecs[3][2];
    const auto w = m.vecs[0][3] * p.x + m.vecs[1][3] * p.y + m.vecs[2][3] * p.z + m.vecs[3][3];

    if (math::isEqual(w, T(1), T(1e-12)))
        return Point3<T>(x, y, z);
    if (math::isZero(w, T(1e-12)))
        return Point3<T>(x, y, z);
    return Point3<T>(x / w, y / w, z / w);
}

/* ---- Factory functions ---- */

template <typename T>
inline Matrix4x4<T> rotate(const Vector3<T>& axis, T angle)
{
    Matrix4x4<T> m;
    makeRotation(m, axis, angle);
    return m;
}

template <typename T>
inline Matrix4x4<T> rotate(const Vector3<T>& from, const Vector3<T>& to)
{
    Matrix4x4<T> m;
    makeRotation(m, from, to);
    return m;
}

template <typename T>
inline Matrix4x4<T> translate(const Vector3<T>& offset)
{
    Matrix4x4<T> m;
    makeTranslation(m, offset);
    return m;
}

template <typename T>
inline Matrix4x4<T> translate(T x, T y, T z)
{
    Matrix4x4<T> m;
    makeTranslation(m, x, y, z);
    return m;
}

template <typename T>
inline Matrix4x4<T> scale(const Vector3<T>& vec)
{
    Matrix4x4<T> m;
    makeScale(m, vec);
    return m;
}

template <typename T>
inline Matrix4x4<T> scale(T x, T y, T z)
{
    Matrix4x4<T> m;
    makeScale(m, x, y, z);
    return m;
}

template <typename T>
inline Matrix4x4<T> scale(T factor)
{
    Matrix4x4<T> m;
    makeScale(m, factor);
    return m;
}

template <typename T>
inline Matrix4x4<T> lookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up)
{
    Matrix4x4<T> m;
    makeLookAt(m, eye, target, up);
    return m;
}

template <typename T>
inline Matrix4x4<T> ortho(double l, double r, double b, double t, double n, double f)
{
    Matrix4x4<T> m;
    makeOrtho(m, l, r, b, t, n, f);
    return m;
}

template <typename T>
inline Matrix4x4<T> perspective(double fovy, double aspect, double n, double f)
{
    Matrix4x4<T> m;
    makePerspective(m, fovy, aspect, n, f);
    return m;
}

template <typename T>
inline Matrix4x4<T> fromBasis(const Point3<T>& origin, const Vector3<T>& x, const Vector3<T>& y, const Vector3<T>& z)
{
    Matrix4x4<T> m;
    setBasis(m, origin, x, y, z);
    return m;
}

template <typename T>
inline Matrix4x4<T> reflect(const Vector3<T>& n, T d)
{
    Matrix4x4<T> m;
    makeReflection(m, n, d);
    return m;
}

/* makeRotation / preRotate / postRotate  (delegate to rotate(quat)) */

template <typename T>
inline void makeRotation(Matrix4x4<T>& m, const Quaternion<T>& quat)
{
    m = rotate(quat);
}

template <typename T>
inline void makeRotation(Matrix4x4<T>& m, const Vector3<T>& from, const Vector3<T>& to)
{
    Quaternion<T> q;
    q.makeRotate(from, to);
    m = rotate(q);
}

template <typename T>
inline Matrix4x4<T>& preRotate(Matrix4x4<T>& m, const Quaternion<T>& quat)
{
    const Matrix4x4<T> rot = rotate(quat);
    for (size_t col = 0; col < 4; ++col) {
        const auto ox = m.vecs[col][0], oy = m.vecs[col][1], oz = m.vecs[col][2];
        m.vecs[col][0] = rot.vecs[0][0] * ox + rot.vecs[1][0] * oy + rot.vecs[2][0] * oz;
        m.vecs[col][1] = rot.vecs[0][1] * ox + rot.vecs[1][1] * oy + rot.vecs[2][1] * oz;
        m.vecs[col][2] = rot.vecs[0][2] * ox + rot.vecs[1][2] * oy + rot.vecs[2][2] * oz;
    }
    return m;
}

template <typename T>
inline Matrix4x4<T>& postRotate(Matrix4x4<T>& m, const Quaternion<T>& quat)
{
    const Matrix4x4<T> rot = rotate(quat);
    for (size_t row = 0; row < 4; ++row) {
        const auto oc0 = m.vecs[0][row], oc1 = m.vecs[1][row], oc2 = m.vecs[2][row];
        m.vecs[0][row] = oc0 * rot.vecs[0][0] + oc1 * rot.vecs[0][1] + oc2 * rot.vecs[0][2];
        m.vecs[1][row] = oc0 * rot.vecs[1][0] + oc1 * rot.vecs[1][1] + oc2 * rot.vecs[1][2];
        m.vecs[2][row] = oc0 * rot.vecs[2][0] + oc1 * rot.vecs[2][1] + oc2 * rot.vecs[2][2];
    }
    return m;
}

/* ---- toRotationMatrix ---- */

template <typename T>
inline Matrix3x3<T> toRotationMatrix(const Quaternion<T>& quat)
{
    const Matrix4x4<T> m4 = rotate(quat);
    return Matrix3x3<T>(m4(0, 0), m4(0, 1), m4(0, 2), m4(1, 0), m4(1, 1), m4(1, 2), m4(2, 0), m4(2, 1), m4(2, 2));
}

template <typename T>
inline Matrix4x4<T> toMatrix4x4(const Isometry3<T>& tf)
{
    Matrix4x4<T> m;
    setBasis(m, tf.translation, tf.right(), tf.up(), tf.forward());
    return m;
}

V_MATH_NS_END
