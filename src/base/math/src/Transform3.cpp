#include <vine/math/Transform3.hpp>

#include <vine/math/Point3.hpp>
#include <vine/math/Vector3.hpp>

V_MATH_NS_BEGIN

// ---------------------------------------------------------------------------
// Inversion
// ---------------------------------------------------------------------------

template <typename T>
Transform3<T> Transform3<T>::inverted() const
{
    // T = (R, t)  =>  T⁻¹ = (Rᵀ, -Rᵀ * t)
    Transform3<T> inv;
    inv.rotation    = rotation.transposed();
    inv.translation = -(inv.rotation * translation);
    return inv;
}

template <typename T>
void Transform3<T>::invert()
{
    // T = (R, t)  =>  T⁻¹ = (Rᵀ, -Rᵀ * t)
    rotation.transpose();
    translation = -(rotation * translation);
}

// ---------------------------------------------------------------------------
// Translation composition
// ---------------------------------------------------------------------------

template <typename T>
Transform3<T>& Transform3<T>::preTranslate(const Vector3<T>& dt)
{
    // T := T_trans * T = (I, dt) * (R, t) = (R, t + dt)
    translation += dt;
    return *this;
}

template <typename T>
Transform3<T>& Transform3<T>::postTranslate(const Vector3<T>& dt)
{
    // T := T * T_trans = (R, t) * (I, dt) = (R, R * dt + t)
    translation += rotation * dt;
    return *this;
}

// ---------------------------------------------------------------------------
// Rotation composition
// ---------------------------------------------------------------------------

template <typename T>
Transform3<T>& Transform3<T>::preRotate(const Rotation3<T>& r)
{
    // T := T_rot * T = (r, 0) * (R, t) = (r * R, r * t)
    translation = r * translation;
    rotation    = r * rotation;
    return *this;
}

template <typename T>
Transform3<T>& Transform3<T>::postRotate(const Rotation3<T>& r)
{
    // T := T * T_rot = (R, t) * (r, 0) = (R * r, t)
    rotation = rotation * r;
    return *this;
}

// ---------------------------------------------------------------------------
// Transform composition
// ---------------------------------------------------------------------------

template <typename T>
Transform3<T> Transform3<T>::operator*(const Transform3<T>& right) const
{
    // (R₁, t₁) * (R₂, t₂) = (R₁ * R₂,  R₁ * t₂ + t₁)
    Transform3<T> result;
    result.rotation    = rotation * right.rotation;
    result.translation = translation;
    result.translation += rotation * right.translation.asVector();
    return result;
}

template <typename T>
Transform3<T>& Transform3<T>::operator*=(const Transform3<T>& right)
{
    *this = *this * right;
    return *this;
}

// ---------------------------------------------------------------------------
// Point / vector transformation
// ---------------------------------------------------------------------------

template <typename T>
Point3<T> operator*(const Transform3<T>& t, const Point3<T>& p)
{
    // p' = R * p + t
    const auto rotated = t.rotation * p;
    return Point3<T>(rotated.x + t.translation.x, rotated.y + t.translation.y, rotated.z + t.translation.z);
}

template <typename T>
Vector3<T> operator*(const Transform3<T>& t, const Vector3<T>& v)
{
    // v' = R * v  (pure rotation, no translation)
    return t.rotation * v;
}

// ---------------------------------------------------------------------------
// Explicit instantiations
// ---------------------------------------------------------------------------

template class V_MATH_API Transform3<float>;
template class V_MATH_API Transform3<double>;
template V_MATH_API Point3<float> operator*(const Transform3<float>&, const Point3<float>&);
template V_MATH_API Point3<double> operator*(const Transform3<double>&, const Point3<double>&);
template V_MATH_API Vector3<float> operator*(const Transform3<float>&, const Vector3<float>&);
template V_MATH_API Vector3<double> operator*(const Transform3<double>&, const Vector3<double>&);

V_MATH_NS_END