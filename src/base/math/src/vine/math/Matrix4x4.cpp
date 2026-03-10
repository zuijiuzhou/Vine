#include <vine/math/Matrix4x4.hpp>

#include <cmath>
#include <cstring>
#include <utility>

#include <vine/math/Math.hpp>
#include <vine/math/Point3.hpp>
#include <vine/math/Vector3.hpp>

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX void Matrix4x4<T>::makeRotation(const Vector3<T>& start, const Vector3<T>& end)
{
    // Build a shortest-arc rotation from start to end via quaternion.
    // Quaternion::makeRotate handles parallel and anti-parallel edge cases.
    Quaternion<T> quat;
    quat.makeRotate(start, end);
    makeRotation(quat);
}

TMPL_PREFIX void Matrix4x4<T>::makeRotation(const Vector3<T>& axis, T angle)
{
    // Rodrigues' rotation formula
    // c=cos(angle)
    // s=sin(angle)
    // x=axis.x
    // y=axis.y
    // z=axis.z
    /*
    |xx(1-c)+c   xy(1-c)-zs   xz(1-c)+ys|
    |xy(1-c)+zs  yy(1-c)+c    yz(1-c)-xs|
    |xz(1-c)-ys  yz(1-c)+xs   zz(1-c)+c |
    Column-major layout
    */
    if (angle == T(0) || math::isZero(axis.length2(), EPS<T>())) {
        makeIdentity();
        return;
    }

    auto axis_norm = axis;
    axis_norm.normalize();

    const auto c  = std::cos(angle);
    const auto ic = 1.0 - c;
    const auto s  = std::sin(angle);
    const auto x  = axis_norm.x;
    const auto y  = axis_norm.y;
    const auto z  = axis_norm.z;

    vecs[0][0] = x * x * ic + c;
    vecs[0][1] = x * y * ic + z * s;
    vecs[0][2] = x * z * ic - y * s;
    vecs[0][3] = T(0);

    vecs[1][0] = x * y * ic - z * s;
    vecs[1][1] = y * y * ic + c;
    vecs[1][2] = y * z * ic + x * s;
    vecs[1][3] = T(0);

    vecs[2][0] = x * z * ic + y * s;
    vecs[2][1] = y * z * ic - x * s;
    vecs[2][2] = z * z * ic + c;
    vecs[2][3] = T(0);

    vecs[3][0] = T(0);
    vecs[3][1] = T(0);
    vecs[3][2] = T(0);
    vecs[3][3] = T(1);
}

template <typename T>
void Matrix4x4<T>::makeRotation(const Quaternion<T>& quat)
{
    // Quaternion-to-matrix conversion.
    // q = (x, y, z, w) is expected to be a unit quaternion.
    // Convert to a 3x3 rotation block and extend to homogeneous 4x4.
    // R = | 1 - 2*(y*y + z*z)      2*(x*y - z*w)        2*(x*z + y*w)     |
    //     | 2*(x*y + z*w)          1 - 2*(x*x + z*z)    2*(y*z - x*w)     |
    //     | 2*(x*z - y*w)          2*(y*z + x*w)        1 - 2*(x*x + y*y) |
    // This form comes from quaternion multiplication and avoids trig calls.
    // Normalize defensively to avoid injecting scale when input is not unit-length.
    auto       q      = quat;
    const auto q_len2 = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    if (q_len2 == T(0)) {
        makeIdentity();
        return;
    }
    if (!math::isEqual(q_len2, T(1), T(1e-12))) {
        const auto inv_q_len = T(1) / std::sqrt(q_len2);
        q.x *= inv_q_len;
        q.y *= inv_q_len;
        q.z *= inv_q_len;
        q.w *= inv_q_len;
    }

    const auto x = q.x;
    const auto y = q.y;
    const auto z = q.z;
    const auto w = q.w;

    // Precompute reused terms.
    const auto xx = x * x; // x^2
    const auto yy = y * y; // y^2
    const auto zz = z * z; // z^2
    const auto xy = x * y; // xy
    const auto zw = z * w; // zw
    const auto xz = x * z; // xz
    const auto yw = y * w; // yw
    const auto yz = y * z; // yz
    const auto xw = x * w; // xw

    // Column 0
    vecs[0][0] = 1 - 2 * (yy + zz);
    vecs[0][1] = 2 * (xy + zw);
    vecs[0][2] = 2 * (xz - yw);
    vecs[0][3] = T(0);

    // Column 1
    vecs[1][0] = 2 * (xy - zw);
    vecs[1][1] = 1 - 2 * (xx + zz);
    vecs[1][2] = 2 * (yz + xw);
    vecs[1][3] = T(0);

    // Column 2
    vecs[2][0] = 2 * (xz + yw);
    vecs[2][1] = 2 * (yz - xw);
    vecs[2][2] = 1 - 2 * (xx + yy);
    vecs[2][3] = T(0);

    // Column 3
    vecs[3][0] = T(0);
    vecs[3][1] = T(0);
    vecs[3][2] = T(0);
    vecs[3][3] = T(1);
}

