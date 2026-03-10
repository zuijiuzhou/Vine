#include <vine/math/Rect2.hpp>

#include <algorithm>
#include <cstdint>

#include <vine/math/Math.hpp>

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

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

TMPL_PREFIX Rect2<T> Rect2<T>::intersectWith(const Rect2<T>& rect) const
{
    auto lb1 = bottomLeft();
    auto ub1 = topRight();
    auto lb2 = rect.bottomLeft();
    auto ub2 = rect.topRight();

    auto ix0 = std::max<T>(lb1.x, lb2.x);
    auto iy0 = std::max<T>(lb1.y, lb2.y);
    auto ix1 = std::min<T>(ub1.x, ub2.x);
    auto iy1 = std::min<T>(ub1.y, ub2.y);

    if (ix1 < ix0 || iy1 < iy0) {
        return Rect2<T>();
    }

    return Rect2<T>(ix0, iy0, ix1 - ix0, iy1 - iy0);
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
