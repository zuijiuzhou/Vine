#include <vine/ge/Matrix4x4.h>

#include <cmath>
#include <memory>

#include <vine/ge/Math.h>
#include <vine/ge/Point3.h>
#include <vine/ge/Vector3.h>

VI_GE_NS_BEGIN

#define TMPL_PREFIX template <typename T>
TMPL_PREFIX Matrix4x4<T>::Matrix4x4()
  : cols{
      { 1, 0, 0, 0 }, // COL0
      { 0, 1, 0, 0 }, // COL1
      { 0, 0, 1, 0 }, // COL2
      { 0, 0, 0, 1 }  // COL3
} {
}
TMPL_PREFIX void Matrix4x4<T>::makeIdentity() {
    // std::fill((T*)cols, ((T*)cols)+16, 0.0);
    memset(&cols, 0, sizeof(cols));
    cols[0][0] = cols[1][1] = cols[2][2] = cols[3][3] = 1.0;
}
TMPL_PREFIX void Matrix4x4<T>::makeRotation(const Vector3<T>& start, const Vector3<T>& end) {
}
TMPL_PREFIX void Matrix4x4<T>::makeRotation(const Vector3<T>& axis, T angle) {
    // 参考罗德里格旋转公式
    // c=cos(angle)
    // s=sin(angle)
    // x=axis.x
    // y=axis.y
    // z=axis.z
    /*
    |xx(1-c)+c   xy(1-c)-zs   xz(1-c)+ys |
    |xy(1-c)+zs  yy(1-c)+c    yz(1-c)-xs |
    |xz(1-c)-ys  yz(1-c)+xs   zz(1-c) + c|
    列矩阵
    */
    makeIdentity();
    if (angle == 0.0) return;

    auto c  = std::cos(angle);
    auto ic = 1.0 - c;
    auto s  = std::sin(angle);
    auto x  = axis.x;
    auto y  = axis.y;
    auto z  = axis.z;

    cols[0][0] = x * x * ic + c;
    cols[0][1] = x * y * ic + z * s;
    cols[0][2] = x * z * ic - y * s;

    cols[1][0] = x * y * ic - z * s;
    cols[1][1] = y * y * ic + c;
    cols[1][2] = y * z * ic + x * s;

    cols[2][0] = x * z * ic + y * s;
    cols[2][1] = y * z * ic - x * s;
    cols[2][2] = z * z * ic + c;
}
TMPL_PREFIX void Matrix4x4<T>::makeRotation(const Point3<T>& center, const Vector3<T>& axis, T angle) {
}
TMPL_PREFIX void Matrix4x4<T>::makeTranslation(const Vector3<T>& offset) {
    makeIdentity();
    cols[3][0] = offset.x;
    cols[3][1] = offset.y;
    cols[3][2] = offset.z;
}
TMPL_PREFIX void Matrix4x4<T>::makeTranslation(T x, T y, T z) {
    makeIdentity();
    cols[3][0] = x;
    cols[3][1] = y;
    cols[3][2] = z;
}
TMPL_PREFIX void Matrix4x4<T>::makeScale(const Vector3<T>& vec) {
    makeIdentity();
    cols[0][0] = vec.x;
    cols[1][1] = vec.y;
    cols[2][2] = vec.z;
}
TMPL_PREFIX void Matrix4x4<T>::makeScale(T x, T y, T z) {
    makeIdentity();
    cols[0][0] = x;
    cols[1][1] = y;
    cols[2][2] = z;
}
TMPL_PREFIX void Matrix4x4<T>::makeScale(T factor) {
    makeIdentity();
    cols[0][0] = factor;
    cols[1][1] = factor;
    cols[2][2] = factor;
}
TMPL_PREFIX void Matrix4x4<T>::makeLookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up) {
    auto f = eye - target;
    auto s = cross(up, f);
    auto u = cross(f, s);
    normalize(f);
    normalize(s);
    normalize(u);
    setCoordSystem(eye, s, u, f);
    invert();
}
TMPL_PREFIX void Matrix4x4<T>::setCoordSystem(const Point3<T>&  origin,
                                              const Vector3<T>& xAxis,
                                              const Vector3<T>& yAxis,
                                              const Vector3<T>& zAxis) {
    // col0
    cols[0][0] = xAxis.x;
    cols[0][1] = xAxis.y;
    cols[0][2] = xAxis.z;
    cols[0][3] = 0;
    // col1
    cols[1][0] = yAxis.x;
    cols[1][1] = yAxis.y;
    cols[1][2] = yAxis.z;
    cols[1][3] = 0;
    // col2
    cols[2][0] = zAxis.x;
    cols[2][1] = zAxis.y;
    cols[2][2] = zAxis.z;
    cols[2][3] = 0;
    // col3
    cols[3][0] = origin.x;
    cols[3][1] = origin.y;
    cols[3][2] = origin.z;
    cols[3][3] = 0;
}
TMPL_PREFIX void
Matrix4x4<T>::getCoordSystem(Point3<T>& o_origin, Vector3<T>& o_xAxis, Vector3<T>& o_yAxis, Vector3<T>& o_zAxis) const {
    //// col0
    // o_xAxis.x  = cols[0].x;
    // o_xAxis.y  = cols[0][1];
    // o_xAxis.z  = cols[0][2];
    //// col1
    // o_yAxis.x  = cols[1][0];
    // o_yAxis.y  = cols[1][1];
    // o_yAxis.z  = cols[1][2];
    //// col2
    // o_zAxis.x  = cols[2][0];
    // o_zAxis.y  = cols[2][1];
    // o_zAxis.z  = cols[2][2];
    //// col3
    // o_origin.x = cols[3][0];
    // o_origin.y = cols[3][1];
    // o_origin.z = cols[3][2];
}
TMPL_PREFIX void Matrix4x4<T>::transpose() {
    std::swap(cols[0][1], cols[1][0]);
    std::swap(cols[0][2], cols[2][0]);
    std::swap(cols[0][3], cols[3][0]);

    std::swap(cols[1][2], cols[2][1]);
    std::swap(cols[1][3], cols[3][1]);

    std::swap(cols[2][3], cols[3][2]);
}
TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::transposed() const {
    auto m = *this;
    m.transpose();
    return m;
}
TMPL_PREFIX void Matrix4x4<T>::invert() {
}
TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::inverted() const {
    auto m = *this;
    m.invert();
    return m;
}
TMPL_PREFIX bool Matrix4x4<T>::isRigid() const {
    if (!isAffine()) return false;
    Point3<T>  o;
    Vector3<T> x, y, z;
    getCoordSystem(o, x, y, z);

    return true;
}
TMPL_PREFIX bool Matrix4x4<T>::isAffine() const {
    return cols[0][3] == T(0) && cols[1][3] == T(0) && cols[2][3] == T(0) && cols[3][3] == T(1);
}
TMPL_PREFIX bool Matrix4x4<T>::isIdentity() const {
    return cols[0][0] == 1 && cols[0][1] == 0 && cols[0][2] == 0 && cols[0][3] == 0     // col0
           && cols[1][0] == 0 && cols[1][1] == 1 && cols[1][2] == 0 && cols[1][3] == 0  // col1
           && cols[2][0] == 0 && cols[2][1] == 0 && cols[2][2] == 0 && cols[2][3] == 0  // col2
           && cols[3][0] == 0 && cols[3][1] == 0 && cols[3][2] == 0 && cols[3][3] == 1; // col3
}
TMPL_PREFIX T Matrix4x4<T>::operator()(int row, int col) const {
    return cols[row][col];
}
TMPL_PREFIX T& Matrix4x4<T>::operator()(int row, int col) {
    return cols[row][col];
}
TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& right) const {
    Matrix4x4<T> m;

    m.cols[0][0] = cols[0][0] * right.cols[0][0] + cols[0][1] * right.cols[1][0] + cols[0][2] * cols[2][0] +
                   cols[0][3] * right.cols[3][0];
    m.cols[0][1] = cols[0][0] * right.cols[0][1] + cols[0][1] * right.cols[1][1] + cols[0][2] * cols[2][1] +
                   cols[0][3] * right.cols[3][1];
    return m;
}
TMPL_PREFIX Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4<T>& right) {
    Matrix4x4<T> m;
    return const_cast<Matrix4x4<T>&>(*this);
}
TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::rotate(const Point3<T>& center, const Vector3<T>& axis, T angle) {
    Matrix4x4<T> m;
    m.makeRotation(center, axis, angle);
    return m;
}
TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::translate(const Vector3<T>& offset) {
    Matrix4x4<T> m;
    m.makeTranslation(offset);
    return m;
}
TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::translate(T x, T y, T z) {
    Matrix4x4<T> m;
    m.makeTranslation(x, y, z);
    return m;
}
TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::scale(const Vector3<T>& vec) {
    Matrix4x4<T> m;
    m.makeScale(vec);
    return m;
}
TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::scale(T x, T y, T z) {
    Matrix4x4<T> m;
    m.makeScale(x, y, z);
    return m;
}
TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::scale(T factor) {
    Matrix4x4<T> m;
    m.makeScale(factor);
    return m;
}
TMPL_PREFIX Matrix4x4<T> Matrix4x4<T>::lookAt(const Point3<T>& eye, const Point3<T>& target, const Vector3<T>& up) {
    Matrix4x4<T> m;
    m.makeLookAt(eye, target, up);
    return m;
}
TMPL_PREFIX Vector3<T> operator*(const Matrix4x4<T>& m, const Vector3<T>& v) {
    return v;
}
TMPL_PREFIX Point3<T> operator*(const Matrix4x4<T>& m, const Point3<T>& p) {
    return p;
}
#undef TMPL_PREFIX

template class VI_GE_API Matrix4x4<float>;
template class VI_GE_API Matrix4x4<double>;

VI_GE_NS_END