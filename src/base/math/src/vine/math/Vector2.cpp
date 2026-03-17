
#include <vine/math/Vector2.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <type_traits>

#include <vine/math/Math.hpp>
#include <vine/math/Point2.hpp>

V_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Point2<T> Vector2<T>::toPoint() const
{
    return Point2<T>(x, y);
}

#undef TMPL_PREFIX

template class V_MATH_API Vector2<float>;
template class V_MATH_API Vector2<double>;
template class V_MATH_API Vector2<bool>;
template class V_MATH_API Vector2<int8_t>;
template class V_MATH_API Vector2<uint8_t>;
template class V_MATH_API Vector2<int16_t>;
template class V_MATH_API Vector2<uint16_t>;
template class V_MATH_API Vector2<int32_t>;
template class V_MATH_API Vector2<uint32_t>;
template class V_MATH_API Vector2<int64_t>;
template class V_MATH_API Vector2<uint64_t>;

V_MATH_NS_END
