#pragma once

#include <vine/core_global.hpp>

#include <atomic>

#include <vine/Object.hpp>

V_CORE_NS_BEGIN

template <typename>
class SPtr;
template <typename>
class WPtr;

class RefObject;

template <typename T>
concept RefObjectBased = std::is_base_of<RefObject, T>::value;

/**
 * @brief Base class for reference-counted objects with a separate control
 *        block (strong + weak counters).
 *
 * @note RefObject cannot be moved or copied, otherwise it will cause memory
 *       leak or dangling pointer.
 * @deprecated Use RefCounted<Derived> together with IntrusivePtr<Derived>
 *             instead; no base class is required and no control block is
 *             allocated per object.
 */
class [[deprecated]] V_CORE_API RefObject : public Object {
    V_OBJECT_META_DECL
    V_DISABLE_COPY_MOVE(RefObject)
    // V_DECLARE_PRIVATE(RefObject)
    // V_DECLARE_DPTR(RefObject)

  public:
    RefObject() noexcept;
    virtual ~RefObject() noexcept;

    // Control block is private; SPtr/WPtr are friends and may
    // access `cb_` directly. Do not expose it publicly.

  public:
    // Reference management functions moved to Ptr.hpp (SPtr/WPtr).
    // Use SPtr<T> and WPtr<T> to manage strong/weak references.

  private:
  private:
    // Control block holds atomic strong/weak counters. Kept private
    // inside RefObject; SPtr/WPtr are declared friends so they
    // can manage the counters without exposing them publicly.
    struct PtrControlBlock {
        std::atomic<unsigned int> strong_refs{ 0 };
        std::atomic<unsigned int> weak_refs{ 0 };
    };

    PtrControlBlock* cb_;

    template <typename U>
    friend class SPtr;
    template <typename U>
    friend class WPtr;
};

// class V_CORE_API RefObjectPrivate {
//     V_DECLARE_PUBLIC(RefObject)
//     V_DECLARE_VPTR(RefObject)

//   protected:
//     RefObjectPrivate()
//       : v_ptr(nullptr)
//     {}
// };

V_CORE_NS_END
