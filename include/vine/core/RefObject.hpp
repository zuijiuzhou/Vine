#pragma once
#include "core_global.hpp"

#include <atomic>
#include <cstddef>

#include "Object.hpp"
#include "Ptr.hpp"

VI_CORE_NS_BEGIN

class VI_CORE_API RefObject : public Object {
    VI_OBJECT_META;

  public:
    RefObject() noexcept;
    RefObject(const RefObject& other) noexcept;
    RefObject(RefObject&& other) noexcept;
    virtual ~RefObject() noexcept;

  public:
    void addRef();

    void removeRef(bool del = true);

    size_t getRefs() const noexcept;

  private:
    struct Data;
    Data* d;
};

using RefObjectPtr = RefPtr<RefObject>;

VI_CORE_NS_END