TMPL_PREFIX void Matrix4x4<T>::makeLookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up)
{
    // Build camera-to-world basis first, then invert to get the view matrix.
    // f is camera +Z axis in this convention (points from target to eye).
    // Final matrix maps world coordinates into camera space.
    auto f = eye - target;
    auto s = up.cross(f);
    auto u = f.cross(s);
    f.normalize();
    s.normalize();
    u.normalize();
    setBasis(eye, s, u, f);
    invert();
}

TMPL_PREFIX void Matrix4x4<T>::makeOrtho(double left, double right, double bottom, double top, double z_near, double z_far)
{
    // Orthographic projection keeps parallel lines parallel (no perspective divide).
    // It maps the axis-aligned view box [left,right]x[bottom,top]x[z_near,z_far]
    // into normalized device coordinates [-1,1]^3.
    auto tx = -(right + left) / (right - left);
    auto ty = -(top + bottom) / (top - bottom);
    auto tz = -(z_far + z_near) / (z_far - z_near);

    vec0 = Vector4<T>(T(2) / (right - left), T(0), T(0), T(0));
    vec1 = Vector4<T>(T(0), T(2) / (top - bottom), T(0), T(0));
    vec2 = Vector4<T>(T(0), T(0), T(-2) / (z_far - z_near), T(0));
    vec3 = Vector4<T>(tx, ty, tz, T(1));
}

TMPL_PREFIX void Matrix4x4<T>::makePerspective(double fovy, double aspect_ratio, double z_near, double z_far)
{
    // Build an OpenGL-style perspective projection matrix.
    // fovy: vertical field-of-view angle in radians.
    // aspect_ratio: viewport width/height.
    // z_near: near plane distance (must be > 0).
    // z_far: far plane distance (must be > z_near).
    //
    // Standard OpenGL projection form:
    // f = cot(fovy/2) = 1/tan(fovy/2)
    // | f/aspect  0      0                    0                        |
    // | 0         f      0                    0                        |
    // | 0         0      (f_far+n_near)/(n_near-f_far)  -1            |
    // | 0         0      2*f_far*n_near/(n_near-f_far)   0            |
    // In homogeneous clip space, w is used for perspective divide.

    const auto f = 1.0 / std::tan(fovy / 2.0); // cotangent of half field of view angle

    // Zero-initialize all matrix entries first.
    std::memset(data, 0, sizeof(data));

    // Fill projection entries.
    vecs[0][0] = T(f / aspect_ratio);                      // f/aspect
    vecs[1][1] = T(f);                                     // f
    vecs[2][2] = T((z_far + z_near) / (z_near - z_far));   // (near+far)/(near-far)
    vecs[2][3] = T(-1);                                    // -1 contributes to homogeneous w
    vecs[3][2] = T(2 * z_far * z_near / (z_near - z_far)); // 2*far*near/(near-far)
}

TMPL_PREFIX void Matrix4x4<T>::setBasis(const Point3<T>& origin, const Vector3<T>& x_axis, const Vector3<T>& y_axis, const Vector3<T>& z_axis)
{
    // Compose an affine transform from basis vectors and origin.
    // In column-major layout, each basis vector occupies one column.
    // col0
    vecs[0].set(x_axis, T(0));
    // col1
    vecs[1].set(y_axis, T(0));
    // col2
    vecs[2].set(z_axis, T(0));
    // col3
    vecs[3].set(origin.x, origin.y, origin.z, T(1));
}

TMPL_PREFIX void Matrix4x4<T>::getBasis(Point3<T>& o_origin, Vector3<T>& o_x_axis, Vector3<T>& o_y_axis, Vector3<T>& o_z_axis) const
{
    // Extract basis vectors and translation from affine matrix columns.
    o_origin = vecs[3].asVector3().asPoint();
    o_x_axis = vecs[0].asVector3();
    o_y_axis = vecs[1].asVector3();
    o_z_axis = vecs[2].asVector3();
}

TMPL_PREFIX void Matrix4x4<T>::transpose()
{
    // Transpose swaps row/column indices: M(r,c) <-> M(c,r).
    // For 4x4, only off-diagonal elements are swapped in-place.
    std::swap(vecs[0][1], vecs[1][0]);
    std::swap(vecs[0][2], vecs[2][0]);
    std::swap(vecs[0][3], vecs[3][0]);

    std::swap(vecs[1][2], vecs[2][1]);
    std::swap(vecs[1][3], vecs[3][1]);

    std::swap(vecs[2][3], vecs[3][2]);
}

