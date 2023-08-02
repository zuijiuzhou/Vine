#include <ge/Vector3d.h>
#include <ge/Ge.h>

#include <ge/Vector2d.h>
#include <ge/Point3d.h>
#include <ge/Matrix4x4.h>

#include <cmath>

VINE_GE_NS_BEGIN
Vector3d::Vector3d() : Vector3d(0., 0., 0.)
{
}

Vector3d::Vector3d(double xx, double yy, double zz)
    : x(xx), y(yy), z(zz)
{
}

double Vector3d::dotProduct(const Vector3d &vec) const
{
    return x * vec.x + y * vec.y + z * vec.z;
}
Vector3d Vector3d::crossProduct(const Vector3d &vec) const
{
    Vector3d v;
    v.x = y * vec.z - z * vec.y;
    v.y = z * vec.x - x * vec.z;
    v.z = x * vec.y - y * vec.x;
    return v;
}

void Vector3d::normalize()
{
    double len = length();
    if (len == 0)
    {
        return;
    }
    *this /= len;
}

Vector3d Vector3d::perpVector() const
{
    if (x != 0. || y != 0)
    {
        return Vector3d(y, -x, 0.);
    }
    else
    {
        return Vector3d(z, 0., -x);
    }
}

void Vector3d::setLength(double len)
{
    double currentLen = length();
    if (currentLen == 0)
    {
        Vector3d v;
        v.x = v.y = v.z = len / sqrt(3.);
    }
    else
    {
        *this *= (len / currentLen);
    }
}
void Vector3d::set(double xx, double yy, double zz)
{
    x = xx;
    y = yy;
    z = zz;
}
void Vector3d::get(double &xx, double &yy, double &zz) const
{
    xx = x;
    yy = y;
    zz = z;
}
void Vector3d::rotate(const Matrix4x4 &mat)
{
}
double Vector3d::angleTo(const Vector3d &vec)
{
    if (isZeroLength() || vec.isZeroLength())
    {
        return 0.;
    }
    return acos((*this * vec) / (length() * vec.length()));
}
double Vector3d::angleTo(const Vector3d &vec, const Vector3d &refVec)
{
    auto rad = angleTo(vec);
    if ((*this ^ vec) * refVec > 0.)
    {
        return rad;
    }
    return PIx2 - rad;
}

double Vector3d::length() const
{
    return sqrt(x * x + y * y + z * z);
}

double Vector3d::lengthSqrd() const
{
    return x * x + y * y + z * z;
}

bool Vector3d::isZeroLength() const
{
    return x == 0. && y == 0. && z == 0.;
}
bool Vector3d::isParalleTo(const Vector3d &vec) const
{
    return crossProduct(vec).isZeroLength();
}

bool Vector3d::isPerpendicularTo(const Vector3d &vec) const
{
    return dotProduct(vec) == 0.;
}

Point3d Vector3d::toPoint() const
{
    return Point3d(x, y, z);
}

const Point3d &Vector3d::asPoint()
{
    return reinterpret_cast<const Point3d &>(*this);
}

Vector2d Vector3d::convert2d() const
{
    return Vector2d(x, y);
}

bool Vector3d::operator==(const Vector3d &right) const
{
    return x == right.x && y == right.y && z == right.z;
}
bool Vector3d::operator!=(const Vector3d &right) const
{
    return !(*this == right);
}
Vector3d Vector3d::operator-(const Vector3d &right) const
{
    return Vector3d(x - right.x, y - right.y, z - right.z);
}
Vector3d Vector3d::operator+(const Vector3d &right) const
{
    return Vector3d(x + right.x, y + right.y, z + right.z);
}
Vector3d &Vector3d::operator+=(const Vector3d &right)
{
    x += right.x;
    y += right.y;
    z += right.z;
    return *this;
}
Vector3d &Vector3d::operator-=(const Vector3d &right)
{
    x -= right.x;
    y -= right.y;
    z -= right.z;
    return *this;
}
Vector3d Vector3d::operator*(double scale) const
{
    Vector3d v(*this);
    v.x *= scale;
    v.y *= scale;
    v.z *= scale;
    return v;
}
Vector3d &Vector3d::operator*=(double scale)
{
    x *= scale;
    y *= scale;
    z *= scale;
    return *this;
}
Vector3d Vector3d::operator/(double scale) const
{
    Vector3d v(*this);
    v.x /= scale;
    v.y /= scale;
    v.z /= scale;
    return v;
}
Vector3d &Vector3d::operator/=(double scale)
{
    x /= scale;
    y /= scale;
    z /= scale;
    return *this;
}
Vector3d Vector3d::operator^(const Vector3d &vec) const
{
    return crossProduct(vec);
}
double Vector3d::operator*(const Vector3d &vec) const
{
    return dotProduct(vec);
}
VINE_GE_NS_END