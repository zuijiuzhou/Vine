#pragma once
#include "core_global.hpp"

#include <atomic>

#include "Object.hpp"
#include "Ptr.hpp"

VI_CORE_NS_BEGIN

VI_DECLARE_PIMPL_CLASS(RefObject)

class VI_CORE_API RefObject : public Object {
    VI_OBJECT_META
    VI_DECLARE_PRIVATE(RefObject)
    VI_DECLARE_DPTR(RefObject)
    VI_DISABLE_COPY_MOVE(RefObject)


  public:
    RefObject();
    virtual ~RefObject() noexcept;

  protected:
    explicit RefObject(RefObjectPrivate* dptr) noexcept;

  public:
    /**
     * @brief
     */
    void ref();
    void unref(bool del = true);

    void weak_ref();
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
    RefObjectPrivate();
};

using RefObjectPtr = RefPtr<RefObject>;

VI_CORE_NS_END