TMPL_PREFIX void Matrix4x4<T>::invert()
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
        return;
    }

    // M^(-1) = (1/det) * adj(M)
    const T inv_det = T(1) / det;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            vecs[i][j] = adj.vecs[i][j] * inv_det;
        }
    }
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
    const auto len_x = x.length();
    const auto len_y = y.length();
    const auto len_z = z.length();

    if (!math::isEqual(len_x, T(1), eps) || !math::isEqual(len_y, T(1), eps) || !math::isEqual(len_z, T(1), eps)) {
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

TMPL_PREFIX bool Matrix4x4<T>::isAffine(T eps) const
{
    // Check if bottom row is [0 0 0 1] within tolerance.
    // Use epsilon to tolerate accumulated floating-point errors.
    return math::isZero(vecs[0][3], eps) && math::isZero(vecs[1][3], eps) && math::isZero(vecs[2][3], eps) && math::isEqual(vecs[3][3], T(1), eps);
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

// TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::fromEulerXYZ(const Vector3<T>& xyz)
// {
//     /* - In three-dimensional rotations, a specific intrinsic rotation and a specific extrinsic rotation are equivalent.
//      * - For example, an extrinsic ZYX rotation (rotation about the fixed Z-axis, then the fixed Y-axis, and finally the
//      * fixed X-axis)
//      * - is mathematically equivalent to an intrinsic XYZ rotation (rotation about the current X-axis, then the current
//      * Y-axis, and finally the current Z-axis).
//      * - Both result in the same final orientation, but the reference frames of the rotation axes are different.
//      * */
// }

// TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::fromEulerZYX(const Vector3<T>& xyz)
// {}

TMPL_PREFIX Vector3<T> operator*(const Matrix4x4<T>& m, const Vector3<T>& v)
{
    // Transform a direction vector with homogeneous w=0.
    // Translation does not affect vectors.
    // Result uses the upper-left 3x3 linear block.
    //
    // [x', y', z'] = [M00*vx + M10*vy + M20*vz,
    //                 M01*vx + M11*vy + M21*vz,
    //                 M02*vx + M12*vy + M22*vz]
    // Matrix storage is column-major: vecs[col][row].

    return Vector3<T>(m.vecs[0][0] * v.x + m.vecs[1][0] * v.y + m.vecs[2][0] * v.z,
                      m.vecs[0][1] * v.x + m.vecs[1][1] * v.y + m.vecs[2][1] * v.z,
                      m.vecs[0][2] * v.x + m.vecs[1][2] * v.y + m.vecs[2][2] * v.z);
}

TMPL_PREFIX Point3<T> operator*(const Matrix4x4<T>& m, const Point3<T>& p)
{
    // Transform a point with homogeneous w=1.
    // Translation, rotation, and scale all apply.
    // Compute homogeneous coordinates and then perspective divide if needed.
    //
    // [x', y', z', w'] = M * [px, py, pz, 1]^T
    // w' = M30*px + M31*py + M32*pz + M33
    // If w' != 1, apply homogeneous divide: [x'/w', y'/w', z'/w'].
    // Matrix storage is column-major: vecs[col][row].

    const auto x = m.vecs[0][0] * p.x + m.vecs[1][0] * p.y + m.vecs[2][0] * p.z + m.vecs[3][0];
    const auto y = m.vecs[0][1] * p.x + m.vecs[1][1] * p.y + m.vecs[2][1] * p.z + m.vecs[3][1];
    const auto z = m.vecs[0][2] * p.x + m.vecs[1][2] * p.y + m.vecs[2][2] * p.z + m.vecs[3][2];
    const auto w = m.vecs[0][3] * p.x + m.vecs[1][3] * p.y + m.vecs[2][3] * p.z + m.vecs[3][3];

    // Fast path for affine transform (w == 1), otherwise perspective divide.
    // If w is near zero, skip divide to avoid generating infinities/NaNs.
    if (math::isEqual(w, T(1), T(1e-12))) {
        return Point3<T>(x, y, z);
    }
    if (math::isZero(w, T(1e-12))) {
        return Point3<T>(x, y, z);
    }
    else {
        return Point3<T>(x / w, y / w, z / w);
    }
}

#undef TMPL_PREFIX

template class VI_MATH_API Matrix4x4<float>;
template class VI_MATH_API Matrix4x4<double>;
template VI_MATH_API Vector3<float> operator*(const Matrix4x4<float>& m, const Vector3<float>& v);
template VI_MATH_API Vector3<double> operator*(const Matrix4x4<double>& m, const Vector3<double>& v);
template VI_MATH_API Point3<float> operator*(const Matrix4x4<float>& m, const Point3<float>& p);
template VI_MATH_API Point3<double> operator*(const Matrix4x4<double>& m, const Point3<double>& p);

VI_MATH_NS_END
