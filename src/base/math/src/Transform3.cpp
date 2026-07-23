#include <vine/math/Transform3.hpp>

#include <cmath>

#include <vine/math/Math.hpp>

V_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

/* ========================================================================= */
/*  toQuaternion  –  extract rotation from upper-left 3x3                    */
/* ========================================================================= */

TMPL_PREFIX Quaternion<T> getRotation(const Matrix4x4<T>& m)
{
    using namespace math;

    if (!m.isAffine(EPS<T>()))
        return Quaternion<T>(T(0), T(0), T(0), T(1));

    const auto col0_len2 = m.vecs[0].asVector3().length2();
    const auto col1_len2 = m.vecs[1].asVector3().length2();
    const auto col2_len2 = m.vecs[2].asVector3().length2();

    if (math::isZero(col0_len2, EPS<T>()) || math::isZero(col1_len2, EPS<T>()) || math::isZero(col2_len2, EPS<T>()))
        return Quaternion<T>(T(0), T(0), T(0), T(1));

    const auto inv0 = T(1) / std::sqrt(col0_len2);
    const auto inv1 = T(1) / std::sqrt(col1_len2);
    const auto inv2 = T(1) / std::sqrt(col2_len2);

    const Vector3<T> cx(m.vecs[0][0] * inv0, m.vecs[0][1] * inv0, m.vecs[0][2] * inv0);
    const Vector3<T> cy(m.vecs[1][0] * inv1, m.vecs[1][1] * inv1, m.vecs[1][2] * inv1);
    const Vector3<T> cz(m.vecs[2][0] * inv2, m.vecs[2][1] * inv2, m.vecs[2][2] * inv2);

    if (!math::isZero(cx.dot(cy), EPS<T>()) || !math::isZero(cx.dot(cz), EPS<T>()) || !math::isZero(cy.dot(cz), EPS<T>()))
        return Quaternion<T>(T(0), T(0), T(0), T(1));

    const auto m00 = cx.x, m10 = cx.y, m20 = cx.z;
    const auto m01 = cy.x, m11 = cy.y, m21 = cy.z;
    const auto m02 = cz.x, m12 = cz.y, m22 = cz.z;
    const auto trace = m00 + m11 + m22;

    Quaternion<T> quat;
    if (trace > T(0)) {
        const auto s = T(2) * std::sqrt(trace + T(1));
        quat.w = T(0.25) * s;
        quat.x = (m21 - m12) / s;
        quat.y = (m02 - m20) / s;
        quat.z = (m10 - m01) / s;
    } else if (m00 > m11 && m00 > m22) {
        const auto s = T(2) * std::sqrt(T(1) + m00 - m11 - m22);
        quat.w = (m21 - m12) / s;
        quat.x = T(0.25) * s;
        quat.y = (m01 + m10) / s;
        quat.z = (m02 + m20) / s;
    } else if (m11 > m22) {
        const auto s = T(2) * std::sqrt(T(1) + m11 - m00 - m22);
        quat.w = (m02 - m20) / s;
        quat.x = (m01 + m10) / s;
        quat.y = T(0.25) * s;
        quat.z = (m12 + m21) / s;
    } else {
        const auto s = T(2) * std::sqrt(T(1) + m22 - m00 - m11);
        quat.w = (m10 - m01) / s;
        quat.x = (m02 + m20) / s;
        quat.y = (m12 + m21) / s;
        quat.z = T(0.25) * s;
    }

    const auto q_len2 = quat.x * quat.x + quat.y * quat.y + quat.z * quat.z + quat.w * quat.w;
    if (math::isZero(q_len2, EPS<T>()))
        return Quaternion<T>(T(0), T(0), T(0), T(1));
    const auto inv_len = T(1) / std::sqrt(q_len2);
    quat.x *= inv_len; quat.y *= inv_len; quat.z *= inv_len; quat.w *= inv_len;
    return quat;
}

/* ========================================================================= */
/*  rotate(quat)  –  canonical quaternion → rotation matrix                 */
/* ========================================================================= */

TMPL_PREFIX Matrix4x4<T> rotate(const Quaternion<T>& quat)
{
    auto       q      = quat;
    const auto q_len2 = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    if (q_len2 == T(0)) return Matrix4x4<T>{};
    if (!math::isEqual(q_len2, T(1), T(1e-12))) {
        const auto inv = T(1) / std::sqrt(q_len2);
        q.x *= inv; q.y *= inv; q.z *= inv; q.w *= inv;
    }

    const auto x  = q.x,  y  = q.y,  z  = q.z,  w  = q.w;
    const auto xx = x * x, yy = y * y, zz = z * z;
    const auto xy = x * y, zw = z * w, xz = x * z;
    const auto yw = y * w, yz = y * z, xw = x * w;

    Matrix4x4<T> m;
    m.vecs[0][0] = T(1) - T(2) * (yy + zz);
    m.vecs[0][1] = T(2) * (xy + zw);
    m.vecs[0][2] = T(2) * (xz - yw);
    m.vecs[0][3] = T(0);

    m.vecs[1][0] = T(2) * (xy - zw);
    m.vecs[1][1] = T(1) - T(2) * (xx + zz);
    m.vecs[1][2] = T(2) * (yz + xw);
    m.vecs[1][3] = T(0);

    m.vecs[2][0] = T(2) * (xz + yw);
    m.vecs[2][1] = T(2) * (yz - xw);
    m.vecs[2][2] = T(1) - T(2) * (xx + yy);
    m.vecs[2][3] = T(0);

    m.vecs[3][0] = T(0);
    m.vecs[3][1] = T(0);
    m.vecs[3][2] = T(0);
    m.vecs[3][3] = T(1);
    return m;
}

