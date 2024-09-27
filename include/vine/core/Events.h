#pragma once

#include "Signal.h"

#include "Object.h"

VI_CORE_NS_BEGIN

class VI_CORE_API EventArgs : public Object {
    VI_OBJECT_META
  public:
};

template <typename TVal> class PropertyChangedEventArgs : public EventArgs {
    VI_TMPL_OBJECT_META(PropertyChangedEventArgs<TVal>, EventArgs)

  public:
    using ValType   = typename std::remove_reference<typename std::remove_const<TVal>::type>::type;
    using ValTypeCR = typename std::add_lvalue_reference<typename std::add_const<ValType>::type>::type;

  public:
    PropertyChangedEventArgs(ValTypeCR new_val, ValTypeCR old_val)
      : new_val_(new_val)
      , old_val_(old_val) {}

  public:
    ValTypeCR newValue() const { return new_val_; }
    ValTypeCR oldValue() const { return old_val_; }

  private:
    ValType new_val_;
    ValType old_val_;
};

template <typename TSender, typename TEventArgs = EventArgs>
    requires std::is_base_of<EventArgs, TEventArgs>::value
using Event = Signal<TSender, TEventArgs>;

VI_CORE_NS_END