#include <vine/math/Rotation3.hpp>

#include <cmath>

#include <vine/math/Math.hpp>

V_MATH_NS_BEGIN

template <typename T>
Rotation3<T>::Rotation3(const Quaternion3<T>& quat) noexcept
{ fromQuaternion(quat); }

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
    // In our column-major layout:
    //   Column 0 (X axis)  = [R₀₀, R₁₀, R₂₀]
    //   Column 1 (Y axis)  = [R₀₁, R₁₁, R₂₁]
    //   Column 2 (Z axis)  = [R₀₂, R₁₂, R₂₂]

    // Normalize defensively; non-unit quaternions would inject scale.
    auto       q      = quat;
    const auto q_len2 = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    if (q_len2 == T(0)) {
        *this = Rotation3<T>();
        return;
    }
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

// Explicit template instantiations.
template class V_MATH_API Rotation3<float>;
template class V_MATH_API Rotation3<double>;

V_MATH_NS_END