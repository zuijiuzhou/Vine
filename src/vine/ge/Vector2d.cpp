#include <vine/ge/Vector2d.h>
#include <vine/ge/Point2d.h>
#include <cmath>

VI_GE_NS_BEGIN
Vector2d::Vector2d() : Vector2d(0., 0.)
{
}
Vector2d::Vector2d(double xx, double yy)
	: x(xx), y(yy)
{
}

double Vector2d::dotProduct(const Vector2d& vec) const
{
	return x * vec.x + y * vec.y;
}
double Vector2d::crossProduct(const Vector2d& vec) const
{
	return x * vec.y - y * vec.x;
}

void Vector2d::normalize()
{
	double len = length();
	if (len == 0)
	{
		return;
	}
	*this /= len;
}

Vector2d Vector2d::perpVector() const
{
	return Vector2d(-y, x);
}

void Vector2d::setLength(double len)
{
	double currentLen = length();
	if (currentLen == 0)
	{
		Vector2d v;
		v.x = v.y = len / sqrt(2.);
	}
	else
	{
		*this *= (len / currentLen);
	}
}
void Vector2d::set(double xx, double yy, double zz)
{
	x = xx;
	y = yy;
}
void Vector2d::get(double& xx, double& yy, double& zz) const
{
	xx = x;
	yy = y;
}
// void Vector2d::rotate(const Matrix4x4 &mat)
// {
// }
double Vector2d::angleTo(const Vector2d& vec) const
{
	if (isZero() || vec.isZero())
		return 0.;
	auto cos = (*this * vec) / (length() * vec.length());
	if (cos > 1.0)
		cos = 1.;
	else if (cos < -1.)
		cos = 1.;
	return acos(cos);
}

double Vector2d::length() const
{
	return sqrt(x * x + y * y);
}

double Vector2d::lengthSqrd() const
{
	return x * x + y * y;
}

bool Vector2d::isZero(double eps) const
{
	return ge::isEqual(x, 0., eps) && ge::isEqual(y, 0., eps);
}

bool Vector2d::isParalleTo(const Vector2d& vec, double eps) const
{
	if (isZero(eps) || vec.isZero(eps))
		return true;
	return ge::isZero(crossProduct(vec), eps);
}

bool Vector2d::isPerpendicularTo(const Vector2d& vec, double eps) const
{
	if (isZero(eps) || vec.isZero(eps))
		return true;
	return ge::isZero(dotProduct(vec), eps);
}

Point2d Vector2d::toPoint() const
{
	return Point2d(x, y);
}

const Point2d& Vector2d::asPoint()
{
	return reinterpret_cast<const Point2d&>(*this);
}

bool Vector2d::operator==(const Vector2d& right) const
{
	return x == right.x && y == right.y;
}
bool Vector2d::operator!=(const Vector2d& right) const
{
	return !(*this == right);
}
Vector2d Vector2d::operator-(const Vector2d& right) const
{
	return Vector2d(x - right.x, y - right.y);
}
Vector2d Vector2d::operator+(const Vector2d& right) const
{
	return Vector2d(x + right.x, y + right.y);
}
Vector2d& Vector2d::operator+=(const Vector2d& right)
{
	x += right.x;
	y += right.y;
	return *this;
}
Vector2d& Vector2d::operator-=(const Vector2d& right)
{
	x -= right.x;
	y -= right.y;
	return *this;
}
Vector2d Vector2d::operator*(double scale) const
{
	Vector2d v(*this);
	v.x *= scale;
	v.y *= scale;
	return v;
}
Vector2d& Vector2d::operator*=(double scale)
{
	x *= scale;
	y *= scale;
	return *this;
}
Vector2d Vector2d::operator/(double scale) const
{
	Vector2d v(*this);
	v.x /= scale;
	v.y /= scale;
	return v;
}
Vector2d& Vector2d::operator/=(double scale)
{
	x /= scale;
	y /= scale;
	return *this;
}
double Vector2d::operator^(const Vector2d& vec) const
{
	return crossProduct(vec);
}
double Vector2d::operator*(const Vector2d& vec) const
{
	return dotProduct(vec);
}
VI_GE_NS_END