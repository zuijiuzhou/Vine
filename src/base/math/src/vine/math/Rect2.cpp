#include <vine/math/Rect2.hpp>

#include <algorithm>
#include <cstdint>

#include <vine/math/Math.hpp>

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Rect2<T>::Rect2()
  : x(T())
  , y(T())
  , w(T())
  , h(T())
{}

TMPL_PREFIX Rect2<T>::Rect2(const Point2<T>& top_left, const Vector2<T>& size)
  : x(top_left.x)
  , y(top_left.y)
  , w(size.x)
  , h(size.y)
{}

TMPL_PREFIX Rect2<T>::Rect2(T xx, T yy, T ww, T hh)
  : x(xx)
  , y(yy)
  , w(ww)
  , h(hh)
{}

TMPL_PREFIX T Rect2<T>::top() const
{
    return std::max<T>(y, y + h);
}

TMPL_PREFIX T Rect2<T>::bottom() const
{
    return std::min<T>(y, y + h);
}

TMPL_PREFIX T Rect2<T>::left() const
{
    return std::min<T>(x, x + w);
}

TMPL_PREFIX T Rect2<T>::right() const
{
    return std::max<T>(x, x + w);
}

TMPL_PREFIX Point2<T> Rect2<T>::bottomLeft() const
{
    return Point2<T>(std::min<T>(x, x + w), std::min<T>(y, y + h));
}

TMPL_PREFIX Point2<T> Rect2<T>::bottomRight() const
{
    return Point2<T>(std::max<T>(x, x + w), std::min<T>(y, y + h));
}

TMPL_PREFIX Point2<T> Rect2<T>::topLeft() const
{
    return Point2<T>(std::min<T>(x, x + w), std::max<T>(y, y + h));
}

TMPL_PREFIX Point2<T> Rect2<T>::topRight() const
{
    return Point2<T>(std::max<T>(x, x + w), std::max<T>(y, y + h));
}

TMPL_PREFIX T Rect2<T>::width() const
{
    return w < 0 ? -w : w;
}

TMPL_PREFIX T Rect2<T>::height() const
{
    return h < 0 ? -h : h;
}

TMPL_PREFIX Vector2<T> Rect2<T>::size() const
{
    return Vector2<T>(w < 0 ? -w : w, h < 0 ? -h : h);
}

TMPL_PREFIX bool Rect2<T>::contains(T x, T y) const
{
    auto l = bottomLeft();
    auto u = topRight();
    return x >= l.x && x <= u.x && y >= l.y && y <= u.y;
}

TMPL_PREFIX bool Rect2<T>::contains(const Point2<T>& pt) const
{
    auto l = bottomLeft();
    auto u = topRight();
    return pt.x >= l.x && pt.x <= u.x && pt.y >= l.y && pt.y <= u.y;
}

TMPL_PREFIX void Rect2<T>::expandBy(const Vector2<T>& pt)
{
    auto l = bottomLeft();
    auto u = topRight();
    l.x    = std::min<T>(l.x, pt.x);
    l.y    = std::min<T>(l.y, pt.y);
    u.x    = std::max<T>(u.x, pt.x);
    u.y    = std::max<T>(u.y, pt.y);
    x      = l.x;
    y      = l.y;
    w      = u.x - l.x;
    h      = u.y - l.y;
}

TMPL_PREFIX void Rect2<T>::expandBy(const Rect2<T>& rect)
{
    auto lb1 = bottomLeft();
    auto ub1 = topRight();
    auto lb2 = rect.bottomLeft();
    auto ub2 = rect.topRight();

    lb1.x = std::min<T>(lb1.x, lb2.x);
    lb1.y = std::min<T>(lb1.y, lb2.y);
    ub1.x = std::max<T>(ub1.x, ub2.x);
    ub1.y = std::max<T>(ub1.y, ub2.y);

    x = lb1.x;
    y = lb1.y;

    w = ub1.x - lb1.x;
    h = ub1.y - lb1.y;
}

TMPL_PREFIX bool Rect2<T>::isZero() const
{
    return x == T(0) && y == T(0) && w == T(0) && h == T(0);
}

TMPL_PREFIX bool Rect2<T>::isZero(T eps) const
{
    return math::isZero(x, eps) && math::isZero(y, eps) && math::isZero(w, eps) && math::isZero(h, eps);
}

TMPL_PREFIX bool Rect2<T>::operator==(const Rect2<T>& right) const
{
    return x == right.x && y == right.y && w == right.w && h == right.h;
}

TMPL_PREFIX bool Rect2<T>::operator!=(const Rect2<T>& right) const
{
    return !(*this == right);
}

#undef TMPL_PREFIX

template class VI_MATH_API Rect2<float>;
template class VI_MATH_API Rect2<double>;
template class VI_MATH_API Rect2<int8_t>;
template class VI_MATH_API Rect2<uint8_t>;
template class VI_MATH_API Rect2<int16_t>;
template class VI_MATH_API Rect2<uint16_t>;
template class VI_MATH_API Rect2<int32_t>;
template class VI_MATH_API Rect2<uint32_t>;
template class VI_MATH_API Rect2<int64_t>;
template class VI_MATH_API Rect2<uint64_t>;

VI_MATH_NS_END