#pragma once
#include "core_global.hpp"

#include <cstddef>
#include <cstdint>
#include <ostream>

V_CORE_NS_BEGIN

/**
 * @brief An output stream that wraps an existing byte range.
 *
 * Wraps a `const void*` buffer together with its length as an `std::ostream`
 * so the bytes can be read back through the standard stream interface. The
 * buffer is not copied and must outlive the stream.
 */
class MemoryStream : public std::ostream
{
  private:
    /**
     * @brief Stream buffer that exposes the wrapped bytes in its get area.
     *
     * The bytes live in the get area, so they can be read back through the
     * standard rdbuf() interface even though the stream is an ostream.
     */
    class StreamBuf : public std::streambuf
    {
      public:
        /**
         * @brief Constructs a buffer over an existing byte range.
         *
         * @param data Pointer to the first byte.
         * @param size Number of bytes.
         */
        explicit StreamBuf(const std::uint8_t* data, std::size_t size) noexcept
        {
            if (size == 0) {
                setg(nullptr, nullptr, nullptr);
                return;
            }
            auto* p = reinterpret_cast<char*>(const_cast<std::uint8_t*>(data));
            setg(p, p, p + size);
        }
    };

  public:
    /**
     * @brief Constructs a stream over an existing byte range.
     *
     * @param data Pointer to the first byte.
     * @param size Number of bytes.
     */
    MemoryStream(const void* data, std::size_t size) noexcept
        : std::ostream(nullptr)
        , data_(static_cast<const std::uint8_t*>(data))
        , size_(data == nullptr ? 0 : size)
        , buf_(data_, size_)
    {
        rdbuf(&buf_);
        clear();
    }

    /**
     * @brief Returns a pointer to the first byte.
     *
     * @return The wrapped buffer pointer (nullptr when the stream is empty).
     */
    const std::uint8_t* data() const noexcept
    {
        return data_;
    }

    /**
     * @brief Returns the number of wrapped bytes.
     *
     * @return The wrapped buffer size.
     */
    std::size_t size() const noexcept
    {
        return size_;
    }

  private:
    const std::uint8_t* data_;
    std::size_t         size_;
    StreamBuf           buf_;
};

V_CORE_NS_END
