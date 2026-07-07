#include <vine/math/Transform3.hpp>

#include <vine/math/Point3.hpp>
#include <vine/math/Vector3.hpp>

V_MATH_NS_BEGIN

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

namespace
{

/// Rotate a vector by a column-major 3x3 rotation matrix.
/// result = R * v = v.x * col0(R) + v.y * col1(R) + v.z * col2(R)
template <typename T>
Vector3<T> rotateVector(const Rotation3<T>& r, const Vector3<T>& v)
{
    return r.vecs[0] * v.x + r.vecs[1] * v.y + r.vecs[2] * v.z;
}

/// Compose two rotation matrices: C = A * B (column-major).
template <typename T>
Rotation3<T> composeRotation(const Rotation3<T>& a, const Rotation3<T>& b)
{
    Rotation3<T> c;
    // Column j of C = A * (column j of B)
    //              = b.vecs[j].x * a.vecs[0] + b.vecs[j].y * a.vecs[1] + b.vecs[j].z * a.vecs[2]
    c.vecs[0] = a.vecs[0] * b.vecs[0].x + a.vecs[1] * b.vecs[0].y + a.vecs[2] * b.vecs[0].z;
    c.vecs[1] = a.vecs[0] * b.vecs[1].x + a.vecs[1] * b.vecs[1].y + a.vecs[2] * b.vecs[1].z;
    c.vecs[2] = a.vecs[0] * b.vecs[2].x + a.vecs[1] * b.vecs[2].y + a.vecs[2] * b.vecs[2].z;
    return c;
}

} // namespace

// ---------------------------------------------------------------------------
// Inversion
// ---------------------------------------------------------------------------

template <typename T>
Transform3<T> Transform3<T>::inverted() const
{
    // T = (R, t)  =>  T⁻¹ = (Rᵀ, -Rᵀ * t)
    Transform3<T> inv;

    // Rᵀ: transpose rotation in-place on inv (swap off-diagonals).
    inv.rotation.m00 = rotation.m00;
    inv.rotation.m10 = rotation.m01;
    inv.rotation.m20 = rotation.m02;
    inv.rotation.m01 = rotation.m10;
    inv.rotation.m11 = rotation.m11;
    inv.rotation.m21 = rotation.m12;
    inv.rotation.m02 = rotation.m20;
    inv.rotation.m12 = rotation.m21;
    inv.rotation.m22 = rotation.m22;

    // -Rᵀ * t
    inv.translation = Point3<T>();
    inv.translation -= rotateVector(inv.rotation, translation.asVector());
    return inv;
}

template <typename T>
void Transform3<T>::invert()
{
    // T = (R, t)  =>  T⁻¹ = (Rᵀ, -Rᵀ * t)
    //
    // Perform in-place to avoid temporary allocations:
    //   1) Save the original translation (needed after R is transposed).
    //   2) Transpose R in-place (only off-diagonals need swapping).
    //   3) Compute new translation = -(Rᵀ * old_t).

    const auto old_t = translation;

    // Transpose rotation in-place: swap 3 off-diagonal pairs.
    std::swap(rotation.m01, rotation.m10);
    std::swap(rotation.m02, rotation.m20);
    std::swap(rotation.m12, rotation.m21);

    // translation = -Rᵀ * old_t
    const auto rotated = rotateVector(rotation, old_t.asVector());
    translation        = Point3<T>(-rotated.x, -rotated.y, -rotated.z);
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
    translation += rotateVector(rotation, dt);
    return *this;
}

// ---------------------------------------------------------------------------
// Rotation composition
// ---------------------------------------------------------------------------

template <typename T>
Transform3<T>& Transform3<T>::preRotate(const Rotation3<T>& r)
{
    // T := T_rot * T = (r, 0) * (R, t) = (r * R, r * t)
    translation = Point3<T>();
    translation += rotateVector(r, translation.asVector());
    rotation = composeRotation(r, rotation);
    return *this;
}

template <typename T>
Transform3<T>& Transform3<T>::postRotate(const Rotation3<T>& r)
{
    // T := T * T_rot = (R, t) * (r, 0) = (R * r, t)
    rotation = composeRotation(rotation, r);
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
    result.rotation    = composeRotation(rotation, right.rotation);
    result.translation = translation;
    result.translation += rotateVector(rotation, right.translation.asVector());
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
    const auto rotated = rotateVector(t.rotation, p.asVector());
    return Point3<T>(rotated.x + t.translation.x, rotated.y + t.translation.y, rotated.z + t.translation.z);
}

template <typename T>
Vector3<T> operator*(const Transform3<T>& t, const Vector3<T>& v)
{
    // v' = R * v  (pure rotation, no translation)
    return rotateVector(t.rotation, v);
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