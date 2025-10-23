
#include <vine/core/RefObject.hpp>

VI_CORE_NS_BEGIN

VI_OBJECT_META_IMPL(RefObject, Object)

struct RefObject::Data {
    std::atomic<unsigned int> strong_refs;
    std::atomic<unsigned int> weak_refs;
};

RefObject::RefObject() noexcept
  : d(new Data())
{}

RefObject::RefObject(const RefObject& other) noexcept
  : d(other.d)
{}

RefObject::RefObject(RefObject&& other) noexcept
  : d(new Data())
{}

RefObject::~RefObject() noexcept
{
    if (d->weak_refs.load() == 0)
        delete d;
}

void RefObject::ref()
{
    d->strong_refs.fetch_add(1, std::memory_order_relaxed);
}

void RefObject::unref(bool del)
{
    d->strong_refs.fetch_sub(1, std::memory_order_seq_cst);
    if (!d->strong_refs && del)
        delete this;
}

unsigned int RefObject::nbRefs() const noexcept
{
    return d->strong_refs.load();
}

VI_CORE_NS_END