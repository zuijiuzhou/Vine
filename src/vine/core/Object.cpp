
#include <vine/core/Object.h>

#include <atomic>
#include <stddef.h>

#include <vine/core/Class.h>
#include <vine/core/String.h>

VI_CORE_NS_BEGIN

struct Object::Data {
    std::atomic<size_t> num_refs = 0;
};

Object::Object() noexcept
  : d(new Data()) {}

Object::Object(const Object& other) noexcept
  : d(new Data()) {}

Object::Object(Object&& other) noexcept
  : d(new Data()) {}

Object::~Object() noexcept { delete d; }

const Class* Object::getClass() const noexcept { return desc(); }

bool Object::isKindOf(const Class* type) const { return getClass()->isSubclassOf(type); }

void Object::addRef() { d->num_refs.fetch_add(1, std::memory_order_relaxed); }

void Object::removeRef(bool del) {
    d->num_refs.fetch_sub(1, std::memory_order_seq_cst);
    if (!d->num_refs && del)
        delete this;
}

size_t Object::getRefs() const noexcept { return d->num_refs.load(); }

bool Object::equals(const Object& other) const noexcept { return this == &other; }

String Object::toString() const { return getClass()->fullName(); }

Object& Object::operator=(const Object& right) noexcept { return *this; }

Object& Object::operator=(Object&& right) noexcept { return *this; }

const Class* Object::desc() {
    static Class* cls = new Class(nullptr, typeid(Object));
    return cls;
}

VI_CORE_NS_END