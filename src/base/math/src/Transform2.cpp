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
Transform2<T>& Transform2<T>::preRotate(T angle)
{
    const auto c = std::cos(angle);
    const auto s = std::sin(angle);

    // translation = r * translation  (rotate the translation vector)
    const auto tx = translation.x;
    const auto ty = translation.y;
    translation.x = c * tx - s * ty;
    translation.y = s * tx + c * ty;

    // rotation = r * rotation = R(θ + α), only compute first column
    const auto new_m00 = rotation.m00 * c + rotation.m01 * s;
    const auto new_m10 = rotation.m10 * c + rotation.m11 * s;
    rotation.m00 = new_m00;
    rotation.m10 = new_m10;
    rotation.m01 = -new_m10;  // derived: -sin(θ+α)
    rotation.m11 = new_m00;   // derived:  cos(θ+α)

    return *this;
}

template <typename T>
Transform2<T>& Transform2<T>::postRotate(const Rotation2<T>& r)
{
    // T * T_rot = (R * r, t)
    rotation = rotation * r;
    return *this;
}

template <typename T>
Transform2<T>& Transform2<T>::postRotate(T angle)
{
    const auto c = std::cos(angle);
    const auto s = std::sin(angle);

    // rotation = rotation * R(angle) = R(θ + α), only compute first column
    const auto new_m00 = rotation.m00 * c + rotation.m01 * s;
    const auto new_m10 = rotation.m10 * c + rotation.m11 * s;
    rotation.m00 = new_m00;
    rotation.m10 = new_m10;
    rotation.m01 = -new_m10;  // derived: -sin(θ+α)
    rotation.m11 = new_m00;   // derived:  cos(θ+α)

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