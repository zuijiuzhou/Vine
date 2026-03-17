#include <vine/math/Vector4.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <type_traits>

#include <vine/math/Math.hpp>
#include <vine/math/Vector3.hpp>

V_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

#undef TMPL_PREFIX

template class V_MATH_API Vector4<float>;
template class V_MATH_API Vector4<double>;
template class V_MATH_API Vector4<bool>;
template class V_MATH_API Vector4<int8_t>;
template class V_MATH_API Vector4<uint8_t>;
template class V_MATH_API Vector4<int16_t>;
template class V_MATH_API Vector4<uint16_t>;
template class V_MATH_API Vector4<int32_t>;
template class V_MATH_API Vector4<uint32_t>;
template class V_MATH_API Vector4<int64_t>;
template class V_MATH_API Vector4<uint64_t>;

V_MATH_NS_END
