#include <vine/math/Rect2.hpp>

#include <algorithm>
#include <cstdint>

#include <vine/math/Math.hpp>

V_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX bool Rect2<T>::contains(T x, T y) const
{
    auto mx = std::min(xmin, xmax), Mx = std::max(xmin, xmax);
    auto my = std::min(ymin, ymax), My = std::max(ymin, ymax);
    return x >= mx && x <= Mx && y >= my && y <= My;
}

TMPL_PREFIX bool Rect2<T>::contains(const Point2<T>& pt) const
{
    auto mx = std::min(xmin, xmax), Mx = std::max(xmin, xmax);
    auto my = std::min(ymin, ymax), My = std::max(ymin, ymax);
    return pt.x >= mx && pt.x <= Mx && pt.y >= my && pt.y <= My;
}

TMPL_PREFIX void Rect2<T>::expandBy(const Point2<T>& pt)
{
    auto mx = std::min(xmin, xmax), Mx = std::max(xmin, xmax);
    auto my = std::min(ymin, ymax), My = std::max(ymin, ymax);
    xmin = std::min<T>(mx, pt.x);
    ymin = std::min<T>(my, pt.y);
    xmax = std::max<T>(Mx, pt.x);
    ymax = std::max<T>(My, pt.y);
}

TMPL_PREFIX void Rect2<T>::expandBy(const Rect2<T>& rect)
{
    auto mx  = std::min(xmin, xmax),      Mx  = std::max(xmin, xmax);
    auto my  = std::min(ymin, ymax),      My  = std::max(ymin, ymax);
    auto rmx = std::min(rect.xmin, rect.xmax), rMx = std::max(rect.xmin, rect.xmax);
    auto rmy = std::min(rect.ymin, rect.ymax), rMy = std::max(rect.ymin, rect.ymax);
    xmin = std::min<T>(mx, rmx);
    ymin = std::min<T>(my, rmy);
    xmax = std::max<T>(Mx, rMx);
    ymax = std::max<T>(My, rMy);
}

TMPL_PREFIX bool Rect2<T>::intersectWith(const Rect2<T>& rect, Rect2<T>& out) const
{
    auto ax0 = std::min(xmin, xmax), ax1 = std::max(xmin, xmax);
    auto ay0 = std::min(ymin, ymax), ay1 = std::max(ymin, ymax);
    auto bx0 = std::min(rect.xmin, rect.xmax), bx1 = std::max(rect.xmin, rect.xmax);
    auto by0 = std::min(rect.ymin, rect.ymax), by1 = std::max(rect.ymin, rect.ymax);

    auto ix0 = std::max<T>(ax0, bx0);
    auto iy0 = std::max<T>(ay0, by0);
    auto ix1 = std::min<T>(ax1, bx1);
    auto iy1 = std::min<T>(ay1, by1);

    out.xmin = std::min<T>(ix0, ix1);
    out.ymin = std::min<T>(iy0, iy1);
    out.xmax = std::max<T>(ix0, ix1);
    out.ymax = std::max<T>(iy0, iy1);

    return ix0 <= ix1 && iy0 <= iy1;
}

#undef TMPL_PREFIX

template class V_MATH_API Rect2<float>;
template class V_MATH_API Rect2<double>;
template class V_MATH_API Rect2<int8_t>;
template class V_MATH_API Rect2<uint8_t>;
template class V_MATH_API Rect2<int16_t>;
template class V_MATH_API Rect2<uint16_t>;
template class V_MATH_API Rect2<int32_t>;
template class V_MATH_API Rect2<uint32_t>;
template class V_MATH_API Rect2<int64_t>;
template class V_MATH_API Rect2<uint64_t>;

V_MATH_NS_END
