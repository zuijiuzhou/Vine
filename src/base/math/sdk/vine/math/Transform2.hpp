#pragma once

#include "math_global.hpp"

#include "Point2.hpp"
#include "Vector2.hpp"

V_MATH_NS_BEGIN

/**
 * @brief 2D rigid transformation (translation + rotation), no scale.
 * @tparam T floating-point type (typically float or double).
 *
 * Represented as T = (θ, t) where θ is the CCW rotation angle in radians
 * and t is the translation. A point p is transformed as:
 *
 *   p' = R(θ) * p + t
 *
 * Composition: T₁ * T₂ = (θ₁+θ₂,  R(θ₁)*t₂ + t₁)
 *
 * This is far more efficient than full 3x3 matrix multiplication for
 * rigid 2D transforms.
 */
template <typename T>
class Transform2 {
  public:
    using value_type = T;

  public:
    /**
     * @brief Construct an identity transform (θ=0, t=(0,0)).
     */
    constexpr Transform2() noexcept
      : translation()
      , rotation(T(0))
    {}

    /**
     * @brief Construct from translation and rotation angle.
     * @param t Translation offset.
     * @param r Rotation angle in radians (CCW).
     */
    constexpr Transform2(const Point2<T>& t, T r) noexcept
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
     * @brief Pre-multiply a rotation: T := T_rot(angle) * T.
     * @param angle Rotation angle in radians (CCW) in world space.
     */
    Transform2<T>& preRotate(T angle);

    /**
     * @brief Post-multiply a rotation: T := T * T_rot(angle).
     * @param angle Rotation angle in radians (CCW) in local space.
     */
    constexpr Transform2<T>& postRotate(T angle)
    {
        rotation += angle;
        return *this;
    }

    /**
     * @brief Compose two transforms: T₁ * T₂.
     */
    Transform2<T>  operator*(const Transform2<T>& right) const;
    Transform2<T>& operator*=(const Transform2<T>& right);

  public:
    Point2<T> translation; ///< Translation offset.
    T         rotation;    ///< Rotation angle in radians (CCW).
};

template <typename T>
Point2<T> operator*(const Transform2<T>& t, const Point2<T>& p);

template <typename T>
Vector2<T> operator*(const Transform2<T>& t, const Vector2<T>& v);

using Transform2f = Transform2<float>;
using Transform2d = Transform2<double>;

V_MATH_NS_END