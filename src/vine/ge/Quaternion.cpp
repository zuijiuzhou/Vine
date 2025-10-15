#include <vine/ge/Quaternion.hpp>

#include "Comm.hpp"

VI_GE_NS_BEGIN

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

TMPL_PREFIX Quaternion<T> Quaternion<T>::inverse() const
{
    return conj() / lenght2();
}

TMPL_PREFIX void Quaternion<T>::makeRotate(T angle, const Vector3<T>& axis)
{}

TMPL_PREFIX void Quaternion<T>::makeRotate(const Vector3<T>& from, const Vector3<T>& to)
{}

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
    Quaternion<T> q;
    q.x = x / right;
    q.y = y / right;
    q.z = z / right;
    q.w = w / right;
    return q;
}

TMPL_PREFIX Quaternion<T>& Quaternion<T>::operator/=(T right)
{
    x /= right;
    y /= right;
    z /= right;
    w /= right;
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
    return *this;
}

TMPL_PREFIX Quaternion<T> Quaternion<T>::operator*(const Quaternion& right) const
{
    Quaternion<T> q;
    return q;
}

TMPL_PREFIX Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& right)
{
    return *this;
}

TMPL_PREFIX Quaternion<T> Quaternion<T>::operator/(const Quaternion& right) const
{
    Quaternion<T> q;
    return q;
}

TMPL_PREFIX Quaternion<T>& Quaternion<T>::operator/=(const Quaternion& right)
{
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

template class VI_GE_API Quaternion<float>;
template class VI_GE_API Quaternion<double>;

VI_GE_NS_END