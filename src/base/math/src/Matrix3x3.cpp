#include <vine/math/Matrix3x3.hpp>

#include <cmath>
#include <cstring>

#include <vine/math/Math.hpp>
#include <vine/math/Vector2.hpp>

V_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

namespace
{

/// Compose two 3x3 column-major matrices: C = A * B.
template <typename T>
Matrix3x3<T> compose3x3(const Matrix3x3<T>& a, const Matrix3x3<T>& b)
{
    Matrix3x3<T> c;
    // c.vecs[j] = b.vecs[j].x * a.vecs[0] + b.vecs[j].y * a.vecs[1] + b.vecs[j].z * a.vecs[2]
    c.vecs[0] = a.vecs[0] * b.vecs[0].x + a.vecs[1] * b.vecs[0].y + a.vecs[2] * b.vecs[0].z;
    c.vecs[1] = a.vecs[0] * b.vecs[1].x + a.vecs[1] * b.vecs[1].y + a.vecs[2] * b.vecs[1].z;
    c.vecs[2] = a.vecs[0] * b.vecs[2].x + a.vecs[1] * b.vecs[2].y + a.vecs[2] * b.vecs[2].z;
    return c;
}

} // namespace

// ---------------------------------------------------------------------------
// Identity
// ---------------------------------------------------------------------------

TMPL_PREFIX void Matrix3x3<T>::makeIdentity() noexcept
{
    std::memset(data, 0, sizeof(data));
    vecs[0][0] = vecs[1][1] = vecs[2][2] = T(1);
}

// ---------------------------------------------------------------------------
// Pre/post multiply
// ---------------------------------------------------------------------------

TMPL_PREFIX Matrix3x3<T>& Matrix3x3<T>::preMulti(const Matrix3x3<T>& left)
{
    // M := left * M
    T old[3][3];
    std::memcpy(old, vecs, sizeof(old));

    for (size_t col = 0; col < 3; ++col) {
        for (size_t row = 0; row < 3; ++row) {
            T v = T(0);
            for (size_t k = 0; k < 3; ++k) {
                v += left.vecs[k][row] * old[col][k];
            }
            vecs[col][row] = v;
        }
    }
    return *this;
}

TMPL_PREFIX Matrix3x3<T>& Matrix3x3<T>::postMulti(const Matrix3x3<T>& right)
{
    // M := M * right
    T old[3][3];
    std::memcpy(old, vecs, sizeof(old));

    for (size_t col = 0; col < 3; ++col) {
        for (size_t row = 0; row < 3; ++row) {
            T v = T(0);
            for (size_t k = 0; k < 3; ++k) {
                v += old[k][row] * right.vecs[col][k];
            }
            vecs[col][row] = v;
        }
    }
    return *this;
}

// ---------------------------------------------------------------------------
// Determinant
// ---------------------------------------------------------------------------

TMPL_PREFIX T Matrix3x3<T>::determinant() const
{
    // det = a(ei-fh) - b(di-fg) + c(dh-eg)
    const auto a = vecs[0][0], b = vecs[1][0], c = vecs[2][0];
    const auto d = vecs[0][1], e = vecs[1][1], f = vecs[2][1];
    const auto g = vecs[0][2], h = vecs[1][2], i = vecs[2][2];

    return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
}

// ---------------------------------------------------------------------------
// Inversion
// ---------------------------------------------------------------------------

TMPL_PREFIX void Matrix3x3<T>::invert()
{
    const auto a = vecs[0][0], b = vecs[1][0], c = vecs[2][0];
    const auto d = vecs[0][1], e = vecs[1][1], f = vecs[2][1];
    const auto g = vecs[0][2], h = vecs[1][2], i = vecs[2][2];

    const T det = a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
    if (math::isZero(det, EPS<T>())) {
        return;
    }

    T adj[3][3];
    adj[0][0] = (e * i - f * h);
    adj[0][1] = (c * h - b * i);
    adj[0][2] = (b * f - c * e);
    adj[1][0] = (f * g - d * i);
    adj[1][1] = (a * i - c * g);
    adj[1][2] = (c * d - a * f);
    adj[2][0] = (d * h - e * g);
    adj[2][1] = (b * g - a * h);
    adj[2][2] = (a * e - b * d);

    const T inv_det = T(1) / det;
    for (int col = 0; col < 3; ++col) {
        for (int row = 0; row < 3; ++row) {
            vecs[col][row] = adj[col][row] * inv_det;
        }
    }
}

// ---------------------------------------------------------------------------
// Queries
// ---------------------------------------------------------------------------

TMPL_PREFIX bool Matrix3x3<T>::isRigid(T eps) const
{
    if (!isAffine(eps)) {
        return false;
    }

    const Vector2<T> x(vecs[0][0], vecs[0][1]);
    const Vector2<T> y(vecs[1][0], vecs[1][1]);

    const auto len2_x  = x.length2();
    const auto len2_y  = y.length2();
    const auto eps_len = T(2) * eps + eps * eps;

    if (!math::isEqual(len2_x, T(1), eps_len) || !math::isEqual(len2_y, T(1), eps_len)) {
        return false;
    }
    if (!math::isZero(x.dot(y), eps)) {
        return false;
    }

    // Right-handed: det of upper-left 2x2 > 0 → x.x*y.y - y.x*x.y > 0
    const auto det2 = vecs[0][0] * vecs[1][1] - vecs[1][0] * vecs[0][1];
    if (det2 < T(0)) {
        return false;
    }

    return true;
}

TMPL_PREFIX bool Matrix3x3<T>::isEqual(const Matrix3x3<T>& other, T eps) const
{
    for (int col = 0; col < 3; ++col) {
        for (int row = 0; row < 3; ++row) {
            if (!math::isEqual(vecs[col][row], other.vecs[col][row], eps)) {
                return false;
            }
        }
    }
    return true;
}

TMPL_PREFIX bool Matrix3x3<T>::isZero(T eps) const
{
    for (int col = 0; col < 3; ++col) {
        for (int row = 0; row < 3; ++row) {
            if (!math::isZero(vecs[col][row], eps)) {
                return false;
            }
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// Matrix multiplication
// ---------------------------------------------------------------------------

TMPL_PREFIX Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3<T>& right) const
{
    return compose3x3(*this, right);
}

TMPL_PREFIX Matrix3x3<T>& Matrix3x3<T>::operator*=(const Matrix3x3<T>& right)
{
    *this = compose3x3(*this, right);
    return *this;
}

// ---------------------------------------------------------------------------
// Explicit instantiations
// ---------------------------------------------------------------------------

#undef TMPL_PREFIX

template class V_MATH_API Matrix3x3<float>;
template class V_MATH_API Matrix3x3<double>;

V_MATH_NS_END