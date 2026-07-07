#include <vine/math/Transform2.hpp>

#include <cmath>

V_MATH_NS_BEGIN

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

namespace
{

/// Rotate a 2D vector by angle (radians, CCW).
template <typename T>
Point2<T> rotatePoint(T angle, const Point2<T>& p)
{
    const auto c = std::cos(angle);
    const auto s = std::sin(angle);
    return Point2<T>(c * p.x - s * p.y, s * p.x + c * p.y);
}

/// Rotate a 2D vector by angle (radians, CCW).
template <typename T>
Vector2<T> rotateVector(T angle, const Vector2<T>& v)
{
    const auto c = std::cos(angle);
    const auto s = std::sin(angle);
    return Vector2<T>(c * v.x - s * v.y, s * v.x + c * v.y);
}

} // namespace

// ---------------------------------------------------------------------------
// Inversion
// ---------------------------------------------------------------------------

template <typename T>
Transform2<T> Transform2<T>::inverted() const
{
    // T = (θ, t)  =>  T⁻¹ = (-θ, -R(-θ) * t)
    Transform2<T> inv;
    inv.rotation = -rotation;

    // R(-θ) * t = R(θ)ᵀ * t
    const auto c    = std::cos(rotation); // cos(θ) = cos(-θ)
    const auto s    = std::sin(rotation); // sin(θ)
    // -R(-θ) * t = -(c·tx + s·ty,  -s·tx + c·ty)
    inv.translation = Point2<T>(-c * translation.x - s * translation.y, s * translation.x - c * translation.y);
    return inv;
}

template <typename T>
void Transform2<T>::invert()
{
    const auto old_t = translation;

    // R(-θ) * old_t
    const auto c = std::cos(rotation);
    const auto s = std::sin(rotation);
    translation  = Point2<T>(-c * old_t.x - s * old_t.y, s * old_t.x - c * old_t.y);
    rotation     = -rotation;
}

// ---------------------------------------------------------------------------
// Translation composition
// ---------------------------------------------------------------------------

template <typename T>
Transform2<T>& Transform2<T>::preTranslate(const Vector2<T>& dt)
{
    // T_trans * T = (θ, t + dt)
    translation.x += dt.x;
    translation.y += dt.y;
    return *this;
}

template <typename T>
Transform2<T>& Transform2<T>::postTranslate(const Vector2<T>& dt)
{
    // T * T_trans = (θ, R(θ) * dt + t)
    translation.x += std::cos(rotation) * dt.x - std::sin(rotation) * dt.y;
    translation.y += std::sin(rotation) * dt.x + std::cos(rotation) * dt.y;
    return *this;
}

// ---------------------------------------------------------------------------
// Rotation composition
// ---------------------------------------------------------------------------

template <typename T>
Transform2<T>& Transform2<T>::preRotate(T angle)
{
    // T_rot(α) * T = (α + θ, R(α) * t)
    if (angle == T(0)) {
        return *this;
    }
    translation = rotatePoint(angle, translation);
    rotation += angle;
    return *this;
}

// ---------------------------------------------------------------------------
// Transform composition
// ---------------------------------------------------------------------------

template <typename T>
Transform2<T> Transform2<T>::operator*(const Transform2<T>& right) const
{
    // (θ₁, t₁) * (θ₂, t₂) = (θ₁+θ₂, R(θ₁)*t₂ + t₁)
    Transform2<T> result;
    result.rotation    = rotation + right.rotation;
    result.translation = rotatePoint(rotation, right.translation);
    result.translation.x += translation.x;
    result.translation.y += translation.y;
    return result;
}

template <typename T>
Transform2<T>& Transform2<T>::operator*=(const Transform2<T>& right)
{
    *this = *this * right;
    return *this;
}

// ---------------------------------------------------------------------------
// Point / vector transformation (global operators)
// ---------------------------------------------------------------------------

template <typename T>
Point2<T> operator*(const Transform2<T>& t, const Point2<T>& p)
{
    // p' = R(θ) * p + t
    auto r = rotatePoint(t.rotation, p);
    r.x += t.translation.x;
    r.y += t.translation.y;
    return r;
}

template <typename T>
Vector2<T> operator*(const Transform2<T>& t, const Vector2<T>& v)
{
    // v' = R(θ) * v
    return rotateVector(t.rotation, v);
}

// ---------------------------------------------------------------------------
// Explicit instantiations
// ---------------------------------------------------------------------------

template class V_MATH_API Transform2<float>;
template class V_MATH_API Transform2<double>;
template V_MATH_API Point2<float> operator*(const Transform2<float>&, const Point2<float>&);
template V_MATH_API Point2<double> operator*(const Transform2<double>&, const Point2<double>&);
template V_MATH_API Vector2<float> operator*(const Transform2<float>&, const Vector2<float>&);
template V_MATH_API Vector2<double> operator*(const Transform2<double>&, const Vector2<double>&);

V_MATH_NS_END