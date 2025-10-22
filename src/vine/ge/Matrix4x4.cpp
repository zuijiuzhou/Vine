#include <vine/ge/Matrix4x4.hpp>

#include <cmath>
#include <cstring>
#include <utility>

#include <vine/ge/Math.hpp>
#include <vine/ge/Point3.hpp>
#include <vine/ge/Vector3.hpp>

VI_GE_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Matrix4x4<T>::Matrix4x4()
  : vecs{
      { 1, 0, 0, 0 }, // COL0
      { 0, 1, 0, 0 }, // COL1
      { 0, 0, 1, 0 }, // COL2
      { 0, 0, 0, 1 }  // COL3
}
{}

TMPL_PREFIX Matrix4x4<T>::Matrix4x4(const Quaternion<T>& quat)
{
    // quta 必须归一化
    // R = | 1 - 2*(y*y + z*z)      2*(x*y - z*w)        2*(x*z + y*w)     |
    //     | 2*(x*y + z*w)          1 - 2*(x*x + z*z)    2*(y*z - x*w)     |
    //     | 2*(x*z - y*w)          2*(y*z + x*w)        1 - 2*(x*x + y*y) |
}

TMPL_PREFIX void Matrix4x4<T>::makeIdentity()
{
    // std::fill((T*)vecs, ((T*)vecs)+16, 0.0);
    memset(data, 0, sizeof(data));
    vecs[0][0] = vecs[1][1] = vecs[2][2] = vecs[3][3] = T(0);
}

TMPL_PREFIX void Matrix4x4<T>::makeRotation(const Vector3<T>& start, const Vector3<T>& end)
{}

TMPL_PREFIX void Matrix4x4<T>::makeRotation(const Vector3<T>& axis, T angle)
{
    // 参考罗德里格旋转公式
    // c=cos(angle)
    // s=sin(angle)
    // x=axis.x
    // y=axis.y
    // z=axis.z
    /*
    |xx(1-c)+c   xy(1-c)-zs   xz(1-c)+ys|
    |xy(1-c)+zs  yy(1-c)+c    yz(1-c)-xs|
    |xz(1-c)-ys  yz(1-c)+xs   zz(1-c)+c |
    列矩阵
    */
    if (angle == T(0) || axis.isZero())
        return;

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

TMPL_PREFIX void Matrix4x4<T>::makeRotation(const Point3<T>& center, const Vector3<T>& axis, T angle)
{}

TMPL_PREFIX void Matrix4x4<T>::makeTranslation(const Vector3<T>& offset)
{
    makeIdentity();
    vecs[3][0] = offset.x;
    vecs[3][1] = offset.y;
    vecs[3][2] = offset.z;
}

TMPL_PREFIX void Matrix4x4<T>::makeTranslation(T x, T y, T z)
{
    makeIdentity();
    vecs[3][0] = x;
    vecs[3][1] = y;
    vecs[3][2] = z;
}

TMPL_PREFIX void Matrix4x4<T>::makeScale(const Vector3<T>& vec)
{
    makeIdentity();
    vecs[0][0] = vec.x;
    vecs[1][1] = vec.y;
    vecs[2][2] = vec.z;
}

TMPL_PREFIX void Matrix4x4<T>::makeScale(T x, T y, T z)
{
    makeIdentity();
    vecs[0][0] = x;
    vecs[1][1] = y;
    vecs[2][2] = z;
}

TMPL_PREFIX void Matrix4x4<T>::makeScale(T factor)
{
    makeIdentity();
    vecs[0][0] = factor;
    vecs[1][1] = factor;
    vecs[2][2] = factor;
}

TMPL_PREFIX void Matrix4x4<T>::makeLookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up)
{
    auto f = eye - target;
    auto s = up.cross(f);
    auto u = f.cross(s);
    f.normalize();
    s.normalize();
    u.normalize();
    setCoordSystem(eye, s, u, f);
    invert();
}

TMPL_PREFIX void
Matrix4x4<T>::makeOrtho(double left, double right, double bottom, double top, double z_near, double z_far)
{
    auto tx = -(right + left) / (right - left);
    auto ty = -(top + bottom) / (top - bottom);
    auto tz = -(z_far + z_near) / (z_far - z_near);

    vec0 = column_type(T(2) / (right - left), T(0), T(0), T(0));
    vec1 = column_type(T(0), T(2) / (top - bottom), T(0), T(0));
    vec2 = column_type(T(0), T(0), T(-2) / (z_far - z_near), T(0));
    vec3 = column_type(tx, ty, tz, T(1));
}

