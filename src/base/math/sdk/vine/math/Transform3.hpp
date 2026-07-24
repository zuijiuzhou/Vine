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
 * All functions operate on Matrix4x4<T, Order> and follow the standard
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
template <typename T, typename Order>
constexpr void makeTranslation(Matrix4x4<T, Order>& m, const Vector3<T>& offset) noexcept;

/**
 * @brief Build translation matrix from components.
 * @param m Target matrix (overwritten).
 * @param x Translation along X axis.
 * @param y Translation along Y axis.
 * @param z Translation along Z axis.
 */
template <typename T, typename Order>
constexpr void makeTranslation(Matrix4x4<T, Order>& m, T x, T y, T z) noexcept;

/* ---- rotation ---- */

/**
 * @brief Build rotation matrix from quaternion.
 * @param m    Target matrix (overwritten).
 * @param quat Rotation quaternion.
 */
template <typename T, typename Order>
void makeRotation(Matrix4x4<T, Order>& m, const Quaternion<T>& quat);

/**
 * @brief Build axis-angle rotation matrix.
 * @param m     Target matrix (overwritten).
 * @param axis  Rotation axis.
 * @param angle Rotation angle in radians.
 */
template <typename T, typename Order>
void makeRotation(Matrix4x4<T, Order>& m, const Vector3<T>& axis, T angle);

/**
 * @brief Build rotation from start vector to end vector.
 * @param m     Target matrix (overwritten).
 * @param start Source direction vector.
 * @param end   Target direction vector.
 */
template <typename T, typename Order>
void makeRotation(Matrix4x4<T, Order>& m, const Vector3<T>& start, const Vector3<T>& end);

/* ---- scale ---- */

/**
 * @brief Build non-uniform scale matrix from vector.
 * @param m   Target matrix (overwritten).
 * @param vec Scale factors for x/y/z.
 */
template <typename T, typename Order>
constexpr void makeScale(Matrix4x4<T, Order>& m, const Vector3<T>& vec) noexcept;

/**
 * @brief Build non-uniform scale matrix from components.
 * @param m Target matrix (overwritten).
 * @param x Scale factor along X axis.
 * @param y Scale factor along Y axis.
 * @param z Scale factor along Z axis.
 */
template <typename T, typename Order>
constexpr void makeScale(Matrix4x4<T, Order>& m, T x, T y, T z) noexcept;

/**
 * @brief Build uniform scale matrix.
 * @param m      Target matrix (overwritten).
 * @param factor Uniform scale factor.
 */
template <typename T, typename Order>
constexpr void makeScale(Matrix4x4<T, Order>& m, T factor) noexcept;

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
template <typename T, typename Order>
void makeLookAt(Matrix4x4<T, Order>& m, const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up);

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
template <typename T, typename Order>
void makeOrtho(Matrix4x4<T, Order>& m, double left, double right, double bottom, double top, double z_near, double z_far);

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
template <typename T, typename Order>
void makePerspective(Matrix4x4<T, Order>& m, double fovy, double aspect_ratio, double z_near, double z_far);

/* ---- reflection ---- */

/**
 * @brief Make a reflection matrix across a plane defined by its normal and offset.
 * @param m            Target matrix (overwritten).
 * @param plane_normal Normal vector of the mirror plane.
 * @param plane_offset Offset of the mirror plane from the origin.
 */
template <typename T, typename Order>
void makeReflection(Matrix4x4<T, Order>& m, const Vector3<T>& plane_normal, T plane_offset);

/* ---- basis ---- */

/**
 * @brief Set the coordinate system represented by this matrix.
 * @param m      Target matrix (overwritten).
 * @param origin the origin point of the coordinate system.
 * @param x_axis the x axis direction of the coordinate system.
 * @param y_axis the y axis direction of the coordinate system.
 * @param z_axis the z axis direction of the coordinate system.
 */
template <typename T, typename Order>
void setBasis(Matrix4x4<T, Order>& m, const Point3<T>& origin, const Vector3<T>& x_axis, const Vector3<T>& y_axis, const Vector3<T>& z_axis);

