#pragma once

#include "core_global.hpp"

#include <atomic>
#include "RefObject.hpp"

V_CORE_NS_BEGIN

template <typename T>
class RefPtr {
  private:
    template <typename TOther>
    friend class RefPtr;

  public:
        /**
         * Construct from raw pointer without modifying the strong refcount.
         * This is intended for internal use by `WRefPtr::lock()` after it
         * atomically incremented the strong count.
         */
        RefPtr(T* ptr, bool addRef)
            : ptr_(ptr)
        {
            if (ptr_ && addRef) {
                static_assert(std::is_base_of_v<RefObject, T>, "RefPtr requires RefObject-based T");
                auto* cb = static_cast<RefObject*>(ptr_)->cb_;
                if (cb)
                    cb->strong_refs.fetch_add(1, std::memory_order_relaxed);
            }
        }

    RefPtr()
      : ptr_(nullptr)
    {}

    RefPtr(T* ptr)
      : ptr_(ptr)
    {
        if (ptr_) {
            static_assert(std::is_base_of_v<RefObject, T>, "RefPtr requires RefObject-based T");
            auto* cb = static_cast<RefObject*>(ptr_)->cb_;
            if (cb)
                cb->strong_refs.fetch_add(1, std::memory_order_relaxed);
        }
    }

    RefPtr(const RefPtr& other)
      : ptr_(other.ptr_)
    {
        if (ptr_) {
            static_assert(std::is_base_of_v<RefObject, T>, "RefPtr requires RefObject-based T");
            auto* cb = static_cast<RefObject*>(ptr_)->cb_;
            if (cb)
                cb->strong_refs.fetch_add(1, std::memory_order_relaxed);
        }
    }

    template <typename TOther>
    RefPtr(const RefPtr<TOther>& other)
      : ptr_(other.ptr_)
    {
        if (ptr_) {
            static_assert(std::is_base_of_v<RefObject, T>, "RefPtr requires RefObject-based T");
            auto* cb = static_cast<RefObject*>(ptr_)->cb_;
            if (cb)
                cb->strong_refs.fetch_add(1, std::memory_order_relaxed);
        }
    }

    ~RefPtr()
    {
        if (ptr_) {
            static_assert(std::is_base_of_v<RefObject, T>, "RefPtr requires RefObject-based T");
            auto* cb = static_cast<RefObject*>(ptr_)->cb_;
            if (cb) {
                if (cb->strong_refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                    std::atomic_thread_fence(std::memory_order_acquire);
                    delete ptr_;
                }
            } else {
                delete ptr_;
            }
        }
    }

  public:
    T* get() const
    {
        return ptr_;
    }

    void set(T* ptr)
    {
        if (ptr == ptr_)
            return;
        if (ptr_) {
            static_assert(std::is_base_of_v<RefObject, T>, "RefPtr requires RefObject-based T");
            auto* cb = static_cast<RefObject*>(ptr_)->cb_;
            if (cb) {
                if (cb->strong_refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                    std::atomic_thread_fence(std::memory_order_acquire);
                    delete ptr_;
                }
            } else {
                delete ptr_;
            }
        }
        ptr_ = ptr;
        if (ptr_) {
            static_assert(std::is_base_of_v<RefObject, T>, "RefPtr requires RefObject-based T");
            auto* cb = static_cast<RefObject*>(ptr_)->cb_;
            if (cb)
                cb->strong_refs.fetch_add(1, std::memory_order_relaxed);
        }
    }

    T* release()
    {
        auto temp = ptr_;
        if (ptr_) {
            static_assert(std::is_base_of_v<RefObject, T>, "RefPtr requires RefObject-based T");
            auto* cb = static_cast<RefObject*>(ptr_)->cb_;
            if (cb) {
                cb->strong_refs.fetch_sub(1, std::memory_order_acq_rel);
            }
        }
        ptr_ = nullptr;
        return temp;
    }

    void swap(RefPtr& other)
    {
        std::swap(ptr_, other.ptr_);
    }

  public:
    bool operator!() const
    {
        return !ptr_;
    }

    T* operator->() const
    {
        return ptr_;
    }

