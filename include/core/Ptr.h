#pragma once

#include "core_global.h"

#include <atomic>
#include <memory>
#include <type_traits>

VINE_CORE_NS_BEGIN

class Object;
template <typename T>
    requires std::is_base_of<Object, T>::value
class RefPtr
{
public:
    RefPtr() : ptr_(nullptr)
    {
    }
    RefPtr(T *ptr) : ptr_(ptr)
    {
        if (ptr_)
        {
            ptr_->addRef();
        }
    }
    RefPtr(const RefPtr &other) : ptr_(other.ptr_)
    {
        if (ptr_)
        {
            ptr_->addRef();
        }
    }
    ~RefPtr()
    {
        if (ptr_)
        {
            ptr_->removeRef();
        }
    }

public:
    T *get() const
    {
        return ptr_;
    }

    void set(T *ptr)
    {
        if (ptr == ptr_)
            return;
        if (ptr_)
            ptr_->removeRef();
        ptr_ = ptr;
        if (ptr_)
            ptr_->addRef();
    }

    T *release()
    {
        auto temp = ptr_;
        if (ptr_)
            ptr_->removeRef(false);
        ptr_ = nullptr;
        return temp;
    }

    void swap(RefPtr &other)
    {
        std::swap(ptr_, other.ptr_);
    }

public:
    bool operator!()
    {
        return !!ptr_;
    }

    T *operator->()
    {
        return ptr_;
    }

    T &operator*()
    {
        return *ptr_;
    }

    RefPtr &operator=(const RefPtr &right)
    {
        set(right.ptr_);
        return *this;
    }

    template <typename TOther>
        requires std::is_base_of<T, TOther>::value
    RefPtr &operator=(const RefPtr<TOther> &right)
    {
        set(right.ptr_);
        return *this;
    }

    template <typename TOther>
        requires std::is_base_of<T, TOther>::value
    RefPtr &operator=(TOther* ptr)
    {
        set(ptr);
        return *this;
    }

    bool operator==(const RefPtr &right)
    {
        return ptr_ == right.ptr_;
    }

    bool operator==(const T *right)
    {
        return ptr_ == right;
    }

    bool operator!=(const RefPtr &right)
    {
        return !(*this == right);
    }

    bool operator!=(const T *right)
    {
        return ptr_ != right;
    }

private:
    template <typename TOther>
        requires std::is_base_of<Object, TOther>::value
    friend class RefPtr;

private:
    T *ptr_;
};

VINE_CORE_NS_END
