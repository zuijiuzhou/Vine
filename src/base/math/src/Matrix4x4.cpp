#include <vine/math/Matrix4x4.hpp>

#include <cmath>
#include <cstring>
#include <utility>

#include <vine/math/Math.hpp>
#include <vine/math/Point3.hpp>
#include <vine/math/Vector3.hpp>

V_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Matrix4x4<T>& Matrix4x4<T>::preMulti(const Matrix4x4<T>& left)
{
    // Pre-multiply: M := left * M.
    // With column-major storage vecs[col][row], each new column is:
    // new_col = left * old_col.
    T old[4][4];
    std::memcpy(old, vecs, sizeof(old));

    for (size_t col = 0; col < 4; ++col) {
        for (size_t row = 0; row < 4; ++row) {
            T v = T(0);
            for (size_t k = 0; k < 4; ++k) {
                v += left.vecs[k][row] * old[col][k];
            }
            vecs[col][row] = v;
        }
    }

    return *this;
}

TMPL_PREFIX Matrix4x4<T>& Matrix4x4<T>::postMulti(const Matrix4x4<T>& right)
{
    // Post-multiply: M := M * right.
    // In column-major form, each output column is a linear combination
    // of old columns weighted by the corresponding column of right.
    T old[4][4];
    std::memcpy(old, vecs, sizeof(old));

    for (size_t col = 0; col < 4; ++col) {
        for (size_t row = 0; row < 4; ++row) {
            T v = T(0);
            for (size_t k = 0; k < 4; ++k) {
                v += old[k][row] * right.vecs[col][k];
            }
            vecs[col][row] = v;
        }
    }

    return *this;
}

TMPL_PREFIX T Matrix4x4<T>::determinant() const
{
    // Compute the determinant of a 4x4 matrix.
    // This is needed for inversion and can be used to detect singularity.
    // The formula is derived from expansion by minors and is optimized
    // to minimize redundant calculations.

    const auto a2323 = vecs[2][2] * vecs[3][3] - vecs[2][3] * vecs[3][2];
    const auto a1323 = vecs[2][1] * vecs[3][3] - vecs[2][3] * vecs[3][1];
    const auto a1223 = vecs[2][1] * vecs[3][2] - vecs[2][2] * vecs[3][1];
    const auto a0323 = vecs[2][0] * vecs[3][3] - vecs[2][3] * vecs[3][0];
    const auto a0223 = vecs[2][0] * vecs[3][2] - vecs[2][2] * vecs[3][0];
    const auto a0123 = vecs[2][0] * vecs[3][1] - vecs[2][1] * vecs[3][0];

    return (vecs[0][0] * (vecs[1][1] * a2323 - vecs[1][2] * a1323 + vecs[1][3] * a1223) -
            vecs[0][1] * (vecs[1][0] * a2323 - vecs[1][2] * a0323 + vecs[1][3] * a0223) +
            vecs[0][2] * (vecs[1][0] * a1323 - vecs[1][1] * a0323 + vecs[1][3] * a0123) -
            vecs[0][3] * (vecs[1][0] * a1223 - vecs[1][1] * a0223 + vecs[1][2] * a0123));
}