/* ========================================================================= */
/*  2. Get  –  extract properties from a matrix                              */
/* ========================================================================= */

/**
 * @brief Get the translation vector from column 3 of a 4x4 matrix.
 * @param m Transform matrix.
 * @return Column 3 xyz as Vector3<T>.
 */
template <typename T, typename Order>
constexpr Vector3<T> getTranslation(const Matrix4x4<T, Order>& m) noexcept;

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
template <typename T, typename Order>
Quaternion<T> getRotation(const Matrix4x4<T, Order>& m);

/**
 * @brief Get the diagonal of the upper-left 3x3 block.
 * @param m Transform matrix.
 * @return (m00, m11, m22) as Vector3<T>.
 */
template <typename T, typename Order>
constexpr Vector3<T> getScale(const Matrix4x4<T, Order>& m) noexcept;

/**
 * @brief Get the coordinate system represented by this matrix.
 * @param m        Source matrix.
 * @param o_origin Output origin point.
 * @param o_x_axis Output x-axis direction.
 * @param o_y_axis Output y-axis direction.
 * @param o_z_axis Output z-axis direction.
 */
template <typename T, typename Order>
void getBasis(const Matrix4x4<T, Order>& m, Point3<T>& o_origin, Vector3<T>& o_x_axis, Vector3<T>& o_y_axis, Vector3<T>& o_z_axis);

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
template <typename T, typename Order>
Matrix4x4<T, Order>& preRotate(Matrix4x4<T, Order>& m, const Vector3<T>& axis, T angle);

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
template <typename T, typename Order>
Matrix4x4<T, Order>& postRotate(Matrix4x4<T, Order>& m, const Vector3<T>& axis, T angle);

/**
 * @brief Apply a quaternion rotation in world space: M := R * M.
 *
 * Equivalent to preMulti() with a pure rotation matrix.
 *
 * @param m    Target matrix (modified in-place).
 * @param quat Rotation quaternion.
 * @return Reference to m.
 */
template <typename T, typename Order>
Matrix4x4<T, Order>& preRotate(Matrix4x4<T, Order>& m, const Quaternion<T>& quat);

/**
 * @brief Apply a quaternion rotation in local space: M := M * R.
 *
 * Equivalent to postMulti() with a pure rotation matrix.
 *
 * @param m    Target matrix (modified in-place).
 * @param quat Rotation quaternion.
 * @return Reference to m.
 */
template <typename T, typename Order>
Matrix4x4<T, Order>& postRotate(Matrix4x4<T, Order>& m, const Quaternion<T>& quat);

/**
 * @brief Apply a translation in world space: M := T * M.
 *
 * Equivalent to preMulti() with a pure translation matrix.
 *
 * @param m      Target matrix (modified in-place).
 * @param offset Translation offset (world-space).
 * @return Reference to m.
 */
template <typename T, typename Order>
Matrix4x4<T, Order>& preTranslate(Matrix4x4<T, Order>& m, const Vector3<T>& offset);

/**
 * @brief Apply a translation in local space: M := M * T.
 *
 * Equivalent to postMulti() with a pure translation matrix.
 *
 * @param m      Target matrix (modified in-place).
 * @param offset Translation offset (local-space).
 * @return Reference to m.
 */
template <typename T, typename Order>
Matrix4x4<T, Order>& postTranslate(Matrix4x4<T, Order>& m, const Vector3<T>& offset);

/**
 * @brief Apply a non-uniform scale in world space: M := S * M.
 *
 * Equivalent to preMulti() with a pure scale matrix.
 *
 * @param m      Target matrix (modified in-place).
 * @param factor Scale factor per axis (world-space).
 * @return Reference to m.
 */
template <typename T, typename Order>
Matrix4x4<T, Order>& preScale(Matrix4x4<T, Order>& m, const Vector3<T>& factor);

