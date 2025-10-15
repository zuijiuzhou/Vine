#include <vine/ge/Quaternion.hpp>

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
{}

TMPL_PREFIX Quaternion<T> Quaternion<T>::operator*(const Quaternion& right) const
{
    Quaternion<T> q;
    q.x = x + right.x;
    q.y = y + right.y;
    q.z = z + right.z;
    q.w = w + right.w;
    return q;
}

TMPL_PREFIX Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& right)
{}

TMPL_PREFIX Quaternion<T> Quaternion<T>::operator/(const Quaternion& right) const
{
    Quaternion<T> q;
    q.x = x / right.x;
    q.y = y / right.y;
    q.z = z / right.z;
    q.w = w / right.w;
    return q;
}

TMPL_PREFIX Quaternion<T>& Quaternion<T>::operator/=(const Quaternion& right)
{}

TMPL_PREFIX T& Quaternion<T>::operator[](size_t i)
{}

TMPL_PREFIX T Quaternion<T>::operator[](size_t i) const
{}

#undef TMPL_PREFIX

template class VI_GE_API Quaternion<float>;
template class VI_GE_API Quaternion<double>;

VI_GE_NS_END