TMPL_PREFIX bool Matrix4x4<T>::invert()
{
    // Invert with the adjugate method:
    // M^(-1) = adj(M) / det(M)
    // where adj(M) = C^T, and C is the cofactor matrix.
    //
    // For 4x4, compute reused minors first,
    // then assemble the adjugate with checkerboard signs.

    // Reused 2x2 determinants (building blocks for 3x3 minors).
    const T a2323 = vecs[2][2] * vecs[3][3] - vecs[2][3] * vecs[3][2];
    const T a1323 = vecs[2][1] * vecs[3][3] - vecs[2][3] * vecs[3][1];
    const T a1223 = vecs[2][1] * vecs[3][2] - vecs[2][2] * vecs[3][1];
    const T a0323 = vecs[2][0] * vecs[3][3] - vecs[2][3] * vecs[3][0];
    const T a0223 = vecs[2][0] * vecs[3][2] - vecs[2][2] * vecs[3][0];
    const T a0123 = vecs[2][0] * vecs[3][1] - vecs[2][1] * vecs[3][0];
    const T a2313 = vecs[1][2] * vecs[3][3] - vecs[1][3] * vecs[3][2];
    const T a1313 = vecs[1][1] * vecs[3][3] - vecs[1][3] * vecs[3][1];
    const T a1213 = vecs[1][1] * vecs[3][2] - vecs[1][2] * vecs[3][1];
    const T a2312 = vecs[1][2] * vecs[2][3] - vecs[1][3] * vecs[2][2];
    const T a1312 = vecs[1][1] * vecs[2][3] - vecs[1][3] * vecs[2][1];
    const T a1212 = vecs[1][1] * vecs[2][2] - vecs[1][2] * vecs[2][1];
    const T a0313 = vecs[1][0] * vecs[3][3] - vecs[1][3] * vecs[3][0];
    const T a0213 = vecs[1][0] * vecs[3][2] - vecs[1][2] * vecs[3][0];
    const T a0312 = vecs[1][0] * vecs[2][3] - vecs[1][3] * vecs[2][0];
    const T a0212 = vecs[1][0] * vecs[2][2] - vecs[1][2] * vecs[2][0];
    const T a0113 = vecs[1][0] * vecs[3][1] - vecs[1][1] * vecs[3][0];
    const T a0112 = vecs[1][0] * vecs[2][1] - vecs[1][1] * vecs[2][0];

    // Build adjugate matrix.
    // adj[i][j] = (-1)^(i+j) * minor[j][i] (note index swap due to transpose)
    Matrix4x4<T> adj;

    adj.vecs[0][0] = (vecs[1][1] * a2323 - vecs[1][2] * a1323 + vecs[1][3] * a1223);
    adj.vecs[0][1] = -(vecs[0][1] * a2323 - vecs[0][2] * a1323 + vecs[0][3] * a1223);
    adj.vecs[0][2] = (vecs[0][1] * a2313 - vecs[0][2] * a1313 + vecs[0][3] * a1213);
    adj.vecs[0][3] = -(vecs[0][1] * a2312 - vecs[0][2] * a1312 + vecs[0][3] * a1212);

    adj.vecs[1][0] = -(vecs[1][0] * a2323 - vecs[1][2] * a0323 + vecs[1][3] * a0223);
    adj.vecs[1][1] = (vecs[0][0] * a2323 - vecs[0][2] * a0323 + vecs[0][3] * a0223);
    adj.vecs[1][2] = -(vecs[0][0] * a2313 - vecs[0][2] * a0313 + vecs[0][3] * a0213);
    adj.vecs[1][3] = (vecs[0][0] * a2312 - vecs[0][2] * a0312 + vecs[0][3] * a0212);

    adj.vecs[2][0] = (vecs[1][0] * a1323 - vecs[1][1] * a0323 + vecs[1][3] * a0123);
    adj.vecs[2][1] = -(vecs[0][0] * a1323 - vecs[0][1] * a0323 + vecs[0][3] * a0123);
    adj.vecs[2][2] = (vecs[0][0] * a1313 - vecs[0][1] * a0313 + vecs[0][3] * a0113);
    adj.vecs[2][3] = -(vecs[0][0] * a1312 - vecs[0][1] * a0312 + vecs[0][3] * a0112);

    adj.vecs[3][0] = -(vecs[1][0] * a1223 - vecs[1][1] * a0223 + vecs[1][2] * a0123);
    adj.vecs[3][1] = (vecs[0][0] * a1223 - vecs[0][1] * a0223 + vecs[0][2] * a0123);
    adj.vecs[3][2] = -(vecs[0][0] * a1213 - vecs[0][1] * a0213 + vecs[0][2] * a0113);
    adj.vecs[3][3] = (vecs[0][0] * a1212 - vecs[0][1] * a0212 + vecs[0][2] * a0112);

    // Determinant expansion on row 0.
    const T det = vecs[0][0] * adj.vecs[0][0] + vecs[0][1] * adj.vecs[1][0] + vecs[0][2] * adj.vecs[2][0] + vecs[0][3] * adj.vecs[3][0];

    // Singular matrix (det == 0): keep original matrix unchanged.
    if (math::isZero(det, EPS<T>())) {
        return false;
    }

    // M^(-1) = (1/det) * adj(M)
    const T inv_det = T(1) / det;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            vecs[i][j] = adj.vecs[i][j] * inv_det;
        }
    }

    return true;
}

