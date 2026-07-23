#include <vine/math/Rect3.hpp>

#include <algorithm>
#include <cstdint>

#include <vine/math/Math.hpp>
#include <vine/math/Point3.hpp>
#include <vine/math/Vector3.hpp>


V_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX bool Rect3<T>::contains(T x, T y, T z) const
{
    auto mx = std::min(xmin, xmax), Mx = std::max(xmin, xmax);
    auto my = std::min(ymin, ymax), My = std::max(ymin, ymax);
    auto mz = std::min(zmin, zmax), Mz = std::max(zmin, zmax);
    return x >= mx && x <= Mx && y >= my && y <= My && z >= mz && z <= Mz;
}

TMPL_PREFIX bool Rect3<T>::contains(const Point3<T>& pt) const
{
    auto mx = std::min(xmin, xmax), Mx = std::max(xmin, xmax);
    auto my = std::min(ymin, ymax), My = std::max(ymin, ymax);
    auto mz = std::min(zmin, zmax), Mz = std::max(zmin, zmax);
    return pt.x >= mx && pt.x <= Mx && pt.y >= my && pt.y <= My && pt.z >= mz && pt.z <= Mz;
}

TMPL_PREFIX void Rect3<T>::expandBy(const Point3<T>& pt)
{
    auto mx = std::min(xmin, xmax), Mx = std::max(xmin, xmax);
    auto my = std::min(ymin, ymax), My = std::max(ymin, ymax);
    auto mz = std::min(zmin, zmax), Mz = std::max(zmin, zmax);
    xmin = std::min<T>(mx, pt.x);
    ymin = std::min<T>(my, pt.y);
    zmin = std::min<T>(mz, pt.z);
    xmax = std::max<T>(Mx, pt.x);
    ymax = std::max<T>(My, pt.y);
    zmax = std::max<T>(Mz, pt.z);
}

TMPL_PREFIX void Rect3<T>::expandBy(const Rect3<T>& rect)
{
    auto mx  = std::min(xmin, xmax),      Mx  = std::max(xmin, xmax);
    auto my  = std::min(ymin, ymax),      My  = std::max(ymin, ymax);
    auto mz  = std::min(zmin, zmax),      Mz  = std::max(zmin, zmax);
    auto rmx = std::min(rect.xmin, rect.xmax), rMx = std::max(rect.xmin, rect.xmax);
    auto rmy = std::min(rect.ymin, rect.ymax), rMy = std::max(rect.ymin, rect.ymax);
    auto rmz = std::min(rect.zmin, rect.zmax), rMz = std::max(rect.zmin, rect.zmax);
    xmin = std::min<T>(mx, rmx);
    ymin = std::min<T>(my, rmy);
    zmin = std::min<T>(mz, rmz);
    xmax = std::max<T>(Mx, rMx);
    ymax = std::max<T>(My, rMy);
    zmax = std::max<T>(Mz, rMz);
}

TMPL_PREFIX bool Rect3<T>::intersectWith(const Rect3<T>& rect, Rect3<T>& out) const
{
    auto ax0 = std::min(xmin, xmax), ax1 = std::max(xmin, xmax);
    auto ay0 = std::min(ymin, ymax), ay1 = std::max(ymin, ymax);
    auto az0 = std::min(zmin, zmax), az1 = std::max(zmin, zmax);
    auto bx0 = std::min(rect.xmin, rect.xmax), bx1 = std::max(rect.xmin, rect.xmax);
    auto by0 = std::min(rect.ymin, rect.ymax), by1 = std::max(rect.ymin, rect.ymax);
    auto bz0 = std::min(rect.zmin, rect.zmax), bz1 = std::max(rect.zmin, rect.zmax);

    auto ix0 = std::max<T>(ax0, bx0);
    auto iy0 = std::max<T>(ay0, by0);
    auto iz0 = std::max<T>(az0, bz0);
    auto ix1 = std::min<T>(ax1, bx1);
    auto iy1 = std::min<T>(ay1, by1);
    auto iz1 = std::min<T>(az1, bz1);

    out.xmin = std::min<T>(ix0, ix1);
    out.ymin = std::min<T>(iy0, iy1);
    out.zmin = std::min<T>(iz0, iz1);
    out.xmax = std::max<T>(ix0, ix1);
    out.ymax = std::max<T>(iy0, iy1);
    out.zmax = std::max<T>(iz0, iz1);

    return ix0 <= ix1 && iy0 <= iy1 && iz0 <= iz1;
}

#undef TMPL_PREFIX

template class V_MATH_API Rect3<float>;
template class V_MATH_API Rect3<double>;
template class V_MATH_API Rect3<int8_t>;
template class V_MATH_API Rect3<uint8_t>;
template class V_MATH_API Rect3<int16_t>;
template class V_MATH_API Rect3<uint16_t>;
template class V_MATH_API Rect3<int32_t>;
template class V_MATH_API Rect3<uint32_t>;
template class V_MATH_API Rect3<int64_t>;
template class V_MATH_API Rect3<uint64_t>;

V_MATH_NS_END
