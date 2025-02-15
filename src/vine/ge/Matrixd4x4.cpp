#include <vine/ge/Matrixd4x4.h>

#include <cmath>
#include <memory>

#include <vine/ge/Point3d.h>
#include <vine/ge/Point3f.h>
#include <vine/ge/Vector3d.h>
#include <vine/ge/Vector3f.h>

VI_GE_NS_BEGIN

Matrixd4x4::Matrixd4x4()
  : data{
      1, 0, 0, 0, // row0
      0, 1, 0, 0, // row1
      0, 0, 1, 0, // row2
      0, 0, 0, 1  // row3
  } {
}
void Matrixd4x4::makeIdentity() {
    // std::fill((double*)data, ((double*)data)+16, 0.0);
    memset(data, 0, sizeof(data));
    data[0][0] = data[1][1] = data[2][2] = data[3][3] = 1.0;
}
void Matrixd4x4::makeRotation(const Vector3d& start, const Vector3d& end) {
}
void Matrixd4x4::makeRotation(const Vector3d& axis, double angle) {
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

    data[0][0] = x * x * ic + c;
    data[0][1] = x * y * ic + z * s;
    data[0][2] = x * z * ic - y * s;

    data[1][0] = x * y * ic - z * s;
    data[1][1] = y * y * ic + c;
    data[1][2] = y * z * ic + x * s;

    data[2][0] = x * z * ic + y * s;
    data[2][1] = y * z * ic - x * s;
    data[2][2] = z * z * ic + c;
}
void Matrixd4x4::makeRotation(const Point3d& center, const Vector3d& axis, double angle) {
}
void Matrixd4x4::makeTranslation(const Vector3d& offset) {
    makeIdentity();
    data[3][0] = offset.x;
    data[3][1] = offset.y;
    data[3][2] = offset.z;
}
void Matrixd4x4::makeTranslation(double x, double y, double z) {
    makeIdentity();
    data[3][0] = x;
    data[3][1] = y;
    data[3][2] = z;
}
void Matrixd4x4::makeScale(const Vector3d& vec) {
    makeIdentity();
    data[0][0] = vec.x;
    data[1][1] = vec.y;
    data[2][2] = vec.z;
}
void Matrixd4x4::makeScale(double x, double y, double z) {
    makeIdentity();
    data[0][0] = x;
    data[1][1] = y;
    data[2][2] = z;
}
void Matrixd4x4::makeScale(double factor) {
    makeIdentity();
    data[0][0] = factor;
    data[1][1] = factor;
    data[2][2] = factor;
}
void Matrixd4x4::makeLookAt(const Point3d& eye, const Point3d& target, const Vector3d& up) {
    auto f = eye - target;
    auto s = up ^ f;
    auto u = f ^ s;
    f.normalize();
    s.normalize();
    u.normalize();
    setCoordSystem(eye, s, u, f);
    invert();
}
void Matrixd4x4::setCoordSystem(const Point3d&  origin,
                                const Vector3d& xAxis,
                                const Vector3d& yAxis,
                                const Vector3d& zAxis) {
    // col0
    data[0][0] = xAxis.x;
    data[0][1] = xAxis.y;
    data[0][2] = xAxis.z;
    data[0][3] = 0;
    // col1
    data[1][0] = yAxis.x;
    data[1][1] = yAxis.y;
    data[1][2] = yAxis.z;
    data[1][3] = 0;
    // col2
    data[2][0] = zAxis.x;
    data[2][1] = zAxis.y;
    data[2][2] = zAxis.z;
    data[2][3] = 0;
    // col3
    data[3][0] = origin.x;
    data[3][1] = origin.y;
    data[3][2] = origin.z;
    data[3][3] = 0;
}
void Matrixd4x4::getCoordSystem(Point3d& o_origin, Vector3d& o_xAxis, Vector3d& o_yAxis, Vector3d& o_zAxis) const {
    // row0
    o_xAxis.x  = data[0][0];
    o_xAxis.y  = data[0][1];
    o_xAxis.z  = data[0][2];
    // row1
    o_yAxis.x  = data[1][0];
    o_yAxis.y  = data[1][1];
    o_yAxis.z  = data[1][2];
    // row2
    o_zAxis.x  = data[2][0];
    o_zAxis.y  = data[2][1];
    o_zAxis.z  = data[2][2];
    // row3
    o_origin.x = data[3][0];
    o_origin.y = data[3][1];
    o_origin.z = data[3][2];
}
void Matrixd4x4::transpose() {
    std::swap(data[0][1], data[1][0]);
    std::swap(data[0][2], data[2][0]);
    std::swap(data[0][3], data[3][0]);

    std::swap(data[1][2], data[2][1]);
    std::swap(data[1][3], data[3][1]);

    std::swap(data[2][3], data[3][2]);
}
Matrixd4x4 Matrixd4x4::transposed() const {
    auto m = *this;
    m.transpose();
    return m;
}
void Matrixd4x4::invert() {
}
Matrixd4x4 Matrixd4x4::inverted() const {
    auto m = *this;
    m.invert();
    return m;
}
bool Matrixd4x4::isRigid() const {
    if (!isAffine()) return false;
    Point3d  o;
    Vector3d x, y, z;
    getCoordSystem(o, x, y, z);

    return x.isNormalized()          //
           && y.isNormalized()       //
           && z.isNormalized()       //
           && x.isPerpendicularTo(y) //
           && y.isPerpendicularTo(z) //
           && z.isPerpendicularTo(x);
}
bool Matrixd4x4::isAffine() const {
    return data[0][3] == 0 && data[1][3] == 0 && data[2][3] == 0 && data[3][3] == 1;
}
bool Matrixd4x4::isIdentity() const {
    return data[0][0] == 1 && data[0][1] == 0 && data[0][2] == 0 && data[0][3] == 0     // row0
           && data[1][0] == 0 && data[1][1] == 1 && data[1][2] == 0 && data[1][3] == 0  // row1
           && data[2][0] == 0 && data[2][1] == 0 && data[2][2] == 0 && data[2][3] == 0  // row2
           && data[3][0] == 0 && data[3][1] == 0 && data[3][2] == 0 && data[3][3] == 1; // row3
}
double Matrixd4x4::operator()(int row, int col) const {
    return data[row][col];
}
double& Matrixd4x4::operator()(int row, int col) {
    return data[row][col];
}
Matrixd4x4 Matrixd4x4::operator*(const Matrixd4x4& right) const {
    Matrixd4x4 m;

    m.data[0][0] = data[0][0] * right.data[0][0] + data[0][1] * right.data[1][0] + data[0][2] * data[2][0] +
                   data[0][3] * right.data[3][0];
    m.data[0][1] = data[0][0] * right.data[0][1] + data[0][1] * right.data[1][1] + data[0][2] * data[2][1] +
                   data[0][3] * right.data[3][1];
    return m;
}
Matrixd4x4& Matrixd4x4::operator*=(const Matrixd4x4& right) {
    Matrixd4x4 m;
    return const_cast<Matrixd4x4&>(*this);
}

