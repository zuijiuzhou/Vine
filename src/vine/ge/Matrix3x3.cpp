#include <vine/ge/Matrix3x3.h>

#include <cstring>
#include <memory>

VI_GE_NS_BEGIN
Matrix3x3::Matrix3x3()
{
    makeIdentity();
}
void Matrix3x3::makeIdentity()
{
    // std::fill((double*)data, ((double*)data)+16, 0.0);
    memset(data, 0, sizeof(data));
    data[0][0] = data[1][1] = data[2][2] = 1.0;
}
void Matrix3x3::makeRotation(const Point2d &center, double angle)
{
}
void Matrix3x3::makeTranslation(const Vector2d &offset)
{
}
void Matrix3x3::makeTranslation(double x, double y)
{
}
void Matrix3x3::makeScale(const Vector2d &vec)
{
}
void Matrix3x3::makeScale(double x, double y)
{
}
void Matrix3x3::makeScale(double factor)
{
}
void Matrix3x3::setCoordSystem(const Point2d &origin, const Vector2d &xAxis, const Vector2d &yAxis)
{
}
void Matrix3x3::transpose()
{
    std::swap(data[0][1], data[1][0]);
    std::swap(data[0][2], data[2][0]);

    std::swap(data[1][2], data[2][1]);
}
void Matrix3x3::invert()
{
}

bool Matrix3x3::isIdentity() const
{
    return true;
}
double Matrix3x3::operator()(int row, int col) const
{
    return data[row][col];
}
double &Matrix3x3::operator()(int row, int col)
{
    return data[row][col];
}
Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &right) const
{
    Matrix3x3 m;
    return m;
}
Matrix3x3 &Matrix3x3::operator*=(const Matrix3x3 &right) const
{
    Matrix3x3 m;
    return const_cast<Matrix3x3 &>(*this);
}

Matrix3x3 Matrix3x3::rotate(const Point2d &center, double angle)
{
    Matrix3x3 m;
    m.makeRotation(center, angle);
    return m;
}
Matrix3x3 Matrix3x3::translate(const Vector2d &offset)
{
    Matrix3x3 m;
    m.makeTranslation(offset);
    return m;
}
Matrix3x3 Matrix3x3::translate(double x, double y)
{
    Matrix3x3 m;
    m.makeTranslation(x, y);
    return m;
}
Matrix3x3 Matrix3x3::scale(const Vector2d &vec)
{
    Matrix3x3 m;
    m.makeScale(vec);
    return m;
}
Matrix3x3 Matrix3x3::scale(double x, double y)
{
    Matrix3x3 m;
    m.makeScale(x, y);
    return m;
}
Matrix3x3 Matrix3x3::scale(double factor)
{
    Matrix3x3 m;
    m.makeScale(factor);
    return m;
}
VI_GE_NS_END