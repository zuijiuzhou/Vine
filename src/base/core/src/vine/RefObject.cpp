#include <vine/RefObject.hpp>

#include <stdexcept>

VI_CORE_NS_BEGIN

VI_OBJECT_META_IMPL(RefObject, Object)

RefObjectPrivate::RefObjectPrivate()
  : v_ptr(nullptr)
{}

RefObject::RefObject()
  : RefObject(new RefObjectPrivate())
{}

RefObject::RefObject(RefObjectPrivate* dptr) noexcept
  : d_ptr(dptr)
  , cb_(new ControlBlock())
{
    d_ptr->v_ptr = this;
}

RefObject::~RefObject() noexcept
{
    delete d_ptr;

    if (cb_->weak_refs.load(std::memory_order_seq_cst) == 0)
        delete cb_;
}

void RefObject::ref()
{
    cb_->strong_refs.fetch_add(1, std::memory_order_relaxed);
}

void RefObject::unref(bool del)
{
    if (cb_->strong_refs.fetch_sub(1, std::memory_order_seq_cst) == 1 && del)
        delete this;
}

void RefObject::weak_ref()
{
    cb_->weak_refs.fetch_add(1, std::memory_order_seq_cst);
}

void RefObject::weak_unref()
{
    if (cb_->weak_refs.fetch_sub(1, std::memory_order_seq_cst) == 1 && cb_->strong_refs.load() == 0) { delete cb_; }
}

VI_CORE_NS_END