/**
 * @brief Apply a non-uniform scale in local space: M := M * S.
 *
 * Equivalent to postMulti() with a pure scale matrix.
 *
 * @param m      Target matrix (modified in-place).
 * @param factor Scale factor per axis (local-space).
 * @return Reference to m.
 */
template <typename T, typename Order>
Matrix4x4<T, Order>& postScale(Matrix4x4<T, Order>& m, const Vector3<T>& factor);

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
template <typename T, typename Order>
Vector3<T> operator*(const Matrix4x4<T, Order>& m, const Vector3<T>& v);

/**
 * @brief Transform a point (w = 1, full affine with perspective divide).
 * @param m Transform matrix.
 * @param p Input point.
 * @return Transformed point (perspective-divided if w != 1).
 */
template <typename T, typename Order>
Point3<T> operator*(const Matrix4x4<T, Order>& m, const Point3<T>& p);

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
template <typename T, typename Order = ColMajor>
[[nodiscard]] Matrix4x4<T, Order> rotate(const Quaternion<T>& quat);

/**
 * @brief Create an axis-angle rotation matrix.
 * @param axis  Rotation axis.
 * @param angle Rotation angle in radians.
 * @return Rotation matrix.
 */
template <typename T, typename Order = ColMajor>
[[nodiscard]] Matrix4x4<T, Order> rotate(const Vector3<T>& axis, T angle);

/**
 * @brief Create a rotation matrix from start and end vectors.
 * @param start Source direction vector.
 * @param end   Target direction vector.
 * @return Rotation matrix.
 */
template <typename T, typename Order = ColMajor>
[[nodiscard]] Matrix4x4<T, Order> rotate(const Vector3<T>& start, const Vector3<T>& end);

/**
 * @brief Create a translation matrix from vector.
 * @param offset Translation offset.
 * @return Translation matrix.
 */
template <typename T, typename Order = ColMajor>
[[nodiscard]] Matrix4x4<T, Order> translate(const Vector3<T>& offset);

/**
 * @brief Create a translation matrix from components.
 * @param x Translation along X axis.
 * @param y Translation along Y axis.
 * @param z Translation along Z axis.
 * @return Translation matrix.
 */
template <typename T, typename Order = ColMajor>
[[nodiscard]] Matrix4x4<T, Order> translate(T x, T y, T z);

/**
 * @brief Create a non-uniform scale matrix from vector.
 * @param vec Scale factors for x/y/z.
 * @return Scale matrix.
 */
template <typename T, typename Order = ColMajor>
[[nodiscard]] Matrix4x4<T, Order> scale(const Vector3<T>& vec);

/**
 * @brief Create a non-uniform scale matrix from components.
 * @param x Scale factor along X axis.
 * @param y Scale factor along Y axis.
 * @param z Scale factor along Z axis.
 * @return Scale matrix.
 */
template <typename T, typename Order = ColMajor>
[[nodiscard]] Matrix4x4<T, Order> scale(T x, T y, T z);

/**
 * @brief Create a uniform scale matrix.
 * @param factor Uniform scale factor.
 * @return Scale matrix.
 */
template <typename T, typename Order = ColMajor>
[[nodiscard]] Matrix4x4<T, Order> scale(T factor);

/**
 * @brief Create a look-at view matrix.
 * @param eye    Camera position.
 * @param target Camera target point.
 * @param up     Up direction reference.
 * @return View matrix.
 */
template <typename T, typename Order = ColMajor>
[[nodiscard]] Matrix4x4<T, Order> lookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up);

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
template <typename T, typename Order = ColMajor>
[[nodiscard]] Matrix4x4<T, Order> ortho(double left, double right, double bottom, double top, double z_near, double z_far);

/**
 * @brief Create a perspective projection matrix.
 * @param fovy         Vertical field of view in radians.
 * @param aspect_ratio Viewport aspect ratio.
 * @param z_near       Near clipping plane.
 * @param z_far        Far clipping plane.
 * @return Perspective projection matrix.
 */
