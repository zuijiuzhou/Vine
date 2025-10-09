#pragma once
#include "Signal.hpp"
#include "Object.hpp"

VI_CORE_NS_BEGIN

class VI_CORE_API EventArgs : public Object {
    VI_OBJECT_META
  public:
};

template <typename TVal> class PropertyChangedEventArgs : public EventArgs {
    VI_OBJECT_META;

  public:
    // using ValType   = typename std::remove_reference<typename std::remove_const<TVal>::type>::type;
    // using ValTypeCR = typename std::add_lvalue_reference<typename std::add_const<ValType>::type>::type;

    using ValType = TVal;

  public:
    PropertyChangedEventArgs(ValType new_val, ValType old_val);

  public:
    ValType newValue() const { return new_val_; }
    ValType oldValue() const { return old_val_; }

  private:
    ValType new_val_;
    ValType old_val_;
};

template <typename TSender, typename TEventArgs = EventArgs>
    requires std::is_base_of<EventArgs, TEventArgs>::value
using Event = Signal<TSender, TEventArgs>;

VI_CORE_NS_END