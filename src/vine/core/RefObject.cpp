#include <vine/core/RefObject.hpp>

#include <atomic>
#include <stdexcept>

VI_CORE_NS_BEGIN

VI_OBJECT_META_IMPL(RefObject, Object)

struct RefObject::Data {
    std::atomic<unsigned int> strong_refs = 0;
    std::atomic<unsigned int> weak_refs   = 0;
};

RefObject::RefObject() noexcept
  : d(new Data())
{}

RefObject::~RefObject() noexcept
{
    if (d->weak_refs.load(std::memory_order_seq_cst) == 0)
        delete d;
}

void RefObject::ref()
{
    d->strong_refs.fetch_add(1, std::memory_order_relaxed);
}

void RefObject::unref(bool del)
{
    if (d->strong_refs.fetch_sub(1, std::memory_order_seq_cst) == 1 && del)
        delete this;
}

void RefObject::weak_ref()
{
    d->weak_refs.fetch_add(1, std::memory_order_seq_cst);

    if (d->strong_refs.load(std::memory_order_seq_cst) == 0 &&
        d->weak_refs.fetch_sub(1, std::memory_order_seq_cst) == 1)
    {
        delete d;
    }
}

void RefObject::weak_unref()
{
    if (d->weak_refs.fetch_sub(1, std::memory_order_seq_cst) == 1 && d->strong_refs.load() == 0) {
        delete d;
    }
}

VI_CORE_NS_END