template <typename T, typename Order = ColMajor>
[[nodiscard]] Matrix4x4<T, Order> perspective(double fovy, double aspect_ratio, double z_near, double z_far);

/**
 * @brief Create a reflection matrix across a plane.
 * @param plane_normal Normal vector of the plane.
 * @param plane_offset Offset of the plane from the origin.
 * @return Reflection matrix.
 */
template <typename T, typename Order = ColMajor>
[[nodiscard]] Matrix4x4<T, Order> reflect(const Vector3<T>& plane_normal, T plane_offset);

/**
 * @brief Create a matrix from basis vectors and origin.
 * @param origin Coordinate system origin.
 * @param x_axis X-axis direction.
 * @param y_axis Y-axis direction.
 * @param z_axis Z-axis direction.
 * @return Transform matrix composed from basis.
 */
template <typename T, typename Order = ColMajor>
[[nodiscard]] Matrix4x4<T, Order> fromBasis(const Point3<T>& origin, const Vector3<T>& x_axis, const Vector3<T>& y_axis, const Vector3<T>& z_axis);

/* ========================================================================= */
/*  6. Convert  –  type conversion                                           */
/* ========================================================================= */

/**
 * @brief Convert a unit quaternion to a 3x3 rotation matrix.
 * @param quat Rotation quaternion.
 * @return 3x3 rotation matrix (upper-left block of the equivalent 4x4).
 */
template <typename T, typename Order = ColMajor>
Matrix3x3<T, Order> toRotationMatrix(const Quaternion<T>& quat);

/**
 * @brief Construct a 4x4 rigid transform matrix from an Isometry3 object.
 * @param tf Isometry3 object containing translation and rotation.
 * @return 4x4 rigid transform matrix.
 */
template <typename T, typename Order = ColMajor>
Matrix4x4<T, Order> toMatrix4x4(const Isometry3<T>& tf);

/* ========================================================================= */
/*  Inline implementations (≤ 20 lines each)                                 */
/* ========================================================================= */

/* ---- makeTranslation ---- */

template <typename T, typename Order>
constexpr void makeTranslation(Matrix4x4<T, Order>& m, const Vector3<T>& offset) noexcept
{
    m.makeIdentity();
    m.element(0, 3) = offset.x;
    m.element(1, 3) = offset.y;
    m.element(2, 3) = offset.z;
}

template <typename T, typename Order>
constexpr void makeTranslation(Matrix4x4<T, Order>& m, T x, T y, T z) noexcept
{
    m.makeIdentity();
    m.element(0, 3) = x;
    m.element(1, 3) = y;
    m.element(2, 3) = z;
}

/* ---- makeScale ---- */

template <typename T, typename Order>
constexpr void makeScale(Matrix4x4<T, Order>& m, const Vector3<T>& vec) noexcept
{
    m.makeIdentity();
    m.element(0, 0) = vec.x;
    m.element(1, 1) = vec.y;
    m.element(2, 2) = vec.z;
}

template <typename T, typename Order>
constexpr void makeScale(Matrix4x4<T, Order>& m, T x, T y, T z) noexcept
{
    m.makeIdentity();
    m.element(0, 0) = x;
    m.element(1, 1) = y;
    m.element(2, 2) = z;
}

template <typename T, typename Order>
constexpr void makeScale(Matrix4x4<T, Order>& m, T factor) noexcept
{
    m.makeIdentity();
    m.element(0, 0) = factor;
    m.element(1, 1) = factor;
    m.element(2, 2) = factor;
}

/* ---- makeOrtho / makePerspective ---- */

