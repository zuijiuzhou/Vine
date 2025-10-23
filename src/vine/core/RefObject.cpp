
#include <vine/core/RefObject.hpp>

#include <atomic>
#include <cstddef>

VI_CORE_NS_BEGIN

VI_OBJECT_META_IMPL(RefObject, Object)

struct RefObject::Data {
    std::atomic<int32_t> num_refs = 0;
};

RefObject::RefObject() noexcept
  : d(new Data())
{}

RefObject::RefObject(const RefObject& other) noexcept
  : d(new Data())
{}

RefObject::RefObject(RefObject&& other) noexcept
  : d(new Data())
{}

RefObject::~RefObject() noexcept
{
    delete d;
}

void RefObject::addRef()
{
    d->num_refs.fetch_add(1, std::memory_order_relaxed);
}

void RefObject::removeRef(bool del)
{
    d->num_refs.fetch_sub(1, std::memory_order_seq_cst);
    if (!d->num_refs && del)
        delete this;
}

size_t RefObject::getRefs() const noexcept
{
    return d->num_refs.load();
}

VI_CORE_NS_END