TMPL_PREFIX void Matrix4x4<T>::makePerspective(double fovy, double aspect_ratio, double z_near, double z_far)
{}

TMPL_PREFIX void Matrix4x4<T>::setCoordSystem(const Point3<T>&  origin,
                                              const Vector3<T>& xAxis,
                                              const Vector3<T>& yAxis,
                                              const Vector3<T>& zAxis)
{
    // col0
    vecs[0].set(xAxis, T(0));
    // col1
    vecs[0].set(yAxis, T(0));
    // col2
    vecs[0].set(zAxis, T(0));
    // col3
    vecs[0].set(origin.x, origin.y, origin.z, T(1));
}

TMPL_PREFIX void
Matrix4x4<T>::getCoordSystem(Point3<T>& o_origin, Vector3<T>& o_xAxis, Vector3<T>& o_yAxis, Vector3<T>& o_zAxis) const
{
    // col0
    o_origin = vecs[3].asVector3().asPoint();
    o_xAxis  = vecs[0].asVector3();
    o_yAxis  = vecs[1].asVector3();
    o_zAxis  = vecs[2].asVector3();
}

TMPL_PREFIX void Matrix4x4<T>::transpose()
{
    std::swap(vecs[0][1], vecs[1][0]);
    std::swap(vecs[0][2], vecs[2][0]);
    std::swap(vecs[0][3], vecs[3][0]);

    std::swap(vecs[1][2], vecs[2][1]);
    std::swap(vecs[1][3], vecs[3][1]);

    std::swap(vecs[2][3], vecs[3][2]);
}

TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::transposed() const
{
    auto m = *this;
    m.transpose();
    return m;
}

TMPL_PREFIX void Matrix4x4<T>::invert()
{}

TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::inverted() const
{
    auto m = *this;
    m.invert();
    return m;
}

TMPL_PREFIX bool Matrix4x4<T>::isRigid() const
{
    if (!isAffine())
        return false;
    Point3<T>  o;
    Vector3<T> x, y, z;
    getCoordSystem(o, x, y, z);

    return true;
}

TMPL_PREFIX bool Matrix4x4<T>::isAffine() const
{
    return vecs[0][3] == T(0) && vecs[1][3] == T(0) && vecs[2][3] == T(0) && vecs[3][3] == T(1);
}

TMPL_PREFIX bool Matrix4x4<T>::isIdentity() const
{
    return vecs[0][0] == T(1) && vecs[0][1] == T(0) && vecs[0][2] == T(0) && vecs[0][3] == T(0) && // col1
           vecs[1][0] == T(0) && vecs[1][1] == T(1) && vecs[1][2] == T(0) && vecs[1][3] == T(0) && // col2
           vecs[2][0] == T(0) && vecs[2][1] == T(0) && vecs[2][2] == T(0) && vecs[2][3] == T(0) && // col3
           vecs[3][0] == T(0) && vecs[3][1] == T(0) && vecs[3][2] == T(0) && vecs[3][3] == T(1);   // col4
}

TMPL_PREFIX T Matrix4x4<T>::operator()(int row, int col) const
{
    return vecs[col][row];
}