/* ========================================================================= */
/*  makeRotation  -  axis-angle  (Rodrigues)                                */
/* ========================================================================= */

TMPL_PREFIX void makeRotation(Matrix4x4<T>& m, const Vector3<T>& axis, T angle)
{
    if (angle == T(0) || math::isZero(axis.length2(), EPS<T>())) {
        m.makeIdentity();
        return;
    }

    auto naxis = axis;
    naxis.normalize();

    const auto c  = std::cos(angle);
    const auto ic = T(1) - c;
    const auto s  = std::sin(angle);
    const auto x  = naxis.x;
    const auto y  = naxis.y;
    const auto z  = naxis.z;

    m.vecs[0][0] = x * x * ic + c;
    m.vecs[0][1] = x * y * ic + z * s;
    m.vecs[0][2] = x * z * ic - y * s;
    m.vecs[0][3] = T(0);

    m.vecs[1][0] = x * y * ic - z * s;
    m.vecs[1][1] = y * y * ic + c;
    m.vecs[1][2] = y * z * ic + x * s;
    m.vecs[1][3] = T(0);

    m.vecs[2][0] = x * z * ic + y * s;
    m.vecs[2][1] = y * z * ic - x * s;
    m.vecs[2][2] = z * z * ic + c;
    m.vecs[2][3] = T(0);

    m.vecs[3][0] = T(0);
    m.vecs[3][1] = T(0);
    m.vecs[3][2] = T(0);
    m.vecs[3][3] = T(1);
}

/* ========================================================================= */
/*  makeLookAt                                                              */
/* ========================================================================= */

TMPL_PREFIX void makeLookAt(Matrix4x4<T>& m, const Point3<T>& eye,
                             const Point3<T>& target, const Vector3<T>& up)
{
    auto f = eye - target;
    f.normalize();

    auto s = up.cross(f);
    if (math::isZero(s.length2(), EPS<T>())) {
        const auto ref = (std::abs(f.x) < std::abs(f.y))
                             ? Vector3<T>(T(1), T(0), T(0))
                             : Vector3<T>(T(0), T(1), T(0));
        s = ref.cross(f);
    }
    s.normalize();

    auto u = f.cross(s);
    u.normalize();

    setBasis(m, eye, s, u, f);
    m.invert();
}

/* ========================================================================= */
/*  makeReflection                                                          */
/* ========================================================================= */

TMPL_PREFIX void makeReflection(Matrix4x4<T>& m, const Vector3<T>& plane_normal,
                                 T plane_offset)
{
    auto n = plane_normal;
    n.normalize();

    const auto a = n.x, b = n.y, c = n.z, d = plane_offset;

    m.vecs[0][0] = T(1) - T(2) * a * a;
    m.vecs[0][1] = T(-2) * a * b;
    m.vecs[0][2] = T(-2) * a * c;
    m.vecs[0][3] = T(0);

    m.vecs[1][0] = T(-2) * a * b;
    m.vecs[1][1] = T(1) - T(2) * b * b;
    m.vecs[1][2] = T(-2) * b * c;
    m.vecs[1][3] = T(0);

    m.vecs[2][0] = T(-2) * a * c;
    m.vecs[2][1] = T(-2) * b * c;
    m.vecs[2][2] = T(1) - T(2) * c * c;
    m.vecs[2][3] = T(0);

    m.vecs[3][0] = T(-2) * a * d;
    m.vecs[3][1] = T(-2) * b * d;
    m.vecs[3][2] = T(-2) * c * d;
    m.vecs[3][3] = T(1);
}

/* ========================================================================= */
/*  Explicit instantiations                                                 */
/* ========================================================================= */

template Quaternion<float> getRotation(const Matrix4x4<float>&);
template Quaternion<double> getRotation(const Matrix4x4<double>&);

#undef TMPL_PREFIX

template Matrix4x4<float> rotate(const Quaternion<float>&);
template Matrix4x4<double> rotate(const Quaternion<double>&);

template void makeRotation(Matrix4x4<float>&, const Vector3<float>&, float);
template void makeRotation(Matrix4x4<double>&, const Vector3<double>&, double);

template void makeLookAt(Matrix4x4<float>&, const Point3<float>&, const Point3<float>&, const Vector3<float>&);
template void makeLookAt(Matrix4x4<double>&, const Point3<double>&, const Point3<double>&, const Vector3<double>&);

template void makeReflection(Matrix4x4<float>&, const Vector3<float>&, float);
template void makeReflection(Matrix4x4<double>&, const Vector3<double>&, double);

V_MATH_NS_END
