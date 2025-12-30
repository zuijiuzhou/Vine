#include <vine/math/Quaternion.hpp>

#include "Comm.hpp"

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Quaternion<T>::Quaternion()
  : x(T())
  , y(T())
  , z(T())
  , w(T())
{}

TMPL_PREFIX Quaternion<T>::Quaternion(T x, T y, T z, T w)
  : x(x)
  , y(y)
  , z(z)
  , w(w)
{}

TMPL_PREFIX Quaternion<T>::Quaternion(T angle, const Vector3<T>& axis)
{
    makeRotate(angle, axis);
}

TMPL_PREFIX Quaternion<T>::Quaternion(const Vector3<T>& from, const Vector3<T>& to)
{
    makeRotate(from, to);
}

TMPL_PREFIX Vector4<T> Quaternion<T>::toVector() const
{
    return Vector4<T>(x, y, z, w);
}

TMPL_PREFIX const Vector4<T>& Quaternion<T>::asVector() const
{
    return reinterpret_cast<const Vector4<T>&>(*this);
}

TMPL_PREFIX T Quaternion<T>::lenght() const
{
    return calc_vec_len_safe(x, y, z, w);
}

TMPL_PREFIX T Quaternion<T>::lenght2() const
{
    return calc_vec_len2_safe(x, y, z, w);
}

TMPL_PREFIX Quaternion<T> Quaternion<T>::conj() const
{
    return Quaternion<T>(-x, -y, -z, w);
}

TMPL_PREFIX void Quaternion<T>::invert()
{
    // q-1 = conj / len2
    auto len2 = lenght2();
    auto rcp  = T(1) / len2;
    x         = -x * rcp;
    y         = -y * rcp;
    z         = -z * rcp;
    w         = w * rcp;
}

TMPL_PREFIX Quaternion<T> Quaternion<T>::inverted() const
{
    return conj() / lenght2();
}

TMPL_PREFIX void Quaternion<T>::makeRotate(T angle, const Vector3<T>& axis)
{
    // q = axis * sin(angle/2), cos(angel/2)
    if (angle) {
        auto len = axis.length();

        if (len) {
            auto len_rcp = T(1) / len;

            auto s    = sin(angle * 0.5);
            auto c    = cos(angle * 0.5);
            auto temp = s * len_rcp;
            x         = axis.x * temp;
            y         = axis.y * temp;
            z         = axis.z * temp;
            w         = c;
            return;
        }
    }

    x = T(0);
    y = T(0);
    z = T(0);
    w = T(1);
}

TMPL_PREFIX void Quaternion<T>::makeRotate(const Vector3<T>& from, const Vector3<T>& to)
{}

TMPL_PREFIX void Quaternion<T>::getRotate(T& o_angle, Vector3<T>& o_axis) const
{
    // sin^2(θ/2) + cos^2(θ/2) = 1
    // w = cos(θ/2)
    // x^2 + y^2 + z^2 + w^2 = 1
    // x^2 + y^2 + z^2 = sin^2(θ/2)

    // 2 * acos(w)，θ趋向于0时，误差变大
    // 2 * atan2(sin(θ/2), w)更稳定

    // sin(θ/2)
    auto s  = sqrt(x * x + y * y + z * z);
    o_angle = T(2) * atan2(s, w);

    if (s) {
        o_axis.x = x;
        o_axis.y = y;
        o_axis.z = z;
        o_axis /= s;
    }
    else {
        o_axis.x = T(0);
        o_axis.y = T(0);
        o_axis.z = T(0);
    }
}

TMPL_PREFIX Quaternion<T> Quaternion<T>::slerp(const Quaternion<T>& from, const Quaternion<T>& to, T t)
{
    Quaternion<T> q;

    return q;
}

TMPL_PREFIX Quaternion<T> Quaternion<T>::operator*(T right) const
{
    Quaternion<T> q;
    q.x = x * right;
    q.y = y * right;
    q.z = z * right;
    q.w = w * right;
    return q;
}

