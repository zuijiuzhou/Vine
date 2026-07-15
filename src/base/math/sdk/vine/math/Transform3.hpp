#pragma once

#include "math_global.hpp"

#include "Point3.hpp"
#include "Quaternion.hpp"
#include "Vector3.hpp"

V_MATH_NS_BEGIN

/**
 * @brief 3D rigid transformation class that represents translation, rotation.
 * @tparam T Only accepts float and double
 */
template <typename T>
class Transform3 {
  public:
    using value_type = T;

  public:
    /**
     * @brief Construct an identity transform.
     */
    constexpr Transform3() noexcept
      : translation()
      , rotation(T(0), T(0), T(0), T(1))
    {}

    /**
     * @brief Construct from translation and rotation.
     * @param t Translation offset.
     * @param q Rotation quaternion.
     */
    constexpr Transform3(const Point3<T>& t, const Quaternion<T>& q) noexcept
      : translation(t)
      , rotation(q)
    {}

  public:
    /**
     * @brief Invert this transformation.
     * @return Inverted transformation.
     */
    Transform3<T> inverted() const;

    /**
     * @brief Invert this transformation in place.
     */
    void invert();

    /**
     * @brief Pre-multiply a translation: T := T_trans(dt) * T.
     * @param dt Translation offset in world space.
     */
    Transform3<T>& preTranslate(const Vector3<T>& dt);

    /**
     * @brief Post-multiply a translation: T := T * T_trans(dt).
     * @param dt Translation offset in local space.
     */
    Transform3<T>& postTranslate(const Vector3<T>& dt);

    /**
     * @brief Pre-multiply a quaternion rotation: T := T_rot(quat) * T.
     * @param quat Rotation quaternion (world-space).
     */
    Transform3<T>& preRotate(const Quaternion<T>& quat);

    /**
     * @brief Pre-multiply an axis-angle rotation: T := T_rot(axis, angle) * T.
     * @param axis  Rotation axis (world-space), should be normalized.
     * @param angle Rotation angle in radians.
     */
    Transform3<T>& preRotate(const Vector3<T>& axis, T angle);

    /**
     * @brief Post-multiply a quaternion rotation: T := T * T_rot(quat).
     * @param quat Rotation quaternion (local-space).
     */
    Transform3<T>& postRotate(const Quaternion<T>& quat);

    /**
     * @brief Post-multiply an axis-angle rotation: T := T * T_rot(axis, angle).
     * @param axis  Rotation axis (local-space), should be normalized.
     * @param angle Rotation angle in radians.
     */
    Transform3<T>& postRotate(const Vector3<T>& axis, T angle);

    /**
     * @brief Compose two transforms: T₁ * T₂.
     */
    Transform3<T>  operator*(const Transform3<T>& right) const;
    Transform3<T>& operator*=(const Transform3<T>& right);

    /**
     * @brief Local X axis in parent coordinates (right direction).
     */
    [[nodiscard]] Vector3<T> right()   const { return rotation * Vector3<T>::unitX(); }
    /**
     * @brief Local Y axis in parent coordinates (up direction).
     */
    [[nodiscard]] Vector3<T> up()      const { return rotation * Vector3<T>::unitY(); }
    /**
     * @brief Local Z axis in parent coordinates (forward direction).
     */
    [[nodiscard]] Vector3<T> forward() const { return rotation * Vector3<T>::unitZ(); }

  public:
    Point3<T>     translation;
    Quaternion<T> rotation;
};

template <typename T>
Point3<T> operator*(const Transform3<T>& t, const Point3<T>& p);

template <typename T>
Vector3<T> operator*(const Transform3<T>& t, const Vector3<T>& v);

using Transform3f = Transform3<float>;
using Transform3d = Transform3<double>;

V_MATH_NS_END