Matrixd4x4 Matrixd4x4::rotate(const Point3d& center, const Vector3d& axis, double angle) {
    Matrixd4x4 m;
    m.makeRotation(center, axis, angle);
    return m;
}
Matrixd4x4 Matrixd4x4::translate(const Vector3d& offset) {
    Matrixd4x4 m;
    m.makeTranslation(offset);
    return m;
}
Matrixd4x4 Matrixd4x4::translate(double x, double y, double z) {
    Matrixd4x4 m;
    m.makeTranslation(x, y, z);
    return m;
}
Matrixd4x4 Matrixd4x4::scale(const Vector3d& vec) {
    Matrixd4x4 m;
    m.makeScale(vec);
    return m;
}
Matrixd4x4 Matrixd4x4::scale(double x, double y, double z) {
    Matrixd4x4 m;
    m.makeScale(x, y, z);
    return m;
}
Matrixd4x4 Matrixd4x4::scale(double factor) {
    Matrixd4x4 m;
    m.makeScale(factor);
    return m;
}
Matrixd4x4 Matrixd4x4::lookAt(const Point3d& eye, const Point3d& target, const Vector3d& up) {
    Matrixd4x4 m;
    m.makeLookAt(eye, target, up);
    return m;
}

Vector3d operator*(const Matrixd4x4& m, const Vector3d& v) {
    return v;
}
Vector3f operator*(const Matrixd4x4& m, const Vector3f& v) {
    return v;
}
Point3d operator*(const Matrixd4x4& m, const Point3d& p) {
    return p;
}
Point3f operator*(const Matrixd4x4& m, const Point3f& p) {
    return p;
}
VI_GE_NS_END