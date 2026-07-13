#pragma once

#include "math_global.hpp"

#include "Point2.hpp"
#include "Rotation2.hpp"
#include "Vector2.hpp"

V_MATH_NS_BEGIN

/**
 * @brief 2D rigid transformation (translation + rotation), no scale.
 * @tparam T floating-point type (typically float or double).
 *
 * Structurally mirrors Transform3: (translation, rotation).
 * Rotation2 stores (cos θ, sin θ) internally for efficient composition
 * without repeated trigonometric calls.
 *
 * A point p is transformed as:
 *
 *   p' = R * p + t
 *
 * Composition: T₁ * T₂ = (R₁ * R₂,  R₁ * t₂ + t₁)
 */
template <typename T>
class Transform2 {
  public:
    using value_type = T;

  public:
    /**
     * @brief Construct an identity transform.
     */
    constexpr Transform2() noexcept
      : translation()
      , rotation()
    {}

    /**
     * @brief Construct from translation and rotation.
     * @param t Translation offset.
     * @param r Rotation.
     */
    constexpr Transform2(const Point2<T>& t, const Rotation2<T>& r) noexcept
      : translation(t)
      , rotation(r)
    {}

  public:
    /**
     * @brief Invert this transformation.
     * @return Inverted transformation.
     */
    Transform2<T> inverted() const;

    /**
     * @brief Invert this transformation in place.
     */
    void invert();

    /**
     * @brief Pre-multiply a translation: T := T_trans(dt) * T.
     * @param dt Translation offset in world space.
     */
    Transform2<T>& preTranslate(const Vector2<T>& dt);

    /**
     * @brief Post-multiply a translation: T := T * T_trans(dt).
     * @param dt Translation offset in local space.
     */
    Transform2<T>& postTranslate(const Vector2<T>& dt);

    /**
     * @brief Pre-multiply a rotation: T := T_rot(r) * T.
     * @param r Rotation in world space.
     */
    Transform2<T>& preRotate(const Rotation2<T>& r);

    /**
     * @brief Pre-multiply a rotation by angle: T := T_rot(angle) * T.
     * @param angle Rotation angle in radians (CCW), world space.
     */
    Transform2<T>& preRotate(T angle);

    /**
     * @brief Post-multiply a rotation: T := T * T_rot(r).
     * @param r Rotation in local space.
     */
    Transform2<T>& postRotate(const Rotation2<T>& r);

    /**
     * @brief Post-multiply a rotation by angle: T := T * T_rot(angle).
     * @param angle Rotation angle in radians (CCW), local space.
     */
    Transform2<T>& postRotate(T angle);

    /**
     * @brief Compose two transforms: T₁ * T₂.
     */
    Transform2<T>  operator*(const Transform2<T>& right) const;
    Transform2<T>& operator*=(const Transform2<T>& right);

  public:
    Point2<T>    translation; ///< Translation offset.
    Rotation2<T> rotation;    ///< Rotation as (cos θ, sin θ).
};

template <typename T>
Point2<T> operator*(const Transform2<T>& t, const Point2<T>& p);

template <typename T>
Vector2<T> operator*(const Transform2<T>& t, const Vector2<T>& v);

using Transform2f = Transform2<float>;
using Transform2d = Transform2<double>;

V_MATH_NS_END