#include <vine/core/Events.hpp>

VI_CORE_NS_BEGIN

VI_TMPL_OBJECT_META_IMPL(template <typename TVal>, PropertyChangedEventArgs<TVal>, EventArgs);

template <typename TVal>
PropertyChangedEventArgs<TVal>::PropertyChangedEventArgs(ValType new_val, ValType old_val)
  : new_val_(new_val)
  , old_val_(old_val) {
}

VI_CORE_NS_END