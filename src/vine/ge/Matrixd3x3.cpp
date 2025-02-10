#include <vine/ge/Matrixd3x3.h>

#include <cstring>
#include <memory>

VI_GE_NS_BEGIN
Matrixd3x3::Matrixd3x3()
{
    makeIdentity();
}
void Matrixd3x3::makeIdentity()
{
    // std::fill((double*)data, ((double*)data)+16, 0.0);
    memset(data, 0, sizeof(data));
    data[0][0] = data[1][1] = data[2][2] = 1.0;
}
void Matrixd3x3::makeRotation(const Point2d &center, double angle)
{
}
void Matrixd3x3::makeTranslation(const Vector2d &offset)
{
}
void Matrixd3x3::makeTranslation(double x, double y)
{
}
void Matrixd3x3::makeScale(const Vector2d &vec)
{
}
void Matrixd3x3::makeScale(double x, double y)
{
}
void Matrixd3x3::makeScale(double factor)
{
}
void Matrixd3x3::setCoordSystem(const Point2d &origin, const Vector2d &xAxis, const Vector2d &yAxis)
{
}
void Matrixd3x3::transpose()
{
    std::swap(data[0][1], data[1][0]);
    std::swap(data[0][2], data[2][0]);

    std::swap(data[1][2], data[2][1]);
}
void Matrixd3x3::invert()
{
}

bool Matrixd3x3::isIdentity() const
{
    return true;
}
double Matrixd3x3::operator()(int row, int col) const
{
    return data[row][col];
}
double &Matrixd3x3::operator()(int row, int col)
{
    return data[row][col];
}
Matrixd3x3 Matrixd3x3::operator*(const Matrixd3x3 &right) const
{
    Matrixd3x3 m;
    return m;
}
Matrixd3x3 &Matrixd3x3::operator*=(const Matrixd3x3 &right) const
{
    Matrixd3x3 m;
    return const_cast<Matrixd3x3 &>(*this);
}

Matrixd3x3 Matrixd3x3::rotate(const Point2d &center, double angle)
{
    Matrixd3x3 m;
    m.makeRotation(center, angle);
    return m;
}
Matrixd3x3 Matrixd3x3::translate(const Vector2d &offset)
{
    Matrixd3x3 m;
    m.makeTranslation(offset);
    return m;
}
Matrixd3x3 Matrixd3x3::translate(double x, double y)
{
    Matrixd3x3 m;
    m.makeTranslation(x, y);
    return m;
}
Matrixd3x3 Matrixd3x3::scale(const Vector2d &vec)
{
    Matrixd3x3 m;
    m.makeScale(vec);
    return m;
}
Matrixd3x3 Matrixd3x3::scale(double x, double y)
{
    Matrixd3x3 m;
    m.makeScale(x, y);
    return m;
}
Matrixd3x3 Matrixd3x3::scale(double factor)
{
    Matrixd3x3 m;
    m.makeScale(factor);
    return m;
}
VI_GE_NS_END