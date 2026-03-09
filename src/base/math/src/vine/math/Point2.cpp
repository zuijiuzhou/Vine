#include <vine/math/Point2.hpp>

#include <cassert>
#include <cmath>
#include <cstdint>

#include <vine/math/Math.hpp>
#include <vine/math/Vector2.hpp>

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Vector2<T> Point2<T>::toVector() const
{
    return Vector2<T>(x, y);
}

#undef TMPL_PREFIX

template class VI_MATH_API Point2<float>;
template class VI_MATH_API Point2<double>;
template class VI_MATH_API Point2<bool>;
template class VI_MATH_API Point2<int8_t>;
template class VI_MATH_API Point2<uint8_t>;
template class VI_MATH_API Point2<int16_t>;
template class VI_MATH_API Point2<uint16_t>;
template class VI_MATH_API Point2<int32_t>;
template class VI_MATH_API Point2<uint32_t>;
template class VI_MATH_API Point2<int64_t>;
template class VI_MATH_API Point2<uint64_t>;

VI_MATH_NS_END
