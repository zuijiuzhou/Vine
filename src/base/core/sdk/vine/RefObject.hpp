#pragma once
#include "core_global.hpp"

#include <atomic>

#include "Object.hpp"
V_CORE_NS_BEGIN

V_DECLARE_PIMPL(RefObject)

template <typename>
class RefPtr;
template <typename>
class WRefPtr;

template <typename T>
concept RefObjectBased = std::is_base_of<RefObject, T>::value;

/*
 * @brief RefObject is a base class for reference-counted objects.
 * It provides strong and weak reference counting mechanisms.
 * The control block is used to manage the reference counts and ensure proper deletion of the object and control block when necessary.
 * @note: RefObject cannot be moved or copied, otherwise it will cause memory leak or dangling pointer
 */
class V_CORE_API RefObject : public Object {
    V_OBJECT_META_DECL
    V_DISABLE_COPY_MOVE(RefObject)
    // V_DECLARE_PRIVATE(RefObject)
    // V_DECLARE_DPTR(RefObject)

  public:
    RefObject() noexcept;
    virtual ~RefObject() noexcept;

    // Control block is private; RefPtr/WRefPtr are friends and may
    // access `cb_` directly. Do not expose it publicly.

  public:
    // Reference management functions moved to Ptr.hpp (RefPtr/WRefPtr).
    // Use RefPtr<T> and WRefPtr<T> to manage strong/weak references.

  private:
  private:
    // Control block holds atomic strong/weak counters. Kept private
    // inside RefObject; RefPtr/WRefPtr are declared friends so they
    // can manage the counters without exposing them publicly.
    struct PtrControlBlock {
        std::atomic<unsigned int> strong_refs{ 0 };
        std::atomic<unsigned int> weak_refs{ 0 };
    };

    PtrControlBlock* cb_;

    template <typename U>
    friend class RefPtr;
    template <typename U>
    friend class WRefPtr;
};

// class V_CORE_API RefObjectPrivate {
//     V_DECLARE_PUBLIC(RefObject)
//     V_DECLARE_VPTR(RefObject)

//   protected:
//     RefObjectPrivate()
//       : v_ptr(nullptr)
//     {}
// };

using RefObjectPtr = RefPtr<RefObject>;

V_CORE_NS_END