template <typename T, typename Order>
inline void makeOrtho(Matrix4x4<T, Order>& m, double left, double right, double bottom, double top, double z_near, double z_far)
{
    auto tx = -(right + left) / (right - left);
    auto ty = -(top + bottom) / (top - bottom);
    auto tz = -(z_far + z_near) / (z_far - z_near);

    // Column 0
    m.element(0, 0) = T(2) / (right - left);
    m.element(1, 0) = T(0);
    m.element(2, 0) = T(0);
    m.element(3, 0) = T(0);
    // Column 1
    m.element(0, 1) = T(0);
    m.element(1, 1) = T(2) / (top - bottom);
    m.element(2, 1) = T(0);
    m.element(3, 1) = T(0);
    // Column 2
    m.element(0, 2) = T(0);
    m.element(1, 2) = T(0);
    m.element(2, 2) = T(-2) / (z_far - z_near);
    m.element(3, 2) = T(0);
    // Column 3 (translation)
    m.element(0, 3) = T(tx);
    m.element(1, 3) = T(ty);
    m.element(2, 3) = T(tz);
    m.element(3, 3) = T(1);
}

template <typename T, typename Order>
inline void makePerspective(Matrix4x4<T, Order>& m, double fovy, double aspect_ratio, double z_near, double z_far)
{
    const auto f = T(1) / std::tan(fovy / 2.0);
    m.makeIdentity();
    m.element(0, 0) = T(0);
    m.element(1, 1) = T(0);
    m.element(2, 2) = T(0);
    m.element(3, 3) = T(0);

    m.element(0, 0) = T(f / aspect_ratio);
    m.element(1, 1) = T(f);
    m.element(2, 2) = T((z_far + z_near) / (z_near - z_far));
    m.element(3, 2) = T(-1);
    m.element(2, 3) = T(2 * z_far * z_near / (z_near - z_far));
}

/* ---- setBasis / getBasis ---- */

template <typename T, typename Order>
inline void setBasis(Matrix4x4<T, Order>& m, const Point3<T>& origin, const Vector3<T>& x_axis, const Vector3<T>& y_axis, const Vector3<T>& z_axis)
{
    m.element(0, 0) = x_axis.x; m.element(0, 1) = y_axis.x; m.element(0, 2) = z_axis.x; m.element(0, 3) = origin.x;
    m.element(1, 0) = x_axis.y; m.element(1, 1) = y_axis.y; m.element(1, 2) = z_axis.y; m.element(1, 3) = origin.y;
    m.element(2, 0) = x_axis.z; m.element(2, 1) = y_axis.z; m.element(2, 2) = z_axis.z; m.element(2, 3) = origin.z;
    m.element(3, 0) = T(0);      m.element(3, 1) = T(0);      m.element(3, 2) = T(0);      m.element(3, 3) = T(1);
}

template <typename T, typename Order>
inline void getBasis(const Matrix4x4<T, Order>& m, Point3<T>& o_origin, Vector3<T>& o_x_axis, Vector3<T>& o_y_axis, Vector3<T>& o_z_axis)
{
    o_origin = Point3<T>(m.element(0, 3), m.element(1, 3), m.element(2, 3));
    o_x_axis.set(m.element(0, 0), m.element(1, 0), m.element(2, 0));
    o_y_axis.set(m.element(0, 1), m.element(1, 1), m.element(2, 1));
    o_z_axis.set(m.element(0, 2), m.element(1, 2), m.element(2, 2));
}

/* ---- getTranslation / getScale ---- */

template <typename T, typename Order>
constexpr Vector3<T> getTranslation(const Matrix4x4<T, Order>& m) noexcept
{
    return Vector3<T>(m.element(0, 3), m.element(1, 3), m.element(2, 3));
}

template <typename T, typename Order>
constexpr Vector3<T> getScale(const Matrix4x4<T, Order>& m) noexcept
{
    return Vector3<T>(m.element(0, 0), m.element(1, 1), m.element(2, 2));
}

/* ---- preRotate / postRotate (axis -> quat adapters) ---- */

template <typename T, typename Order>
inline Matrix4x4<T, Order>& preRotate(Matrix4x4<T, Order>& m, const Vector3<T>& axis, T angle)
{
    return preRotate(m, Quaternion<T>(angle, axis));
}

template <typename T, typename Order>
inline Matrix4x4<T, Order>& postRotate(Matrix4x4<T, Order>& m, const Vector3<T>& axis, T angle)
{
    return postRotate(m, Quaternion<T>(angle, axis));
}

