#pragma once

#include "core_global.hpp"

#include <type_traits>

VI_CORE_NS_BEGIN

class Object;

template <typename T>
    requires std::is_base_of<Object, T>::value
class RefPtr
{
private:
    template <typename TOther>
        requires std::is_base_of<Object, TOther>::value
    friend class RefPtr;

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

    template <typename TOther>
        requires std::is_base_of<Object, TOther>::value
    RefPtr(const RefPtr<TOther> &other) : ptr_(other.ptr_)
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
    bool operator!() const
    {
        return !ptr_;
    }

    T *operator->() const
    {
        return ptr_;
    }

    T &operator*() const
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
    RefPtr &operator=(TOther *ptr)
    {
        set(ptr);
        return *this;
    }

    bool operator==(const RefPtr &right) const
    {
        return ptr_ == right.ptr_;
    }

    bool operator==(const T *right) const
    {
        return ptr_ == right;
    }

    friend bool operator ==(const T* left, const RefPtr& right) {
        return left == right.ptr_;
    }

    bool operator!=(const RefPtr &right) const
    {
        return ptr_ != right.ptr_;
    }

    bool operator!=(const T *right) const
    {
        return ptr_ != right;
    }

    friend bool operator !=(const T* left, const RefPtr& right) {
        return left != right.ptr_;
    }

    bool operator<(const RefPtr<T>& right) const
    {
        return ptr_ < right.ptr_;
    }

    bool operator<(const T* right) const
    {
        return ptr_ < right;
    }

    bool operator>(const RefPtr<T>& right) const
    {
        return ptr_ > right.ptr_;
    }

    bool operator>(const T* right) const
    {
        return ptr_ > right;
    }

    bool hasValue() const {
        return ptr_ != nullptr;
    }

private:
    T *ptr_;
};

template<typename T, typename Y>
requires std::is_base_of<Object, T>::value && std::is_base_of<Object, Y>::value
inline RefPtr<T> static_pointer_cast(const RefPtr<Y>& rp) { return static_cast<T*>(rp.get()); }

template<class T, class Y>
requires std::is_base_of<Object, T>::value && std::is_base_of<Object, Y>::value
inline RefPtr<T> dynamic_pointer_cast(const RefPtr<Y>& rp) { return dynamic_cast<T*>(rp.get()); }

template<class T, class Y>
requires std::is_base_of<Object, T>::value && std::is_base_of<Object, Y>::value
inline RefPtr<T> const_pointer_cast(const RefPtr<Y>& rp) { return const_cast<T*>(rp.get()); }

VI_CORE_NS_END
