#include <vine/math/Rotation3.hpp>

#include <cmath>

#include <vine/math/Math.hpp>
#include <vine/math/Point3.hpp>
#include <vine/math/Vector3.hpp>

V_MATH_NS_BEGIN

template <typename T>
Rotation3<T>::Rotation3(const Quaternion3<T>& quat) noexcept
{
    fromQuaternion(quat);
}

template <typename T>
Quaternion3<T> Rotation3<T>::toQuaternion() const
{
    // Extract a unit quaternion from a 3x3 rotation matrix.
    //
    // The columns of the rotation matrix are:
    //   col0 = [R00, R10, R20]^T
    //   col1 = [R01, R11, R21]^T
    //   col2 = [R02, R12, R22]^T
    //
    // Using the standard quaternion-to-matrix formula:
    //   R10 - R01 = 4zw,  R02 - R20 = 4yw,  R21 - R12 = 4xw
    // we reconstruct w first from the trace, then x, y, z.
    //
    // The trace-based branching keeps the computation stable near 180°
    // rotations where one component dominates.

    const auto trace = m00 + m11 + m22;

    Quaternion3<T> quat;
    if (trace > T(0)) {
        const auto s = T(2) * std::sqrt(trace + T(1));
        quat.w       = T(0.25) * s;
        quat.x       = (m21 - m12) / s;
        quat.y       = (m02 - m20) / s;
        quat.z       = (m10 - m01) / s;
    }
    else if (m00 > m11 && m00 > m22) {
        const auto s = T(2) * std::sqrt(T(1) + m00 - m11 - m22);
        quat.w       = (m21 - m12) / s;
        quat.x       = T(0.25) * s;
        quat.y       = (m01 + m10) / s;
        quat.z       = (m02 + m20) / s;
    }
    else if (m11 > m22) {
        const auto s = T(2) * std::sqrt(T(1) + m11 - m00 - m22);
        quat.w       = (m02 - m20) / s;
        quat.x       = (m01 + m10) / s;
        quat.y       = T(0.25) * s;
        quat.z       = (m12 + m21) / s;
    }
    else {
        const auto s = T(2) * std::sqrt(T(1) + m22 - m00 - m11);
        quat.w       = (m10 - m01) / s;
        quat.x       = (m02 + m20) / s;
        quat.y       = (m12 + m21) / s;
        quat.z       = T(0.25) * s;
    }

    // Normalize the extracted quaternion.
    const auto q_len2 = quat.x * quat.x + quat.y * quat.y + quat.z * quat.z + quat.w * quat.w;
    if (math::isZero(q_len2, EPS<T>())) {
        quat.x = T(0);
        quat.y = T(0);
        quat.z = T(0);
        quat.w = T(1);
    }
    else {
        const auto inv_len = T(1) / std::sqrt(q_len2);
        quat.x *= inv_len;
        quat.y *= inv_len;
        quat.z *= inv_len;
        quat.w *= inv_len;
    }

    return quat;
}

template <typename T>
void Rotation3<T>::fromQuaternion(const Quaternion3<T>& quat)
{
    // Convert unit quaternion to column-major 3x3 rotation matrix.
    //
    // Standard formula for q = (x, y, z, w):
    //   R = | 1-2(y²+z²)   2(xy-zw)     2(xz+yw) |
    //       | 2(xy+zw)     1-2(x²+z²)   2(yz-xw) |
    //       | 2(xz-yw)     2(yz+xw)     1-2(x²+y²) |
    //
    // This form comes from quaternion multiplication and avoids trig calls.
    //
    // In our column-major layout:
    //   Column 0 (X axis)  = [R₀₀, R₁₀, R₂₀]
    //   Column 1 (Y axis)  = [R₀₁, R₁₁, R₂₁]
    //   Column 2 (Z axis)  = [R₀₂, R₁₂, R₂₂]

    // Normalize defensively; non-unit quaternions would inject scale.
    auto       q      = quat;
    const auto q_len2 = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    // Zero-length quaternion is invalid; treat as identity rotation.
    if (q_len2 == T(0)) {
        *this = Rotation3<T>();
        return;
    }
    // If the quaternion is not unit-length, normalize it to avoid scaling the rotation.
    if (!math::isEqual(q_len2, T(1), T(1e-12))) {
        const auto inv_q_len = T(1) / std::sqrt(q_len2);
        q.x *= inv_q_len;
        q.y *= inv_q_len;
        q.z *= inv_q_len;
        q.w *= inv_q_len;
    }

    const auto x  = q.x;
    const auto y  = q.y;
    const auto z  = q.z;
    const auto w  = q.w;
    const auto xx = x * x;
    const auto yy = y * y;
    const auto zz = z * z;
    const auto xy = x * y;
    const auto zw = z * w;
    const auto xz = x * z;
    const auto yw = y * w;
    const auto yz = y * z;
    const auto xw = x * w;

    // Column 0 (vec0 / X axis)
    m00 = T(1) - T(2) * (yy + zz);
    m10 = T(2) * (xy + zw);
    m20 = T(2) * (xz - yw);

    // Column 1 (vec1 / Y axis)
    m01 = T(2) * (xy - zw);
    m11 = T(1) - T(2) * (xx + zz);
    m21 = T(2) * (yz + xw);

    // Column 2 (vec2 / Z axis)
    m02 = T(2) * (xz + yw);
    m12 = T(2) * (yz - xw);
    m22 = T(1) - T(2) * (xx + yy);
}