/* ---- preTranslate / postTranslate / preScale / postScale ---- */

template <typename T, typename Order>
inline Matrix4x4<T, Order>& preTranslate(Matrix4x4<T, Order>& m, const Vector3<T>& offset)
{
    const auto tx = offset.x, ty = offset.y, tz = offset.z;
    for (int col = 0; col < 4; ++col) {
        const auto w = m.element(3, col);
        m.element(0, col) += tx * w;
        m.element(1, col) += ty * w;
        m.element(2, col) += tz * w;
    }
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order>& postTranslate(Matrix4x4<T, Order>& m, const Vector3<T>& offset)
{
    const auto tx = offset.x, ty = offset.y, tz = offset.z;
    for (int row = 0; row < 4; ++row) {
        m.element(row, 3) += m.element(row, 0) * tx + m.element(row, 1) * ty + m.element(row, 2) * tz;
    }
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order>& preScale(Matrix4x4<T, Order>& m, const Vector3<T>& factor)
{
    const auto sx = factor.x, sy = factor.y, sz = factor.z;
    for (int col = 0; col < 4; ++col) {
        m.element(0, col) *= sx;
        m.element(1, col) *= sy;
        m.element(2, col) *= sz;
    }
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order>& postScale(Matrix4x4<T, Order>& m, const Vector3<T>& factor)
{
    const auto sx = factor.x, sy = factor.y, sz = factor.z;
    for (int row = 0; row < 4; ++row) {
        m.element(row, 0) *= sx;
        m.element(row, 1) *= sy;
        m.element(row, 2) *= sz;
    }
    return m;
}

/* ---- operator*(Vector3) / operator*(Point3) ---- */

template <typename T, typename Order>
inline Vector3<T> operator*(const Matrix4x4<T, Order>& m, const Vector3<T>& v)
{
    return Vector3<T>(m.element(0, 0) * v.x + m.element(0, 1) * v.y + m.element(0, 2) * v.z,
                      m.element(1, 0) * v.x + m.element(1, 1) * v.y + m.element(1, 2) * v.z,
                      m.element(2, 0) * v.x + m.element(2, 1) * v.y + m.element(2, 2) * v.z);
}

template <typename T, typename Order>
inline Point3<T> operator*(const Matrix4x4<T, Order>& m, const Point3<T>& p)
{
    const auto x = m.element(0, 0) * p.x + m.element(0, 1) * p.y + m.element(0, 2) * p.z + m.element(0, 3);
    const auto y = m.element(1, 0) * p.x + m.element(1, 1) * p.y + m.element(1, 2) * p.z + m.element(1, 3);
    const auto z = m.element(2, 0) * p.x + m.element(2, 1) * p.y + m.element(2, 2) * p.z + m.element(2, 3);
    const auto w = m.element(3, 0) * p.x + m.element(3, 1) * p.y + m.element(3, 2) * p.z + m.element(3, 3);

    if (math::isEqual(w, T(1), T(1e-12)))
        return Point3<T>(x, y, z);
    if (math::isZero(w, T(1e-12)))
        return Point3<T>(x, y, z);
    return Point3<T>(x / w, y / w, z / w);
}

/* ---- Factory functions ---- */

template <typename T, typename Order>
inline Matrix4x4<T, Order> rotate(const Vector3<T>& axis, T angle)
{
    Matrix4x4<T, Order> m;
    makeRotation(m, axis, angle);
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order> rotate(const Vector3<T>& from, const Vector3<T>& to)
{
    Matrix4x4<T, Order> m;
    makeRotation(m, from, to);
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order> translate(const Vector3<T>& offset)
{
    Matrix4x4<T, Order> m;
    makeTranslation(m, offset);
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order> translate(T x, T y, T z)
{
    Matrix4x4<T, Order> m;
    makeTranslation(m, x, y, z);
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order> scale(const Vector3<T>& vec)
{
    Matrix4x4<T, Order> m;
    makeScale(m, vec);
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order> scale(T x, T y, T z)
{
    Matrix4x4<T, Order> m;
    makeScale(m, x, y, z);
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order> scale(T factor)
{
    Matrix4x4<T, Order> m;
    makeScale(m, factor);
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order> lookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up)
{
    Matrix4x4<T, Order> m;
    makeLookAt(m, eye, target, up);
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order> ortho(double l, double r, double b, double t, double n, double f)
{
    Matrix4x4<T, Order> m;
    makeOrtho(m, l, r, b, t, n, f);
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order> perspective(double fovy, double aspect, double n, double f)
{
    Matrix4x4<T, Order> m;
    makePerspective(m, fovy, aspect, n, f);
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order> fromBasis(const Point3<T>& origin, const Vector3<T>& x, const Vector3<T>& y, const Vector3<T>& z)
{
    Matrix4x4<T, Order> m;
    setBasis(m, origin, x, y, z);
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order> reflect(const Vector3<T>& n, T d)
{
    Matrix4x4<T, Order> m;
    makeReflection(m, n, d);
    return m;
}

/* makeRotation / preRotate / postRotate  (delegate to rotate(quat)) */

template <typename T, typename Order>
inline void makeRotation(Matrix4x4<T, Order>& m, const Quaternion<T>& quat)
{
    m = rotate(quat);
}

template <typename T, typename Order>
inline void makeRotation(Matrix4x4<T, Order>& m, const Vector3<T>& from, const Vector3<T>& to)
{
    Quaternion<T> q;
    q.makeRotate(from, to);
    m = rotate(q);
}

template <typename T, typename Order>
inline Matrix4x4<T, Order>& preRotate(Matrix4x4<T, Order>& m, const Quaternion<T>& quat)
{
    const auto rot = rotate(quat);
    for (int col = 0; col < 4; ++col) {
        const auto ox = m.element(0, col), oy = m.element(1, col), oz = m.element(2, col);
        m.element(0, col) = rot.element(0, 0) * ox + rot.element(0, 1) * oy + rot.element(0, 2) * oz;
        m.element(1, col) = rot.element(1, 0) * ox + rot.element(1, 1) * oy + rot.element(1, 2) * oz;
        m.element(2, col) = rot.element(2, 0) * ox + rot.element(2, 1) * oy + rot.element(2, 2) * oz;
    }
    return m;
}

template <typename T, typename Order>
inline Matrix4x4<T, Order>& postRotate(Matrix4x4<T, Order>& m, const Quaternion<T>& quat)
{
    const auto rot = rotate(quat);
    for (int row = 0; row < 4; ++row) {
        const auto oc0 = m.element(row, 0), oc1 = m.element(row, 1), oc2 = m.element(row, 2);
        m.element(row, 0) = oc0 * rot.element(0, 0) + oc1 * rot.element(1, 0) + oc2 * rot.element(2, 0);
        m.element(row, 1) = oc0 * rot.element(0, 1) + oc1 * rot.element(1, 1) + oc2 * rot.element(2, 1);
        m.element(row, 2) = oc0 * rot.element(0, 2) + oc1 * rot.element(1, 2) + oc2 * rot.element(2, 2);
    }
    return m;
}

/* ---- toRotationMatrix ---- */

template <typename T, typename Order>
inline Matrix3x3<T, Order> toRotationMatrix(const Quaternion<T>& quat)
{
    const Matrix4x4<T, Order> m4 = rotate(quat);
    return Matrix3x3<T, Order>(m4(0, 0), m4(0, 1), m4(0, 2), m4(1, 0), m4(1, 1), m4(1, 2), m4(2, 0), m4(2, 1), m4(2, 2));
}

template <typename T, typename Order>
inline Matrix4x4<T, Order> toMatrix4x4(const Isometry3<T>& tf)
{
    Matrix4x4<T, Order> m;
    setBasis(m, tf.translation, tf.right(), tf.up(), tf.forward());
    return m;
}

V_MATH_NS_END
