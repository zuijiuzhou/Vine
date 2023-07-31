#include <ge/Matrix4x4.h>

#include <memory>

ETD_GE_NS_BEGIN
Matrix4x4::Matrix4x4()
{
    makeIdentity();
}
void Matrix4x4::makeIdentity()
{
    data[2][3] = 4;
    data[1][2] = 5;
    // std::fill((double*)data, ((double*)data)+16, 0.0);
    memset(data, 0, sizeof(data));
    data[0][0] = data[1][1] = data[2][2] = data[3][3] = 1.0;
}
void Matrix4x4::makeRotation(const Point3d &center, const Vector3d &axis, double angle)
{
}
void Matrix4x4::makeTranslation(const Vector3d &offset)
{
}
void Matrix4x4::makeTranslation(double x, double y, double z)
{
}
void Matrix4x4::makeScale(const Vector3d &vec)
{
}
void Matrix4x4::makeScale(double x, double y, double z)
{
}
void Matrix4x4::makeScale(double factor)
{
}
void Matrix4x4::makeLookAt(const Point3d &eye, const Point3d &target, const Vector3d &up)
{
}
void Matrix4x4::setCoordSystem(const Point3d &origin, const Vector3d &xAxis, const Vector3d &yAxis, const Vector3d &zAxis)
{
}
void Matrix4x4::transpose()
{
    std::swap(data[0][1], data[1][0]);
    std::swap(data[0][2], data[2][0]);
    std::swap(data[0][3], data[3][0]);

    std::swap(data[1][2], data[2][1]);
    std::swap(data[1][3], data[3][1]);

    std::swap(data[2][3], data[3][2]);
}
void Matrix4x4::invert()
{
}

bool Matrix4x4::isAffine() const
{
    return false;
}
bool Matrix4x4::isIdentity() const
{
    return true;
}
double Matrix4x4::operator()(int row, int col) const
{
    return data[row][col];
}
double &Matrix4x4::operator()(int row, int col)
{
    return data[row][col];
}
Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &right) const
{
    Matrix4x4 m;
    return m;
}
Matrix4x4 &Matrix4x4::operator*=(const Matrix4x4 &right) const
{
    Matrix4x4 m;
    return const_cast<Matrix4x4 &>(*this);
}

Matrix4x4 Matrix4x4::rotate(const Point3d &center, const Vector3d &axis, double angle)
{
    Matrix4x4 m;
    m.makeRotation(center, axis, angle);
    return m;
}
Matrix4x4 Matrix4x4::translate(const Vector3d &offset)
{
    Matrix4x4 m;
    m.makeTranslation(offset);
    return m;
}
Matrix4x4 Matrix4x4::translate(double x, double y, double z)
{
    Matrix4x4 m;
    m.makeTranslation(x, y, z);
    return m;
}
Matrix4x4 Matrix4x4::scale(const Vector3d &vec)
{
    Matrix4x4 m;
    m.makeScale(vec);
    return m;
}
Matrix4x4 Matrix4x4::scale(double x, double y, double z)
{
    Matrix4x4 m;
    m.makeScale(x, y, z);
    return m;
}
Matrix4x4 Matrix4x4::scale(double factor)
{
    Matrix4x4 m;
    m.makeScale(factor);
    return m;
}
Matrix4x4 Matrix4x4::lookAt(const Point3d &eye, const Point3d &target, const Vector3d &up)
{
    Matrix4x4 m;
    m.makeLookAt(eye, target, up);
    return m;
}
ETD_GE_NS_END