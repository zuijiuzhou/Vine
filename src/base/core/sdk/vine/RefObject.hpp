#pragma once
#include "core_global.hpp"

#include <atomic>

#include "Object.hpp"
#include "Ptr.hpp"

VI_CORE_NS_BEGIN

VI_DECLARE_PIMPL(RefObject)

template <typename T>
concept RefObjectBased = std::is_base_of<RefObject, T>::value;

class VI_CORE_API RefObject : public Object {
    VI_OBJECT_META_DECL
    VI_DECLARE_PRIVATE(RefObject)
    VI_DECLARE_DPTR(RefObject)
    VI_DISABLE_COPY_MOVE(RefObject)

  public:
    RefObject() noexcept;
    virtual ~RefObject() noexcept;

  protected:
    explicit RefObject(RefObjectPrivate* dptr) noexcept;

  public:
    /**
     * @brief ref count +1
     */
    void strong_ref();
    /**
     * @brief ref count -1, if ref count becomes 0, the object will be deleted, the control block will be deleted when weak ref count becomes 0
     */
    void strong_unref();

    /**
     * @brief weak ref count +1
     */
    void weak_ref();

    /**
     * @brief weak ref count -1, if weak ref count becomes 0 and strong ref count is 0, the control block will be deleted
     */
    void weak_unref();

  private:
    struct ControlBlock {
        std::atomic<unsigned int> strong_refs{ 0 };
        std::atomic<unsigned int> weak_refs{ 0 };
    };

    ControlBlock* const cb_;
};

class VI_CORE_API RefObjectPrivate {
    VI_DECLARE_PUBLIC(RefObject)
    VI_DECLARE_VPTR(RefObject)

  protected:
    RefObjectPrivate()
      : v_ptr(nullptr)
    {}
};

using RefObjectPtr = RefPtr<RefObject>;

VI_CORE_NS_END
