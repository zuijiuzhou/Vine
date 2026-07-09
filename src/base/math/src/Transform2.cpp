#include <vine/math/Transform2.hpp>

#include <cmath>

V_MATH_NS_BEGIN

// ---------------------------------------------------------------------------
// Inversion
// ---------------------------------------------------------------------------

template <typename T>
Transform2<T> Transform2<T>::inverted() const
{
    // T = (R, t)  =>  T⁻¹ = (Rᵀ, -Rᵀ * t)
    Transform2<T> inv;
    inv.rotation    = rotation.transposed();
    inv.translation = -(inv.rotation * translation);
    return inv;
}

template <typename T>
void Transform2<T>::invert()
{
    // T = (R, t)  =>  T⁻¹ = (Rᵀ, -Rᵀ * t)
    rotation.transpose();
    translation = -(rotation * translation);
}

// ---------------------------------------------------------------------------
// Translation composition
// ---------------------------------------------------------------------------

template <typename T>
Transform2<T>& Transform2<T>::preTranslate(const Vector2<T>& dt)
{
    // T_trans * T = (R, t + dt)
    translation.x += dt.x;
    translation.y += dt.y;
    return *this;
}

template <typename T>
Transform2<T>& Transform2<T>::postTranslate(const Vector2<T>& dt)
{
    // T * T_trans = (R, R * dt + t)
    translation += rotation * dt;
    return *this;
}

// ---------------------------------------------------------------------------
// Rotation composition
// ---------------------------------------------------------------------------

template <typename T>
Transform2<T>& Transform2<T>::preRotate(const Rotation2<T>& r)
{
    // T_rot * T = (r * R, r * t)
    translation = r * translation;
    rotation    = r * rotation;
    return *this;
}

template <typename T>
Transform2<T>& Transform2<T>::postRotate(const Rotation2<T>& r)
{
    // T * T_rot = (R * r, t)
    rotation = rotation * r;
    return *this;
}

// ---------------------------------------------------------------------------
// Transform composition
// ---------------------------------------------------------------------------

template <typename T>
Transform2<T> Transform2<T>::operator*(const Transform2<T>& right) const
{
    // (R₁, t₁) * (R₂, t₂) = (R₁ * R₂,  R₁ * t₂ + t₁)
    Transform2<T> result;
    result.rotation    = rotation * right.rotation;
    result.translation = translation;
    result.translation += (rotation * right.translation).asVector();
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
    // p' = R * p + t
    auto r = t.rotation * p;
    r.x += t.translation.x;
    r.y += t.translation.y;
    return r;
}

template <typename T>
Vector2<T> operator*(const Transform2<T>& t, const Vector2<T>& v)
{
    // v' = R * v  (pure rotation, no translation)
    return t.rotation * v;
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