TMPL_PREFIX Quaternion<T>& Quaternion<T>::operator*=(T right)
{
    x *= right;
    y *= right;
    z *= right;
    w *= right;
    return *this;
}

TMPL_PREFIX Quaternion<T> Quaternion<T>::operator/(T right) const
{
    auto          rcp = T(1) / right;
    Quaternion<T> q;
    q.x = x * rcp;
    q.y = y * rcp;
    q.z = z * rcp;
    q.w = w * rcp;
    return q;
}

TMPL_PREFIX Quaternion<T>& Quaternion<T>::operator/=(T right)
{
    auto rcp = T(1) / right;
    x *= rcp;
    y *= rcp;
    z *= rcp;
    w *= rcp;
    return *this;
}

TMPL_PREFIX bool Quaternion<T>::operator==(const Quaternion& right) const
{
    return x == right.x && y == right.y && z == right.z && w == right.w;
}

TMPL_PREFIX bool Quaternion<T>::operator!=(const Quaternion& right) const
{
    return !(*this == right);
}

TMPL_PREFIX Quaternion<T> Quaternion<T>::operator+(const Quaternion& right) const
{
    Quaternion<T> q;
    q.x = x + right.x;
    q.y = y + right.y;
    q.z = z + right.z;
    q.w = w + right.w;
    return q;
}

TMPL_PREFIX Quaternion<T>& Quaternion<T>::operator+=(const Quaternion& right)
{
    x += right.x;
    y += right.y;
    z += right.z;
    w += right.w;
    return *this;
}

TMPL_PREFIX Quaternion<T> Quaternion<T>::operator-(const Quaternion& right) const
{
    Quaternion<T> q;
    q.x = x + right.x;
    q.y = y + right.y;
    q.z = z + right.z;
    q.w = w + right.w;
    return q;
}

TMPL_PREFIX Quaternion<T>& Quaternion<T>::operator-=(const Quaternion& right)
{
    x -= right.x;
    y -= right.y;
    z -= right.z;
    w -= right.w;
    return *this;
}

TMPL_PREFIX Quaternion<T> Quaternion<T>::operator*(const Quaternion& right) const
{
    Quaternion<T> q;
    q.x = x * right.w + w * right.x + z * right.y - y * right.z;
    q.y = y * right.w - z * right.x + w * right.y + x * right.z;
    q.z = z * right.w + y * right.x - x * right.y + w * right.z;
    q.w = w * right.w - x * right.x - y * right.y - z * right.z;
    return q;
}

TMPL_PREFIX Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& right)
{
    auto x2 = x * right.w + w * right.x + z * right.y - y * right.z;
    auto y2 = y * right.w - z * right.x + w * right.y + x * right.z;
    auto z2 = z * right.w + y * right.x - x * right.y + w * right.z;
    w       = w * right.w - x * right.x - y * right.y - z * right.z;

    x = x2;
    y = y2;
    z = z2;

    return *this;
}

TMPL_PREFIX Quaternion<T> Quaternion<T>::operator/(const Quaternion& right) const
{
    auto q = (*this * right.inverted());
    return q;
}

TMPL_PREFIX Quaternion<T>& Quaternion<T>::operator/=(const Quaternion& right)
{
    *this = *this * right.inverted();
    return *this;
}

TMPL_PREFIX Quaternion<T> Quaternion<T>::operator-() const
{
    return Quaternion<T>(-x, -y, -z, -w);
}

TMPL_PREFIX T& Quaternion<T>::operator[](size_t i)
{
    return data[i];
}

TMPL_PREFIX T Quaternion<T>::operator[](size_t i) const
{
    return data[i];
}

#undef TMPL_PREFIX

template class VI_MATH_API Quaternion<float>;
template class VI_MATH_API Quaternion<double>;

VI_MATH_NS_END