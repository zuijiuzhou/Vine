#pragma once

#include "core_global.hpp"

#include "Object.hpp"
#include "Ptr.hpp"

VI_CORE_NS_BEGIN

class VI_CORE_API RefObject : public Object {
    VI_OBJECT_META;

  public:
    RefObject() noexcept;
    RefObject(const RefObject& other) noexcept = delete;
    RefObject(RefObject&& other) noexcept      = delete;
    virtual ~RefObject() noexcept;

  public:
    /**
     * @brief 
     */
    void ref();
    void unref(bool del = true);

    void weak_ref();
    void weak_unref();

  private:
    struct Data;
    Data* const d;
};

using RefObjectPtr = RefPtr<RefObject>;

VI_CORE_NS_END