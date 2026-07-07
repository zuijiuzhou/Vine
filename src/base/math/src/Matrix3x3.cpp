#include <vine/math/Matrix3x3.hpp>

#include <cmath>
#include <cstring>
#include <utility>

#include <vine/math/Math.hpp>

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
// Rotation
// ---------------------------------------------------------------------------

TMPL_PREFIX void Matrix3x3<T>::makeRotation(T angle)
{
    // 2D rotation matrix (CCW), column-major.
    // R(θ) = | cosθ -sinθ 0 |
    //        | sinθ  cosθ 0 |
    //        |  0     0   1 |
    const auto c = std::cos(angle);
    const auto s = std::sin(angle);

    vecs[0][0] = c;
    vecs[0][1] = s;
    vecs[0][2] = T(0);

    vecs[1][0] = -s;
    vecs[1][1] = c;
    vecs[1][2] = T(0);

    vecs[2][0] = T(0);
    vecs[2][1] = T(0);
    vecs[2][2] = T(1);
}

// ---------------------------------------------------------------------------
// Translation
// ---------------------------------------------------------------------------

TMPL_PREFIX void Matrix3x3<T>::makeTranslation(const Vector2<T>& offset) noexcept
{
    makeIdentity();
    vecs[2][0] = offset.x;
    vecs[2][1] = offset.y;
}

TMPL_PREFIX void Matrix3x3<T>::makeTranslation(T x, T y) noexcept
{
    makeIdentity();
    vecs[2][0] = x;
    vecs[2][1] = y;
}

// ---------------------------------------------------------------------------
// Scale
// ---------------------------------------------------------------------------

TMPL_PREFIX void Matrix3x3<T>::makeScale(const Vector2<T>& vec) noexcept
{
    makeIdentity();
    vecs[0][0] = vec.x;
    vecs[1][1] = vec.y;
}

TMPL_PREFIX void Matrix3x3<T>::makeScale(T x, T y) noexcept
{
    makeIdentity();
    vecs[0][0] = x;
    vecs[1][1] = y;
}

