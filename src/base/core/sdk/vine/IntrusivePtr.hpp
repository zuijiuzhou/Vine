#pragma once

#include "core_global.hpp"

#include <cstddef>
#include <type_traits>
#include <utility>

V_CORE_NS_BEGIN

/**
 * @brief Intrusive strong reference pointer (boost::IntrusivePtr style).
 *
 * IntrusivePtr<T> is a single owning pointer managing T through the ADL free functions
 * intrusive_ptr_add_ref / intrusive_ptr_release. T does not need a common base
 * class: either provide those functions yourself, or derive T from
 * RefCounted<T>. There is no separate control block; the counter lives inside
 * T, so construction is allocation-free and non-throwing.
 *
 * @tparam T The referenced type; must be complete at instantiation and expose
 *         intrusive_ptr_add_ref(T*) / intrusive_ptr_release(T*) via ADL.
 */
template <typename T>
class IntrusivePtr {
  private:
    template <typename U>
    friend class IntrusivePtr;

    T* px_{ nullptr };

    void addRef() const
    {
        if (px_)
            intrusive_ptr_add_ref(px_);
    }

    void release() noexcept
    {
        if (px_)
            intrusive_ptr_release(px_);
    }

  public:
    using element_type = T;

    /**
     * @brief Constructs a null pointer.
     */
    IntrusivePtr() noexcept = default;

    /**
     * @brief Takes ownership of p and adds a reference.
     *
     * @param p Raw pointer to adopt, or nullptr.
     */
    explicit IntrusivePtr(T* p)
      : px_(p)
    {
        addRef();
    }

    /**
     * @brief Adopts p, optionally without adding a reference.
     *
     * @param p Raw pointer to adopt, or nullptr.
     * @param add_ref false to adopt an already-held reference.
     */
    IntrusivePtr(T* p, bool add_ref)
      : px_(p)
    {
        if (add_ref)
            this->addRef();
    }

    /**
     * @brief Copy constructor; increments the reference count.
     */
    IntrusivePtr(const IntrusivePtr& other)
      : px_(other.px_)
    {
        addRef();
    }

    /**
     * @brief Converting constructor from IntrusivePtr<U>.
     *
     * @tparam U Source element type convertible to T.
     */
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    IntrusivePtr(const IntrusivePtr<U>& other)
      : px_(other.px_)
    {
        addRef();
    }

    /**
     * @brief Releases the reference.
     */
    ~IntrusivePtr()
    {
        release();
    }

    /**
     * @brief Copy assignment; releases the old reference and takes a new one.
     *
     * @return *this.
     */
    IntrusivePtr& operator=(const IntrusivePtr& other)
    {
        IntrusivePtr(other).swap(*this);
        return *this;
    }

    /**
     * @brief Converting assignment from IntrusivePtr<U>.
     *
     * @return *this.
     */
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    IntrusivePtr& operator=(const IntrusivePtr<U>& other)
    {
        IntrusivePtr(other).swap(*this);
        return *this;
    }

    /**
     * @brief Assigns a raw pointer, taking ownership.
     *
     * @return *this.
     */
    IntrusivePtr& operator=(T* p)
    {
        IntrusivePtr(p).swap(*this);
        return *this;
    }

    /**
     * @brief Releases the reference and becomes null.
     */
    void reset()
    {
        IntrusivePtr().swap(*this);
    }

    /**
     * @brief Replaces the managed pointer with p.
     *
     * @param p Raw pointer to adopt, or nullptr.
     */
    void reset(T* p)
    {
        IntrusivePtr(p).swap(*this);
    }

    /**
     * @brief Swaps with another pointer.
     */
    void swap(IntrusivePtr& other) noexcept
    {
        std::swap(px_, other.px_);
    }

    /**
     * @brief Returns the raw pointer.
     *
     * @return The managed pointer, or nullptr.
     */
    T* get() const noexcept
    {
        return px_;
    }

    /**
     * @brief Dereferences the managed object.
     *
     * @return A reference to the managed object.
     */
    T& operator*() const
    {
        return *px_;
    }

    /**
     * @brief Provides member access to the managed object.
     *
     * @return The raw pointer.
     */
    T* operator->() const noexcept
    {
        return px_;
    }

    /**
     * @brief Checks whether the pointer is non-null.
     */
    explicit operator bool() const noexcept
    {
        return px_ != nullptr;
    }

    /**
     * @brief Checks whether the pointer is non-null.
     *
     * @return true when non-null.
     */
    bool hasValue() const noexcept
    {
        return px_ != nullptr;
    }

    friend bool operator==(const IntrusivePtr& a, const IntrusivePtr& b) noexcept
    {
        return a.px_ == b.px_;
    }

    friend bool operator!=(const IntrusivePtr& a, const IntrusivePtr& b) noexcept
    {
        return a.px_ != b.px_;
    }

    friend bool operator==(const IntrusivePtr& a, T* b) noexcept
    {
        return a.px_ == b;
    }

    friend bool operator==(T* a, const IntrusivePtr& b) noexcept
    {
        return a == b.px_;
    }

    friend bool operator!=(const IntrusivePtr& a, T* b) noexcept
    {
        return a.px_ != b;
    }

    friend bool operator!=(T* a, const IntrusivePtr& b) noexcept
    {
        return a != b.px_;
    }

    friend bool operator==(const IntrusivePtr& a, std::nullptr_t) noexcept
    {
        return a.px_ == nullptr;
    }

    friend bool operator==(std::nullptr_t, const IntrusivePtr& a) noexcept
    {
        return a.px_ == nullptr;
    }

    friend bool operator!=(const IntrusivePtr& a, std::nullptr_t) noexcept
    {
        return a.px_ != nullptr;
    }

    friend bool operator!=(std::nullptr_t, const IntrusivePtr& a) noexcept
    {
        return a.px_ != nullptr;
    }
};

/**
 * @brief Performs a static pointer cast.
 *
 * @tparam T Target element type.
 * @param r Source pointer.
 * @return A new owning pointer to the same object.
 */
template <typename T, typename U>
inline IntrusivePtr<T> static_pointer_cast(const IntrusivePtr<U>& r)
{
    return IntrusivePtr<T>(static_cast<T*>(r.get()));
}

/**
 * @brief Performs a const pointer cast.
 *
 * @tparam T Target element type.
 * @param r Source pointer.
 * @return A new owning pointer to the same object.
 */
template <typename T, typename U>
inline IntrusivePtr<T> const_pointer_cast(const IntrusivePtr<U>& r)
{
    return IntrusivePtr<T>(const_cast<T*>(r.get()));
}

/**
 * @brief Performs a dynamic pointer cast.
 *
 * @tparam T Target element type (must be polymorphic).
 * @param r Source pointer.
 * @return A new owning pointer, or null when the cast fails.
 */
template <typename T, typename U>
inline IntrusivePtr<T> dynamic_pointer_cast(const IntrusivePtr<U>& r)
{
    return IntrusivePtr<T>(dynamic_cast<T*>(r.get()));
}

/**
 * @brief Alias for IntrusivePtr.
 */
template <typename T>
using IPtr = IntrusivePtr<T>;

V_CORE_NS_END