TMPL_PREFIX bool Matrix4x4<T>::isIdentity(T eps) const
{
    // Check if matrix is identity within tolerance.
    // This handles cases like M * M^(-1) which may have slight errors.
    return math::isEqual(vecs[0][0], T(1), eps) && math::isZero(vecs[0][1], eps) && math::isZero(vecs[0][2], eps) && math::isZero(vecs[0][3], eps) &&
           math::isZero(vecs[1][0], eps) && math::isEqual(vecs[1][1], T(1), eps) && math::isZero(vecs[1][2], eps) && math::isZero(vecs[1][3], eps) &&
           math::isZero(vecs[2][0], eps) && math::isZero(vecs[2][1], eps) && math::isEqual(vecs[2][2], T(1), eps) && math::isZero(vecs[2][3], eps) &&
           math::isZero(vecs[3][0], eps) && math::isZero(vecs[3][1], eps) && math::isZero(vecs[3][2], eps) && math::isEqual(vecs[3][3], T(1), eps);
}

TMPL_PREFIX bool Matrix4x4<T>::isAffine(T eps) const
{
    // Check if bottom row is [0 0 0 1] within tolerance.
    // Use epsilon to tolerate accumulated floating-point errors.
    return math::isZero(vecs[0][3], eps) && math::isZero(vecs[1][3], eps) && math::isZero(vecs[2][3], eps) && math::isEqual(vecs[3][3], T(1), eps);
}

TMPL_PREFIX bool Matrix4x4<T>::isRigid(T eps) const
{
    // Rigid transform check (rotation + translation only, no scale/shear):
    // 1) affine bottom row [0 0 0 1]
    // 2) first 3 columns are unit-length vectors
    // 3) first 3 columns are pairwise orthogonal

    if (!isAffine(eps))
        return false;

    // Extract basis vectors from the first 3 columns.
    const Vector3<T>& x = vecs[0].asVector3();
    const Vector3<T>& y = vecs[1].asVector3();
    const Vector3<T>& z = vecs[2].asVector3();

    // Unit-length constraints.
    // Use squared lengths to avoid sqrt. If |l-1| <= eps, then
    // |l^2-1| = |(l-1)(l+1)| <= eps * (2 + eps) = 2*eps + eps^2.
    const auto len2_x  = x.length2();
    const auto len2_y  = y.length2();
    const auto len2_z  = z.length2();
    const auto eps_len = T(2) * eps + eps * eps;

    // Check if each basis vector is unit length within tolerance. This allows for small numerical errors while rejecting significant scale.
    if (!math::isEqual(len2_x, T(1), eps_len) || !math::isEqual(len2_y, T(1), eps_len) || !math::isEqual(len2_z, T(1), eps_len)) {
        return false;
    }

    // Orthogonality constraints.
    // x·y = 0, y·z = 0, z·x = 0
    if (!math::isZero(x.dot(y), eps) || !math::isZero(y.dot(z), eps) || !math::isZero(z.dot(x), eps)) {
        return false;
    }

    // Enforce proper rotation (right-handed frame) and reject reflection.
    // For pure rotation basis: x x y = z and det(R) = +1.
    const auto rhs = x.cross(y).dot(z);
    if (!math::isEqual(rhs, T(1), eps)) {
        return false;
    }

    return true;
}

TMPL_PREFIX bool Matrix4x4<T>::isEqual(const Matrix4x4<T>& other, T eps) const
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (!math::isEqual(vecs[i][j], other.vecs[i][j], eps)) {
                return false;
            }
        }
    }
    return true;
}

TMPL_PREFIX bool Matrix4x4<T>::isZero(T eps) const
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (!math::isZero(vecs[i][j], eps)) {
                return false;
            }
        }
    }
    return true;
}

TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& right) const
{
    Matrix4x4<T> m;

    // for (int col = 0; col < 4; ++col) {
    //     for (int row = 0; row < 4; ++row) {
    //         m.vecs[col][row] = 0;
    //         for (int k = 0; k < 4; ++k) {
    //             m.vecs[col][row] += vecs[k][row] * right.vecs[col][k];
    //         }
    //     }
    // }

    // m1 * m2 * m3 = m3T * m2T * m1T

    m.vecs[0][0] = vecs[0][0] * right.vecs[0][0] + vecs[1][0] * right.vecs[0][1] + vecs[2][0] * right.vecs[0][2] + vecs[3][0] * right.vecs[0][3];
    m.vecs[0][1] = vecs[0][1] * right.vecs[0][0] + vecs[1][1] * right.vecs[0][1] + vecs[2][1] * right.vecs[0][2] + vecs[3][1] * right.vecs[0][3];
    m.vecs[0][2] = vecs[0][2] * right.vecs[0][0] + vecs[1][2] * right.vecs[0][1] + vecs[2][2] * right.vecs[0][2] + vecs[3][2] * right.vecs[0][3];
    m.vecs[0][3] = vecs[0][3] * right.vecs[0][0] + vecs[1][3] * right.vecs[0][1] + vecs[2][3] * right.vecs[0][2] + vecs[3][3] * right.vecs[0][3];

    m.vecs[1][0] = vecs[0][0] * right.vecs[1][0] + vecs[1][0] * right.vecs[1][1] + vecs[2][0] * right.vecs[1][2] + vecs[3][0] * right.vecs[1][3];
    m.vecs[1][1] = vecs[0][1] * right.vecs[1][0] + vecs[1][1] * right.vecs[1][1] + vecs[2][1] * right.vecs[1][2] + vecs[3][1] * right.vecs[1][3];
    m.vecs[1][2] = vecs[0][2] * right.vecs[1][0] + vecs[1][2] * right.vecs[1][1] + vecs[2][2] * right.vecs[1][2] + vecs[3][2] * right.vecs[1][3];
    m.vecs[1][3] = vecs[0][3] * right.vecs[1][0] + vecs[1][3] * right.vecs[1][1] + vecs[2][3] * right.vecs[1][2] + vecs[3][3] * right.vecs[1][3];

    m.vecs[2][0] = vecs[0][0] * right.vecs[2][0] + vecs[1][0] * right.vecs[2][1] + vecs[2][0] * right.vecs[2][2] + vecs[3][0] * right.vecs[2][3];
    m.vecs[2][1] = vecs[0][1] * right.vecs[2][0] + vecs[1][1] * right.vecs[2][1] + vecs[2][1] * right.vecs[2][2] + vecs[3][1] * right.vecs[2][3];
    m.vecs[2][2] = vecs[0][2] * right.vecs[2][0] + vecs[1][2] * right.vecs[2][1] + vecs[2][2] * right.vecs[2][2] + vecs[3][2] * right.vecs[2][3];
    m.vecs[2][3] = vecs[0][3] * right.vecs[2][0] + vecs[1][3] * right.vecs[2][1] + vecs[2][3] * right.vecs[2][2] + vecs[3][3] * right.vecs[2][3];

    m.vecs[3][0] = vecs[0][0] * right.vecs[3][0] + vecs[1][0] * right.vecs[3][1] + vecs[2][0] * right.vecs[3][2] + vecs[3][0] * right.vecs[3][3];
    m.vecs[3][1] = vecs[0][1] * right.vecs[3][0] + vecs[1][1] * right.vecs[3][1] + vecs[2][1] * right.vecs[3][2] + vecs[3][1] * right.vecs[3][3];
    m.vecs[3][2] = vecs[0][2] * right.vecs[3][0] + vecs[1][2] * right.vecs[3][1] + vecs[2][2] * right.vecs[3][2] + vecs[3][2] * right.vecs[3][3];
    m.vecs[3][3] = vecs[0][3] * right.vecs[3][0] + vecs[1][3] * right.vecs[3][1] + vecs[2][3] * right.vecs[3][2] + vecs[3][3] * right.vecs[3][3];

    return m;
}

TMPL_PREFIX Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4<T>& right)
{
    auto m = *this * right;
    std::memcpy(data, m.data, sizeof(data));
    return *this;
}

#undef TMPL_PREFIX

template class V_MATH_API Matrix4x4<float>;
template class V_MATH_API Matrix4x4<double>;

V_MATH_NS_END