TMPL_PREFIX void Matrix3x3<T>::makeScale(T factor) noexcept
{
    makeIdentity();
    vecs[0][0] = factor;
    vecs[1][1] = factor;
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
// Pre/post rotate
// ---------------------------------------------------------------------------

TMPL_PREFIX Matrix3x3<T>& Matrix3x3<T>::preRotate(T angle)
{
    // M := R(angle) * M
    Matrix3x3<T> rot;
    rot.makeRotation(angle);
    return preMulti(rot);
}

TMPL_PREFIX Matrix3x3<T>& Matrix3x3<T>::postRotate(T angle)
{
    // M := M * R(angle)
    Matrix3x3<T> rot;
    rot.makeRotation(angle);
    return postMulti(rot);
}

// ---------------------------------------------------------------------------
// Pre/post translate
// ---------------------------------------------------------------------------

TMPL_PREFIX Matrix3x3<T>& Matrix3x3<T>::preTranslate(const Vector2<T>& offset)
{
    // Prepend translation: M := T * M.
    // T * M adds offset * row_w to rows 0..1.
    const auto tx = offset.x;
    const auto ty = offset.y;
    for (size_t col = 0; col < 3; ++col) {
        const auto w = vecs[col][2];
        vecs[col][0] += tx * w;
        vecs[col][1] += ty * w;
    }
    return *this;
}

TMPL_PREFIX Matrix3x3<T>& Matrix3x3<T>::postTranslate(const Vector2<T>& offset)
{
    // Append translation: M := M * T.
    // Only column 2 changes: col2 += tx*col0 + ty*col1.
    const auto tx = offset.x;
    const auto ty = offset.y;
    for (size_t row = 0; row < 3; ++row) {
        vecs[2][row] += vecs[0][row] * tx + vecs[1][row] * ty;
    }
    return *this;
}

// ---------------------------------------------------------------------------
// Pre/post scale
// ---------------------------------------------------------------------------

TMPL_PREFIX Matrix3x3<T>& Matrix3x3<T>::preScale(const Vector2<T>& factor)
{
    // Prepend scale: M := S * M. Scale rows 0..1.
    const auto sx = factor.x;
    const auto sy = factor.y;
    for (size_t col = 0; col < 3; ++col) {
        vecs[col][0] *= sx;
        vecs[col][1] *= sy;
    }
    return *this;
}

TMPL_PREFIX Matrix3x3<T>& Matrix3x3<T>::postScale(const Vector2<T>& factor)
{
    // Append scale: M := M * S. Scale columns 0..1.
    const auto sx = factor.x;
    const auto sy = factor.y;
    for (size_t row = 0; row < 3; ++row) {
        vecs[0][row] *= sx;
        vecs[1][row] *= sy;
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
// Component extraction
// ---------------------------------------------------------------------------

TMPL_PREFIX T Matrix3x3<T>::rotation() const
{
    // From upper-left 2x2: θ = atan2(sinθ, cosθ) = atan2(m10, m00)
    // Adjust for column-major: m10 = vecs[0][1], m00 = vecs[0][0]
    return std::atan2(vecs[0][1], vecs[0][0]);
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
// Static factories
// ---------------------------------------------------------------------------

TMPL_PREFIX Matrix3x3<T> Matrix3x3<T>::rotate(T angle)
{
    Matrix3x3<T> m;
    m.makeRotation(angle);
    return m;
}

TMPL_PREFIX Matrix3x3<T> Matrix3x3<T>::translate(const Vector2<T>& offset)
{
    Matrix3x3<T> m;
    m.makeTranslation(offset);
    return m;
}

TMPL_PREFIX Matrix3x3<T> Matrix3x3<T>::translate(T x, T y)
{
    Matrix3x3<T> m;
    m.makeTranslation(x, y);
    return m;
}

TMPL_PREFIX Matrix3x3<T> Matrix3x3<T>::scale(const Vector2<T>& vec)
{
    Matrix3x3<T> m;
    m.makeScale(vec);
    return m;
}

TMPL_PREFIX Matrix3x3<T> Matrix3x3<T>::scale(T x, T y)
{
    Matrix3x3<T> m;
    m.makeScale(x, y);
    return m;
}

TMPL_PREFIX Matrix3x3<T> Matrix3x3<T>::scale(T factor)
{
    Matrix3x3<T> m;
    m.makeScale(factor);
    return m;
}

// ---------------------------------------------------------------------------
// Point / vector transformation (global operators)
// ---------------------------------------------------------------------------

TMPL_PREFIX Vector2<T> operator*(const Matrix3x3<T>& m, const Vector2<T>& v)
{
    // v' = M·v  (w=0, translation ignored)
    return Vector2<T>(m.vecs[0][0] * v.x + m.vecs[1][0] * v.y, m.vecs[0][1] * v.x + m.vecs[1][1] * v.y);
}

TMPL_PREFIX Point2<T> operator*(const Matrix3x3<T>& m, const Point2<T>& p)
{
    // p' = M·p  (w=1, full affine transform, with homogeneous divide)
    const auto x = m.vecs[0][0] * p.x + m.vecs[1][0] * p.y + m.vecs[2][0];
    const auto y = m.vecs[0][1] * p.x + m.vecs[1][1] * p.y + m.vecs[2][1];
    const auto w = m.vecs[0][2] * p.x + m.vecs[1][2] * p.y + m.vecs[2][2];

    if (math::isEqual(w, T(1), T(1e-12))) {
        return Point2<T>(x, y);
    }
    if (math::isZero(w, T(1e-12))) {
        return Point2<T>(x, y);
    }
    return Point2<T>(x / w, y / w);
}

// ---------------------------------------------------------------------------
// Explicit instantiations
// ---------------------------------------------------------------------------

#undef TMPL_PREFIX

template class V_MATH_API Matrix3x3<float>;
template class V_MATH_API Matrix3x3<double>;
template V_MATH_API Vector2<float> operator*(const Matrix3x3<float>&, const Vector2<float>&);
template V_MATH_API Vector2<double> operator*(const Matrix3x3<double>&, const Vector2<double>&);
template V_MATH_API Point2<float> operator*(const Matrix3x3<float>&, const Point2<float>&);
template V_MATH_API Point2<double> operator*(const Matrix3x3<double>&, const Point2<double>&);

V_MATH_NS_END