template <typename T>
void Rotation3<T>::transpose()
{
    // Rotation matrix convention:
    //
    // R maps coordinates from the child frame to the parent frame.
    // The columns of R are the child frame axes expressed in the parent frame.
    //
    // R:
    //   Column 0 -> child X axis in parent coordinates
    //   Column 1 -> child Y axis in parent coordinates
    //   Column 2 -> child Z axis in parent coordinates
    //
    // Rᵀ is the inverse rotation:
    // It maps vectors from the parent frame to the child frame.
    // Each row of Rᵀ represents the projection direction of a parent axis
    // onto the child frame.

    std::swap(m01, m10);
    std::swap(m02, m20);
    std::swap(m12, m21);
}

template <typename T>
Rotation3<T> Rotation3<T>::transposed() const
{
    Rotation3<T> result = *this;
    result.transpose();
    return result;
}

template <typename T>
Rotation3<T> Rotation3<T>::operator*(const Rotation3<T>& other) const
{
    // Column-major 3x3 matrix multiplication: C = A * B
    // Column j of C = A * (column j of B)
    //              = B[0][j] * A.col0 + B[1][j] * A.col1 + B[2][j] * A.col2
    Rotation3<T> result;
    result.vecs[0] = vecs[0] * other.vecs[0].x + vecs[1] * other.vecs[0].y + vecs[2] * other.vecs[0].z;
    result.vecs[1] = vecs[0] * other.vecs[1].x + vecs[1] * other.vecs[1].y + vecs[2] * other.vecs[1].z;
    result.vecs[2] = vecs[0] * other.vecs[2].x + vecs[1] * other.vecs[2].y + vecs[2] * other.vecs[2].z;
    return result;
}

template <typename T>
Rotation3<T>& Rotation3<T>::operator*=(const Rotation3<T>& other)
{
    // In-place column-major 3x3 multiplication: A = A * B
    // Save original columns before overwriting, since each column of
    // the result depends on all three original columns.
    const Vector3<T> a0 = vecs[0];
    const Vector3<T> a1 = vecs[1];
    const Vector3<T> a2 = vecs[2];

    // Column j of result = B[0][j] * A.col0 + B[1][j] * A.col1 + B[2][j] * A.col2
    vecs[0] = a0 * other.vecs[0].x + a1 * other.vecs[0].y + a2 * other.vecs[0].z;
    vecs[1] = a0 * other.vecs[1].x + a1 * other.vecs[1].y + a2 * other.vecs[1].z;
    vecs[2] = a0 * other.vecs[2].x + a1 * other.vecs[2].y + a2 * other.vecs[2].z;

    return *this;
}

template <typename T>
Point3<T> operator*(const Rotation3<T>& left, const Point3<T>& p)
{
    // p' = R * p = p.x * col0(R) + p.y * col1(R) + p.z * col2(R)
    const auto rotated = left.vecs[0] * p.x + left.vecs[1] * p.y + left.vecs[2] * p.z;
    return Point3<T>(rotated.x, rotated.y, rotated.z);
}

template <typename T>
Vector3<T> operator*(const Rotation3<T>& left, const Vector3<T>& v)
{
    // v' = R * v = v.x * col0(R) + v.y * col1(R) + v.z * col2(R)
    return left.vecs[0] * v.x + left.vecs[1] * v.y + left.vecs[2] * v.z;
}

// Explicit template instantiations.
template class V_MATH_API Rotation3<float>;
template class V_MATH_API Rotation3<double>;

template V_MATH_API Point3<float> operator*(const Rotation3<float>&, const Point3<float>&);
template V_MATH_API Point3<double> operator*(const Rotation3<double>&, const Point3<double>&);
template V_MATH_API Vector3<float> operator*(const Rotation3<float>&, const Vector3<float>&);
template V_MATH_API Vector3<double> operator*(const Rotation3<double>&, const Vector3<double>&);

V_MATH_NS_END