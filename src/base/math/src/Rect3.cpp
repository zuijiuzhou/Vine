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
    return x >= xmin && x <= xmax && y >= ymin && y <= ymax && z >= zmin && z <= zmax;
}

TMPL_PREFIX bool Rect3<T>::contains(const Point3<T>& pt) const
{
    return pt.x >= xmin && pt.x <= xmax && pt.y >= ymin && pt.y <= ymax && pt.z >= zmin && pt.z <= zmax;
}

TMPL_PREFIX void Rect3<T>::expandBy(const Point3<T>& pt)
{
    xmin = std::min<T>(xmin, pt.x);
    ymin = std::min<T>(ymin, pt.y);
    zmin = std::min<T>(zmin, pt.z);
    xmax = std::max<T>(xmax, pt.x);
    ymax = std::max<T>(ymax, pt.y);
    zmax = std::max<T>(zmax, pt.z);
}

TMPL_PREFIX void Rect3<T>::expandBy(const Rect3<T>& rect)
{
    xmin = std::min<T>(xmin, rect.xmin);
    ymin = std::min<T>(ymin, rect.ymin);
    zmin = std::min<T>(zmin, rect.zmin);
    xmax = std::max<T>(xmax, rect.xmax);
    ymax = std::max<T>(ymax, rect.ymax);
    zmax = std::max<T>(zmax, rect.zmax);
}

TMPL_PREFIX bool Rect3<T>::intersectWith(const Rect3<T>& rect, Rect3<T>& out) const
{
    auto ix0 = std::max<T>(xmin, rect.xmin);
    auto iy0 = std::max<T>(ymin, rect.ymin);
    auto iz0 = std::max<T>(zmin, rect.zmin);
    auto ix1 = std::min<T>(xmax, rect.xmax);
    auto iy1 = std::min<T>(ymax, rect.ymax);
    auto iz1 = std::min<T>(zmax, rect.zmax);

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
