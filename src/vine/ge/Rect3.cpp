#include <vine/ge/Rect3.hpp>

#include <algorithm>
#include <cstdint>

#include <vine/ge/Math.hpp>
#include <vine/ge/Point3.hpp>
#include <vine/ge/Vector3.hpp>


VI_GE_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Rect3<T>::Rect3()
  : x(T())
  , y(T())
  , z(T())
  , l(T())
  , w(T())
  , h(T())
{}

TMPL_PREFIX Rect3<T>::Rect3(const Point3<T>& conner, const Vector3<T>& size)
  : x(conner.x)
  , y(conner.y)
  , z(conner.z)
  , l(size.x)
  , w(size.y)
  , h(size.z)
{}

TMPL_PREFIX Rect3<T>::Rect3(T xx, T yy, T zz, T ll, T ww, T hh)
  : x(xx)
  , y(yy)
  , z(zz)
  , l(ll)
  , w(ww)
  , h(hh)
{}

TMPL_PREFIX Point3<T> Rect3<T>::lowerBound() const
{
    return Point3<T>(std::min<T>(x, x + l), std::min<T>(y, y + w), std::min<T>(z, z + h));
}

TMPL_PREFIX Point3<T> Rect3<T>::upperBound() const
{
    return Point3<T>(std::max<T>(x, x + l), std::max<T>(y, y + w), std::max<T>(z, z + h));
}

TMPL_PREFIX T Rect3<T>::length() const
{
    return l < 0 ? -l : l;
}

TMPL_PREFIX T Rect3<T>::width() const
{
    return w < 0 ? -w : w;
}

TMPL_PREFIX T Rect3<T>::height() const
{
    return h < 0 ? -h : h;
}

// TMPL_PREFIX Point3<T> Rect3<T>::topLeft() const {
//     return Point3<T>(x, y);
// }
// TMPL_PREFIX Point3<T> Rect3<T>::topRight() const {
//     return Point3<T>(x + w, y);
// }
// TMPL_PREFIX Point3<T> Rect3<T>::bottomLeft() const {
//     return Point3<T>(x, y + h);
// }
// TMPL_PREFIX Point3<T> Rect3<T>::bottomRight() const {
//     return Point3<T>(x + w, y + h);
// }
TMPL_PREFIX Vector3<T> Rect3<T>::size() const
{
    return Vector3<T>(l < 0 ? -l : l, w < 0 ? -w : w, h < 0 ? -h : h);
}

TMPL_PREFIX bool Rect3<T>::contains(T x, T y, T z) const
{
    auto l = lowerBound();
    auto u = upperBound();
    return x >= l.x && x <= u.x && y >= l.y && y <= u.y && z >= l.z && z <= u.z;
}

TMPL_PREFIX bool Rect3<T>::contains(const Point3<T>& pt) const
{
    auto l = lowerBound();
    auto u = upperBound();
    return pt.x >= l.x && pt.x <= u.x && pt.y >= l.y && pt.y <= u.y && pt.z >= l.z && pt.z <= u.z;
}

TMPL_PREFIX void Rect3<T>::expandBy(const Point3<T>& pt)
{
    auto lb = lowerBound();
    auto ub = upperBound();
    lb.x    = std::min<T>(lb.x, pt.x);
    lb.y    = std::min<T>(lb.y, pt.y);
    lb.z    = std::min<T>(lb.z, pt.z);
    ub.x    = std::max<T>(ub.x, pt.x);
    ub.y    = std::max<T>(ub.y, pt.y);
    ub.z    = std::max<T>(ub.z, pt.z);
    x       = lb.x;
    y       = lb.y;
    z       = lb.z;
    l       = ub.x - lb.x;
    w       = ub.y - lb.y;
    h       = ub.z - lb.z;
}

TMPL_PREFIX void Rect3<T>::expandBy(const Rect3<T>& rect)
{
    auto lb1 = lowerBound();
    auto ub1 = upperBound();
    auto lb2 = rect.lowerBound();
    auto ub2 = rect.upperBound();

    lb1.x = std::min<T>(lb1.x, lb2.x);
    lb1.y = std::min<T>(lb1.y, lb2.y);
    lb1.z = std::min<T>(lb1.z, lb2.z);
    ub1.x = std::max<T>(ub1.x, ub2.x);
    ub1.y = std::max<T>(ub1.y, ub2.y);
    ub1.z = std::max<T>(ub1.z, ub2.z);

    x = lb1.x;
    y = lb1.y;
    z = lb1.z;

    l = ub1.x - lb1.x;
    w = ub1.y - lb1.y;
    h = ub1.z - lb1.z;
}

TMPL_PREFIX Rect3<T> Rect3<T>::intersectWith(const Rect3<T>& rect)
{
    return Rect3<T>();
}

TMPL_PREFIX bool Rect3<T>::isZero() const
{
    return x == T(0) && y == T(0) && z == T(0) && l == T(0) && w == T(0) && h == T(0);
}

TMPL_PREFIX bool Rect3<T>::isZero(T eps) const
{
    return ge::isZero(x, eps) && ge::isZero(y, eps) && ge::isZero(z, eps) && ge::isZero(l, eps) && ge::isZero(w, eps) &&
           ge::isZero(h, eps);
}

TMPL_PREFIX bool Rect3<T>::operator==(const Rect3<T>& right) const
{
    return x == right.x && y == right.y && w == right.w && h == right.h;
}

TMPL_PREFIX bool Rect3<T>::operator!=(const Rect3<T>& right) const
{
    return !(*this == right);
}

#undef TMPL_PREFIX

template class VI_GE_API Rect3<float>;
template class VI_GE_API Rect3<double>;
template class VI_GE_API Rect3<int8_t>;
template class VI_GE_API Rect3<uint8_t>;
template class VI_GE_API Rect3<int16_t>;
template class VI_GE_API Rect3<uint16_t>;
template class VI_GE_API Rect3<int32_t>;
template class VI_GE_API Rect3<uint32_t>;
template class VI_GE_API Rect3<int64_t>;
template class VI_GE_API Rect3<uint64_t>;

VI_GE_NS_END