    T& operator*() const
    {
        return *ptr_;
    }

    RefPtr& operator=(const RefPtr& right)
    {
        set(right.ptr_);
        return *this;
    }

    template <typename TOther>
    RefPtr& operator=(const RefPtr<TOther>& right)
    {
        set(right.ptr_);
        return *this;
    }

    template <typename TOther>
    RefPtr& operator=(TOther* ptr)
    {
        set(ptr);
        return *this;
    }

    bool operator==(const RefPtr& right) const
    {
        return ptr_ == right.ptr_;
    }

    bool operator==(const T* right) const
    {
        return ptr_ == right;
    }

    friend bool operator==(const T* left, const RefPtr& right)
    {
        return left == right.ptr_;
    }

    bool operator!=(const RefPtr& right) const
    {
        return ptr_ != right.ptr_;
    }

    bool operator!=(const T* right) const
    {
        return ptr_ != right;
    }

    friend bool operator!=(const T* left, const RefPtr& right)
    {
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

    bool hasValue() const
    {
        return ptr_ != nullptr;
    }

  private:
    T* ptr_;
};

template <typename T>
class WRefPtr {
  private:
    template <typename TOther>
    friend class WRefPtr;

  public:
    WRefPtr()
      : ptr_(nullptr)
      , cb_(nullptr)
    {}

    explicit WRefPtr(T* ptr)
      : ptr_(ptr)
      , cb_(ptr ? static_cast<RefObject*>(ptr)->cb_ : nullptr)
    {
        if (cb_)
            cb_->weak_refs.fetch_add(1, std::memory_order_relaxed);
    }

    WRefPtr(const RefPtr<T>& rp)
      : ptr_(rp.get())
      , cb_(ptr_ ? static_cast<RefObject*>(ptr_)->cb_ : nullptr)
    {
        if (cb_)
            cb_->weak_refs.fetch_add(1, std::memory_order_relaxed);
    }

    WRefPtr(const WRefPtr& other)
      : ptr_(other.ptr_)
      , cb_(other.cb_)
    {
        if (cb_)
            cb_->weak_refs.fetch_add(1, std::memory_order_relaxed);
    }

    ~WRefPtr()
    {
        if (cb_) {
            if (cb_->weak_refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                if (cb_->strong_refs.load(std::memory_order_acquire) == 0) {
                    std::atomic_thread_fence(std::memory_order_acquire);
                    delete cb_;
                }
            }
        }
    }

    WRefPtr& operator=(const WRefPtr& right)
    {
        if (this == &right)
            return *this;
        if (cb_) {
            if (cb_->weak_refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                if (cb_->strong_refs.load(std::memory_order_acquire) == 0) {
                    std::atomic_thread_fence(std::memory_order_acquire);
                    delete cb_;
                }
            }
        }
        ptr_ = right.ptr_;
        cb_ = right.cb_;
        if (cb_)
            cb_->weak_refs.fetch_add(1, std::memory_order_relaxed);
        return *this;
    }

    RefPtr<T> lock() const
    {
        if (!cb_ || !ptr_)
            return RefPtr<T>();

        unsigned int s = cb_->strong_refs.load(std::memory_order_acquire);
        while (s != 0) {
            if (cb_->strong_refs.compare_exchange_weak(
                    s, s + 1,
                    std::memory_order_acq_rel,
                    std::memory_order_acquire)) {
                return RefPtr<T>(ptr_, false);
            }
        }
        return RefPtr<T>();
    }

    private:
        T* ptr_;
        RefObject::PtrControlBlock* cb_;
};

template <typename T, typename Y>
inline RefPtr<T> static_pointer_cast(const RefPtr<Y>& rp)
{
    return static_cast<T*>(rp.get());
}

template <typename T, typename Y>
inline RefPtr<T> dynamic_pointer_cast(const RefPtr<Y>& rp)
{
    return dynamic_cast<T*>(rp.get());
}

template <typename T, typename Y>
inline RefPtr<T> const_pointer_cast(const RefPtr<Y>& rp)
{
    return const_cast<T*>(rp.get());
}

V_CORE_NS_END

#define V_PTR(ClassName) RefPtr<ClassName>