TMPL_PREFIX T& Matrix4x4<T>::operator()(int row, int col)
{
    return vecs[col][row];
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

    m.vecs[0][0] = vecs[0][0] * right.vecs[0][0] + vecs[1][0] * right.vecs[0][1] + vecs[2][0] * right.vecs[0][2] +
                   vecs[3][0] * right.vecs[0][3];
    m.vecs[0][1] = vecs[0][1] * right.vecs[0][0] + vecs[1][1] * right.vecs[0][1] + vecs[2][1] * right.vecs[0][2] +
                   vecs[3][1] * right.vecs[0][3];
    m.vecs[0][2] = vecs[0][2] * right.vecs[0][0] + vecs[1][2] * right.vecs[0][1] + vecs[2][2] * right.vecs[0][2] +
                   vecs[3][2] * right.vecs[0][3];
    m.vecs[0][3] = vecs[0][3] * right.vecs[0][0] + vecs[1][3] * right.vecs[0][1] + vecs[2][3] * right.vecs[0][2] +
                   vecs[3][3] * right.vecs[0][3];

    m.vecs[1][0] = vecs[0][0] * right.vecs[1][0] + vecs[1][0] * right.vecs[1][1] + vecs[2][0] * right.vecs[1][2] +
                   vecs[3][0] * right.vecs[1][3];
    m.vecs[1][1] = vecs[0][1] * right.vecs[1][0] + vecs[1][1] * right.vecs[1][1] + vecs[2][1] * right.vecs[1][2] +
                   vecs[3][1] * right.vecs[1][3];
    m.vecs[1][2] = vecs[0][2] * right.vecs[1][0] + vecs[1][2] * right.vecs[1][1] + vecs[2][2] * right.vecs[1][2] +
                   vecs[3][2] * right.vecs[1][3];
    m.vecs[1][3] = vecs[0][3] * right.vecs[1][0] + vecs[1][3] * right.vecs[1][1] + vecs[2][3] * right.vecs[1][2] +
                   vecs[3][3] * right.vecs[1][3];

    m.vecs[2][0] = vecs[0][0] * right.vecs[2][0] + vecs[1][0] * right.vecs[2][1] + vecs[2][0] * right.vecs[2][2] +
                   vecs[3][0] * right.vecs[2][3];
    m.vecs[2][1] = vecs[0][1] * right.vecs[2][0] + vecs[1][1] * right.vecs[2][1] + vecs[2][1] * right.vecs[2][2] +
                   vecs[3][1] * right.vecs[2][3];
    m.vecs[2][2] = vecs[0][2] * right.vecs[2][0] + vecs[1][2] * right.vecs[2][1] + vecs[2][2] * right.vecs[2][2] +
                   vecs[3][2] * right.vecs[2][3];
    m.vecs[2][3] = vecs[0][3] * right.vecs[2][0] + vecs[1][3] * right.vecs[2][1] + vecs[2][3] * right.vecs[2][2] +
                   vecs[3][3] * right.vecs[2][3];

    m.vecs[3][0] = vecs[0][0] * right.vecs[3][0] + vecs[1][0] * right.vecs[3][1] + vecs[2][0] * right.vecs[3][2] +
                   vecs[3][0] * right.vecs[3][3];
    m.vecs[3][1] = vecs[0][1] * right.vecs[3][0] + vecs[1][1] * right.vecs[3][1] + vecs[2][1] * right.vecs[3][2] +
                   vecs[3][1] * right.vecs[3][3];
    m.vecs[3][2] = vecs[0][2] * right.vecs[3][0] + vecs[1][2] * right.vecs[3][1] + vecs[2][2] * right.vecs[3][2] +
                   vecs[3][2] * right.vecs[3][3];
    m.vecs[3][3] = vecs[0][3] * right.vecs[3][0] + vecs[1][3] * right.vecs[3][1] + vecs[2][3] * right.vecs[3][2] +
                   vecs[3][3] * right.vecs[3][3];

    return m;
}

TMPL_PREFIX Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4<T>& right)
{
    auto m = *this * right;
    std::memcpy(m.data, data, sizeof(data));
    return *this;
}

TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::rotate(const Point3<T>& center, const Vector3<T>& axis, T angle)
{
    Matrix4x4<T> m;
    m.makeRotation(center, axis, angle);
    return m;
}

TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::translate(const Vector3<T>& offset)
{
    Matrix4x4<T> m;
    m.makeTranslation(offset);
    return m;
}

TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::translate(T x, T y, T z)
{
    Matrix4x4<T> m;
    m.makeTranslation(x, y, z);
    return m;
}

TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::scale(const Vector3<T>& vec)
{
    Matrix4x4<T> m;
    m.makeScale(vec);
    return m;
}

TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::scale(T x, T y, T z)
{
    Matrix4x4<T> m;
    m.makeScale(x, y, z);
    return m;
}

TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::scale(T factor)
{
    Matrix4x4<T> m;
    m.makeScale(factor);
    return m;
}

TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::lookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up)
{
    Matrix4x4<T> m;
    m.makeLookAt(eye, target, up);
    return m;
}

TMPL_PREFIX Vector3<T> operator*(const Matrix4x4<T>& m, const Vector3<T>& v)
{
    return v;
}

TMPL_PREFIX Point3<T> operator*(const Matrix4x4<T>& m, const Point3<T>& p)
{
    return p;
}

#undef TMPL_PREFIX

template class VI_GE_API Matrix4x4<float>;
template class VI_GE_API Matrix4x4<double>;

VI_GE_NS_END