#pragma once

#include "math_global.hpp"

#include "Point3.hpp"
#include "Rotation3.hpp"

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
      , rotation()
    {}

    /**
     * @brief Construct from translation and rotation.
     * @param t Translation offset.
     * @param r Rotation matrix.
     */
    constexpr Transform3(const Point3<T>& t, const Rotation3<T>& r) noexcept
      : translation(t)
      , rotation(r)
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
     * @brief Pre-multiply a rotation: T := T_rot(r) * T.
     * @param r Rotation in world space.
     */
    Transform3<T>& preRotate(const Rotation3<T>& r);

    /**
     * @brief Post-multiply a rotation: T := T * T_rot(r).
     * @param r Rotation in local space.
     */
    Transform3<T>& postRotate(const Rotation3<T>& r);

    /**
     * @brief Compose two transforms: T₁ * T₂.
     */
    Transform3<T>  operator*(const Transform3<T>& right) const;
    Transform3<T>& operator*=(const Transform3<T>& right);

  public:
    Point3<T>    translation;
    Rotation3<T> rotation;
};

template <typename T>
Point3<T> operator*(const Transform3<T>& t, const Point3<T>& p);

template <typename T>
Vector3<T> operator*(const Transform3<T>& t, const Vector3<T>& v);

using Transform3f = Transform3<float>;
using Transform3d = Transform3<double>;

V_MATH_NS_END
