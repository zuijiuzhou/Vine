#include <ge/Vector2i.h>
#include <ge/Ge.h>
#include <ge/Point2i.h>
#include <cmath>

VI_GE_NS_BEGIN
Vector2i::Vector2i() : Vector2i(0, 0)
{
}
Vector2i::Vector2i(int xx, int yy)
    : x(xx), y(yy)
{
}

int Vector2i::dotProduct(const Vector2i &vec) const
{
    return x * vec.x + y * vec.y;
}
int Vector2i::crossProduct(const Vector2i &vec) const
{
    return x * vec.y - y * vec.x;
}

Vector2i Vector2i::perpVector() const
{
    return Vector2i(-y, x);
}

void Vector2i::set(int xx, int yy)
{
    x = xx;
    y = yy;
}
void Vector2i::get(int &xx, int &yy) const
{
    xx = x;
    yy = y;
}
// void Vector2i::rotate(const Matrix4x4 &mat)
// {
// }
double Vector2i::angleTo(const Vector2i &vec) const
{
    if (isZero() || vec.isZero())
        return 0;
    auto cos = (*this * vec) / (length() * vec.length());
    if (cos > 1.)
        cos = 1.;
    else if (cos < -1.)
        cos = -1.;
    return acos(cos);
}

double Vector2i::length() const
{
    return sqrt(x * x + y * y);
}

int Vector2i::lengthSqrd() const
{
    return x * x + y * y;
}

bool Vector2i::isZero() const
{
    return x == 0 && y == 0;
}
bool Vector2i::isParalleTo(const Vector2i &vec) const
{
    return crossProduct(vec) == 0;
}

bool Vector2i::isPerpendicularTo(const Vector2i &vec) const
{
    return dotProduct(vec) == 0;
}

Point2i Vector2i::toPoint() const
{
    return Point2i(x, y);
}

const Point2i &Vector2i::asPoint() const
{
    return reinterpret_cast<const Point2i &>(*this);
}

bool Vector2i::operator==(const Vector2i &right) const
{
    return x == right.x && y == right.y;
}
bool Vector2i::operator!=(const Vector2i &right) const
{
    return !(*this == right);
}
Vector2i Vector2i::operator-(const Vector2i &right) const
{
    return Vector2i(x - right.x, y - right.y);
}
Vector2i Vector2i::operator+(const Vector2i &right) const
{
    return Vector2i(x + right.x, y + right.y);
}
Vector2i &Vector2i::operator+=(const Vector2i &right)
{
    x += right.x;
    y += right.y;
    return *this;
}
Vector2i &Vector2i::operator-=(const Vector2i &right)
{
    x -= right.x;
    y -= right.y;
    return *this;
}
Vector2i Vector2i::operator*(int scale) const
{
    Vector2i v(*this);
    v.x *= scale;
    v.y *= scale;
    return v;
}
Vector2i &Vector2i::operator*=(int scale)
{
    x *= scale;
    y *= scale;
    return *this;
}
Vector2i Vector2i::operator/(int scale) const
{
    Vector2i v(*this);
    v.x /= scale;
    v.y /= scale;
    return v;
}
Vector2i &Vector2i::operator/=(int scale)
{
    x /= scale;
    y /= scale;
    return *this;
}
int Vector2i::operator^(const Vector2i &vec) const
{
    return crossProduct(vec);
}
int Vector2i::operator*(const Vector2i &vec) const
{
    return dotProduct(vec);
}
VI_GE_NS_END