#pragma once
#include "core_global.hpp"

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <new>
#include <utility>

V_CORE_NS_BEGIN

template <typename T, std::size_t InlineCapacity>
class SmallVector {
  public:
    using value_type      = T;
    using size_type       = std::size_t;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;

    using iterator       = T*;
    using const_iterator = const T*;


  public:
    SmallVector() noexcept
      : data_(inline_ptr())
      , size_(0)
      , capacity_(InlineCapacity)
    {}

    explicit SmallVector(size_type count)
      : SmallVector()
    {
        resize(count);
    }

    SmallVector(std::initializer_list<T> init)
      : SmallVector()
    {
        reserve(init.size());

        for (const auto& v : init) {
            push_back(v);
        }
    }

    SmallVector(const SmallVector& other)
      : SmallVector()
    {
        reserve(other.size_);

        for (const auto& v : other) {
            push_back(v);
        }
    }

    SmallVector(SmallVector&& other) noexcept
      : data_(inline_ptr())
      , size_(0)
      , capacity_(InlineCapacity)
    {
        move_from(std::move(other));
    }

    ~SmallVector()
    {
        clear();

        if (using_heap()) {
            ::operator delete(data_);
        }
    }


  public:
    SmallVector& operator=(const SmallVector& other)
    {
        if (this != &other) {
            clear();

            reserve(other.size_);

            for (const auto& v : other) {
                push_back(v);
            }
        }

        return *this;
    }

    SmallVector& operator=(SmallVector&& other) noexcept
    {
        if (this != &other) {
            clear();

            if (using_heap()) {
                ::operator delete(data_);
            }

            data_     = inline_ptr();
            size_     = 0;
            capacity_ = InlineCapacity;

            move_from(std::move(other));
        }

        return *this;
    }


  public:
    size_type size() const noexcept
    {
        return size_;
    }

    size_type capacity() const noexcept
    {
        return capacity_;
    }

    bool empty() const noexcept
    {
        return size_ == 0;
    }


  public:
    reference operator[](size_type i)
    {
        return data_[i];
    }

    const_reference operator[](size_type i) const
    {
        return data_[i];
    }

    reference front()
    {
        return data_[0];
    }

    const_reference front() const
    {
        return data_[0];
    }

    reference back()
    {
        return data_[size_ - 1];
    }

    const_reference back() const
    {
        return data_[size_ - 1];
    }

    pointer data() noexcept
    {
        return data_;
    }

    const_pointer data() const noexcept
    {
        return data_;
    }


  public:
    iterator begin() noexcept
    {
        return data_;
    }

    const_iterator begin() const noexcept
    {
        return data_;
    }

    iterator end() noexcept
    {
        return data_ + size_;
    }

    const_iterator end() const noexcept
    {
        return data_ + size_;
    }


  public:
    void push_back(const T& value)
    {
        ensure_capacity();

        // new (data_ + size_) T(value);
        std::construct_at(data_ + size_, value);

        ++size_;
    }

    void push_back(T&& value)
    {
        ensure_capacity();

        // new (data_ + size_) T(std::move(value));
        std::construct_at(data_ + size_, std::move(value));

        ++size_;
    }

    template <typename... Args>
    reference emplace_back(Args&&... args)
    {
        ensure_capacity();

        // T* ptr = new (data_ + size_) T(std::forward<Args>(args)...);
        T* ptr = std::construct_at(data_ + size_, std::forward<Args>(args)...);

        ++size_;

        return *ptr;
    }

    void pop_back()
    {
        if (size_ > 0) {
            --size_;

            // data_[size_].~T();
            std::destroy_at(data_ + size_);
        }
    }

    void reserve(size_type new_capacity)
    {
        if (new_capacity <= capacity_)
            return;


        T* new_data = static_cast<T*>(::operator new(sizeof(T) * new_capacity));

        // for (size_type i = 0; i < size_; ++i) {
        //     new (new_data + i) T(std::move(data_[i]));
        //     data_[i].~T();
        // }

        try {
            std::uninitialized_move_n(data_, size_, new_data);
        }
        catch (...) {
            ::operator delete(new_data);
            throw;
        }

        std::destroy_n(data_, size_);

        if (using_heap()) {
            ::operator delete(data_);
        }


        data_     = new_data;
        capacity_ = new_capacity;
    }


    void resize(size_type count)
    {
        if (count > size_) {
            reserve(count);

            while (size_ < count) {
                emplace_back();
            }
        }
        else {
            while (size_ > count) {
                pop_back();
            }
        }
    }
    
    void clear() noexcept
    {
        // for (size_type i = 0; i < size_; ++i) {
        //     data_[i].~T();
        // }
        // size_ = 0;

        std::destroy(std::make_reverse_iterator(data_ + size_),
                     std::make_reverse_iterator(data_));
        size_ = 0;
    }


  private:
    bool using_heap() const noexcept
    {
        return data_ != inline_ptr();
    }

    void ensure_capacity()
    {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
    }

    T* inline_ptr() noexcept
    {
        return reinterpret_cast<T*>(buffer_);
    }

    const T* inline_ptr() const noexcept
    {
        return reinterpret_cast<const T*>(buffer_);
    }

    void move_from(SmallVector&& other)
    {
        if (other.using_heap()) {
            data_     = other.data_;
            size_     = other.size_;
            capacity_ = other.capacity_;

            other.data_     = other.inline_ptr();
            other.size_     = 0;
            other.capacity_ = InlineCapacity;
        }
        else {
            reserve(other.size_);

            // for (auto& v : other) {
            //     push_back(std::move(v));
            // }
            // other.clear();

            std::uninitialized_move_n(other.data_, other.size_, data_);
            size_ = other.size_;

            std::destroy_n(other.data_, other.size_);
            other.size_ = 0;
        }
    }


  private:
    alignas(T) std::byte buffer_[sizeof(T) * InlineCapacity];

    T* data_;

    size_type size_;

    size_type capacity_;
};

V_CORE_NS_END
