#pragma once
#include "core_global.hpp"

#include <cstddef>
#include <string>
#include <vector>

VI_CORE_NS_BEGIN

/** Optimized C-style string length function
 *  Supports char, char16_t, and char32_t with platform-specific optimizations.
 *  @tparam T Character type (char, char16_t, or char32_t)
 *  @param data Null-terminated string pointer
 *  @return The length of the string (number of characters before null terminator)
 */
template <typename T>
size_t cstrlen(const T* data);

/** Specialization for char using strlen
 *  Uses optimized std::strlen for maximum performance.
 */
template <>
VI_CORE_API size_t cstrlen<char>(const char* data);

/** Specialization for char16_t
 *  Platform-optimized: uses wcslen on Windows, pointer traversal on Unix/Linux.
 */
template <>
VI_CORE_API size_t cstrlen<char16_t>(const char16_t* data);

/** Specialization for char32_t
 *  Platform-optimized: uses wcslen on Unix/Linux, pointer traversal on Windows.
 */
template <>
VI_CORE_API size_t cstrlen<char32_t>(const char32_t* data);

/** String class that wraps std::string and provides additional functionality
 *  such as encoding conversions, formatting, and splitting.
 *  its a utf-8 string, but the internal storage is std::string for simplicity and performance.
 */
class VI_CORE_API String final {

  public:
    using impl_type              = std::u8string;
    using value_type             = std::u8string::value_type;
    using size_type              = std::u8string::size_type;
    using iterator               = std::u8string::iterator;
    using const_iterator         = std::u8string::const_iterator;
    using reverse_iterator       = std::u8string::reverse_iterator;
    using const_reverse_iterator = std::u8string::const_reverse_iterator;

  public:
    /** Default constructor, creates an empty string */
    constexpr String()
    {}

    constexpr String(size_type count, value_type ch)
      : stdstr_(count, ch)
    {}

    /** Constructor from null-terminated C-style string */
    constexpr String(const value_type* data)
      : stdstr_(data)
    {}

    /** Constructor from character array with specified length */
    constexpr String(const value_type* data, size_type count)
      : stdstr_(data, count)
    {}

    /** Constructor from std::u8string */
    constexpr explicit String(const impl_type& str)
      : stdstr_(str)
    {}

    constexpr explicit String(impl_type&& str)
      : stdstr_(std::move(str))
    {}

    /** Copy constructor */
    constexpr String(const String& other) noexcept
      : stdstr_(other.stdstr_)
    {}

    /** Move constructor */
    constexpr String(String&& other) noexcept
      : stdstr_(std::move(other.stdstr_))
    {}

  public:
    /** reinterpret_cast the internal std::u8string as a std::string
     *  the encoding of the string is UTF-8
     *  @return Reference to the internal std::u8string
     */
    std::string& stdstr()
    {
        return reinterpret_cast<std::string&>(stdstr_);
    }

    /** reinterpret_cast the internal std::u8string as a std::string
     *  the encoding of the string is UTF-8
     *  @return Const reference to the internal std::u8string
     */
    const std::string& stdstr() const
    {
        return reinterpret_cast<const std::string&>(stdstr_);
    }

    /** Get a reference to the internal std::u8string
     *  @return Reference to the internal std::u8string
     */
    constexpr std::u8string& stdu8str()
    {
        return stdstr_;
    }

    /** Get a const reference to the internal std::u8string
     *  @return Const reference to the internal std::u8string
     */
    constexpr const std::u8string& stdu8str() const
    {
        return stdstr_;
    }

    /** Get the number of characters in the string
     *  @return The length of the string
     */
    constexpr size_type size() const noexcept
    {
        return stdstr_.size();
    }

    /** Get the number of characters in the string (alias for size())
     *  @return The length of the string
     */
    constexpr size_type length() const noexcept
    {
        return stdstr_.length();
    }

    /** Get the allocated capacity of the string buffer
     *  @return The capacity of the allocated buffer
     */
    constexpr size_type capacity() const noexcept
    {
        return stdstr_.capacity();
    }

    /** Check if the string is empty
     *  @return true if the string has no characters, false otherwise
     */
    constexpr bool empty() const noexcept
    {
        return stdstr_.empty();
    }

    /** Get a mutable pointer to the string data
     *  @return Pointer to the internal character array
     *  @warning The returned pointer becomes invalid if the string is modified through methods like
     *           assign(), insert(), replace(), clear(), resize(), or any operation that changes capacity.
     *           Do not use the pointer after any such modification.
     *  @note The returned pointer is valid as long as the string is not modified
     */
    constexpr value_type* data() noexcept
    {
        return stdstr_.data();
    }

    /** Get a const pointer to the string data
     *  @return Const pointer to the internal character array
     *  @warning The returned pointer becomes invalid if the string is modified through methods like
     *           assign(), insert(), replace(), clear(), resize(), or any operation that changes capacity.
     *           Do not use the pointer after any such modification.
     *  @note The returned pointer is valid as long as the string is not modified
     */
    constexpr const value_type* data() const noexcept
    {
        return stdstr_.data();
    }

    /** Clear the string, making it empty
     *  @note The capacity is not changed
     */
    constexpr void clear() noexcept
    {
        stdstr_.clear();
    }

    /** Resize the string to a new size
     *  @param new_size The new size of the string
     *  @param ch Character to fill new positions with (default: null character)
     *  @note If new_size is larger than current size, appends ch characters
     *        If new_size is smaller than current size, truncates the string
     */
    constexpr void resize(size_type new_size, value_type ch = {})
    {
        stdstr_.resize(new_size, ch);
    }

    /** Reserve capacity for the string
     *  @param new_cap The new capacity to reserve
     *  @note If new_cap is less than current capacity, this call has no effect
     */
    constexpr void reserve(size_type new_cap)
    {
        stdstr_.reserve(new_cap);
    }

    /** Shrink the capacity to match the current size
     *  @note Releases unused memory allocated by the string
     */
    constexpr void shrinkToFit() noexcept
    {
        stdstr_.shrink_to_fit();
    }

    /** Swap contents with another string
     *  @param s The string to swap with
     *  @note This operation is guaranteed to not throw
     */
    constexpr void swap(String& s) noexcept
    {
        stdstr_.swap(s.stdstr_);
    }

    /** Assign a character repeated count times
     *  @param ch The character to assign
     *  @param count The number of times to repeat the character
     *  @return Reference to this string
     *  @note Replaces the current string content
     */
    constexpr String& assign(value_type ch, size_type count)
    {
        stdstr_.assign(count, ch);
        return *this;
    }

    /** Assign from another string
     *  @param str The source string to assign
     *  @return Reference to this string
     *  @note Performs a deep copy of the source string
     */
    constexpr String& assign(const String& str)
    {
        stdstr_.assign(str.stdstr_);
        return *this;
    }

    /** Assign a substring from another string
     *  @param str The source string
     *  @param pos The starting position in the source string
     *  @param count The number of characters to copy
     *  @return Reference to this string
     *  @note Copies characters [pos, pos + count) from str
     */
    constexpr String& assign(const String& str, size_type pos, size_type count)
    {
        stdstr_.assign(str.stdstr_, pos, count);
        return *this;
    }

    /** Assign using move semantics from another string
     *  @param str The source string (will be moved from)
     *  @return Reference to this string
     *  @note Transfers ownership of the buffer from str to this string
     */
    constexpr String& assign(String&& str)
    {
        stdstr_.assign(std::move(str.stdstr_));
        return *this;
    }

    /** Assign from an iterator range
     *  @param begin Iterator to the beginning of the range
     *  @param end Iterator to the end of the range (exclusive)
     *  @return Reference to this string
     *  @note Copies all characters in the range [begin, end)
     */
    constexpr String& assign(const_iterator begin, const_iterator end)
    {
        stdstr_.assign(begin, end);
        return *this;
    }

    /** Assign from a C-style character array
     *  @param data Pointer to the character array
     *  @param count The number of characters to copy (default: NPOS = until null terminator)
     *  @return Reference to this string
     *  @note If count is NPOS, all characters up to the null terminator are copied
     */
    constexpr String& assign(const value_type* data, size_type count)
    {
        stdstr_.assign(data, count);
        return *this;
    }

    /** Insert a single character at the specified position
     *  @param pos The position to insert at
     *  @param ch The character to insert
     *  @return Reference to this string
     *  @note Shifts all characters from pos onwards to the right
     */
    constexpr String& insert(size_type pos, size_type count, value_type ch)
    {
        stdstr_.insert(pos, count, ch);
        return *this;
    }

    /** Insert a C-style string at the specified position
     *  @param pos The position to insert at
     *  @param data Pointer to the character array
     *  @param count The number of characters to insert (default: NPOS = until null terminator)
     *  @return Reference to this string
     *  @note If count is NPOS, all characters up to the null terminator are inserted
     */
    constexpr String& insert(size_type pos, const value_type* data, size_type count)
    {
        stdstr_.insert(pos, data, count);
        return *this;
    }

    /** Insert a string at the specified position
     *  @param pos The position to insert at
     *  @param str The source string to insert
     *  @return Reference to this string
     *  @note The entire str is inserted at position pos
     */
    constexpr String& insert(size_type pos, const String& str)
    {
        stdstr_.insert(pos, str.stdstr_);
        return *this;
    }

    /** Insert a substring at the specified position
     *  @param pos The position to insert at
     *  @param str The source string
     *  @param str_pos The starting position in the source string
     *  @param count The number of characters to insert (default: NPOS)
     *  @return Reference to this string
     *  @note Inserts characters [str_pos, str_pos + count) from str at position pos
     */
    constexpr String& insert(size_type pos, const String& str, size_type str_pos, size_type count)
    {
        stdstr_.insert(pos, str.stdstr_, str_pos, count);
        return *this;
    }

    /** Replace a range with a single character
     *  @param pos The starting position of the range to replace
     *  @param count1 The number of characters to replace
     *  @param count2 The number of characters to insert
     *  @param ch The character to use as replacement
     *  @return Reference to this string
     *  @note Replaces characters [pos, pos + count1) with count2 copies of ch
     */
    constexpr String& replace(size_type pos, size_type count1, size_type count2, value_type ch)
    {
        stdstr_.replace(pos, count1, count2, ch);
        return *this;
    }

    /** Replace a range with a C-style string
     *  @param pos The starting position of the range to replace
     *  @param count The number of characters to replace
     *  @param data Pointer to the character array
     *  @param data_count The number of characters to use (default: NPOS = until null terminator)
     *  @return Reference to this string
     *  @note Replaces characters [pos, pos + count) with data[0..data_count)
     */
    constexpr String& replace(size_type pos, size_type count, const value_type* data, size_type data_count)
    {
        stdstr_.replace(pos, count, data, data_count);
        return *this;
    }

    /** Replace a range with another string
     *  @param pos The starting position of the range to replace
     *  @param count The number of characters to replace
     *  @param str The source string
     *  @return Reference to this string
     *  @note Replaces characters [pos, pos + count) with the entire str
     */
    constexpr String& replace(size_type pos, size_type count, const String& str)
    {
        stdstr_.replace(pos, count, str.stdstr_);
        return *this;
    }

    /** Replace a range with a substring
     *  @param pos The starting position of the range to replace
     *  @param count The number of characters to replace
     *  @param str The source string
     *  @param str_pos The starting position in the source string
     *  @param str_count The number of characters to use (default: NPOS)
     *  @return Reference to this string
     *  @note Replaces characters [pos, pos + count) with str[str_pos, str_pos + str_count)
     */
    constexpr String& replace(size_type pos, size_type count, const String& str, size_type str_pos, size_type str_count)
    {
        stdstr_.replace(pos, count, str.stdstr_, str_pos, str_count);
        return *this;
    }

    /** Get mutable reference to character at specified index
     *  @param idx The index of the character
     *  @return Reference to the character
     *  @warning The returned reference becomes invalid if the string is modified through methods like
     *           assign(), insert(), replace(), clear(), resize(), or any operation that changes capacity.
     *           Do not use the reference after any such modification.
     *  @note Does not perform bounds checking
     */
    constexpr value_type& at(size_type idx)
    {
        return stdstr_.at(idx);
    }

    /** Get const reference to character at specified index
     *  @param idx The index of the character
     *  @return Const reference to the character
     *  @warning The returned reference becomes invalid if the string is modified through methods like
     *           assign(), insert(), replace(), clear(), resize(), or any operation that changes capacity.
     *           Do not use the reference after any such modification.
     *  @note Does not perform bounds checking
     */
    constexpr const value_type& at(size_type idx) const
    {
        return stdstr_.at(idx);
    }

    /** Extract a substring
     *  @param start The starting position
     *  @param count The number of characters to extract (default: NPOS = to the end)
     *  @return A new string containing the substring
     *  @note Returns characters [start, start + count)
     *        If start >= size(), returns an empty string
     */
    constexpr String substr(size_type start, size_type count = -1) const
    {
        return String(stdstr_.substr(start, count).c_str());
    }

    /** Find the first occurrence of a character
     *  @param c The character to search for
     *  @param pos The starting position (default: 0)
     *  @return The index of the first occurrence, or NPOS if not found
     *  @note Searches in the range [pos, size())
     */
    constexpr size_type find(value_type c, size_type pos = 0) const
    {
        return stdstr_.find(c, pos);
    }

    /** Find the first occurrence of a substring
     *  @param str The substring to search for
     *  @param pos The starting position (default: 0)
     *  @return The index of the first occurrence, or NPOS if not found
     *  @note Searches in the range [pos, size())
     *        If str is empty, returns pos
     */
    constexpr size_type find(const String& str, size_type pos = 0) const
    {
        return stdstr_.find(str.stdstr_, pos);
    }

    /** Find the first occurrence of a C-style string
     *  @param data The C-style string to search for
     *  @param pos The starting position (default: 0)
     *  @return The index of the first occurrence, or NPOS if not found
     *  @note Searches in the range [pos, size())
     */
    constexpr size_type find(const value_type* data, size_type pos = 0) const
    {
        return stdstr_.find(data, pos);
    }

    /** Find the last occurrence of a character
     *  @param c The character to search for
     *  @param pos The starting position for backward search (default: npos = from the end)
     *  @return The index of the last occurrence, or NPOS if not found
     *  @note Searches backward in the range [0, pos]
     */
    constexpr size_type rfind(value_type c, size_type pos = -1) const
    {
        return stdstr_.rfind(c, pos);
    }

    /** Find the last occurrence of a substring
     *  @param str The substring to search for
     *  @param pos The starting position for backward search (default: NPOS = from the end)
     *  @return The index of the last occurrence, or NPOS if not found
     *  @note Searches backward in the range [0, pos]
     *        If str is empty, returns min(pos, size())
     */
    constexpr size_type rfind(const String& str, size_type pos = -1) const
    {
        return stdstr_.rfind(str.stdstr_, pos);
    }

    /** Find the last occurrence of a C-style string
     *  @param data The C-style string to search for
     *  @param pos The starting position for backward search (default: NPOS = from the end)
     *  @return The index of the last occurrence, or NPOS if not found
     *  @note Searches backward in the range [0, pos]
     */
    constexpr size_type rfind(const value_type* data, size_type pos = -1) const
    {
        return stdstr_.rfind(data, pos);
    }

    /** Convert string to lowercase
     *  @return A new string with lowercase characters
     *  @note This string is not modified
     */
    String toLower() const;

    /** Convert string to uppercase
     *  @return A new string with uppercase characters
     *  @note This string is not modified
     */
    String toUpper() const;

    /** Remove whitespace from the beginning of the string (modifies in-place)
     *  @return Reference to this string
     *  @note Removes all leading whitespace characters (space, tab, newline, carriage return, form feed, vertical tab)
     */
    String& trimStart();

    /** Get a copy with whitespace removed from the beginning
     *  @return A new string with leading whitespace removed
     *  @note This string is not modified
     */
    String trimmedStart() const
    {
        String result(*this);
        return result.trimStart();
    }

    /** Get a copy with whitespace removed from the end
     *  @return A new string with trailing whitespace removed
     *  @note This string is not modified
     */
    String trimmedEnd() const
    {
        String result(*this);
        return result.trimEnd();
    }

    /** Remove whitespace from the end of the string (modifies in-place)
     *  @return Reference to this string
     *  @note Removes all trailing whitespace characters (space, tab, newline, carriage return, form feed, vertical tab)
     */
    String& trimEnd();

    /** Remove whitespace from both ends of the string (modifies in-place)
     *  @return Reference to this string
     *  @note Removes all leading and trailing whitespace characters
     */
    String& trim()
    {
        return trimStart().trimEnd();
    }

    /** Get a copy with whitespace removed from both ends
     *  @return A new string with all leading and trailing whitespace removed
     *  @note This string is not modified
     */
    String trimmed() const
    {
        String result(*this);
        return result.trim();
    }

    /** Check if the string equals another string
     *  @param other The string to compare with
     *  @param ignore_case If true, performs case-insensitive comparison (default: false)
     *  @return true if the strings are equal, false otherwise
     */
    bool equals(const String& other, bool ignore_case = false) const
    {
        if (stdstr_.size() != other.stdstr_.size())
            return false;

        if (ignore_case) {
            return std::equal(stdstr_.begin(), stdstr_.end(), other.stdstr_.begin(), other.stdstr_.end(), [](char a, char b) {
                return std::tolower(a) == std::tolower(b);
            });
        }
        else {
            return stdstr_ == other.stdstr_;
        }
    }

    /** Check if the string starts with a single character
     *  @param c The character to search for
     *  @param ignore_case If true, performs case-insensitive comparison (default: false)
     *  @return true if the string starts with the character, false otherwise
     */
    bool startsWith(value_type c, bool ignore_case = false) const
    {
        if (stdstr_.empty())
            return false;

        if (ignore_case) {
            return std::tolower(stdstr_[0]) == std::tolower(c);
        }
        else {
            return stdstr_[0] == c;
        }
    }

    /** Check if the string starts with another string
     *  @param str The prefix to check for
     *  @param ignore_case If true, performs case-insensitive comparison (default: false)
     *  @return true if the string starts with str, false otherwise
     */
    bool startsWith(const String& str, bool ignore_case = false) const
    {
        if (str.size() > stdstr_.size())
            return false;

        if (ignore_case) {
            return std::equal(str.stdstr_.begin(), str.stdstr_.end(), stdstr_.begin(), [](char a, char b) { return std::tolower(a) == std::tolower(b); });
        }
        else {
            return stdstr_.compare(0, str.size(), str.stdstr_) == 0;
        }
    }

    /** Check if the string ends with a single character
     *  @param c The character to search for
     *  @param ignore_case If true, performs case-insensitive comparison (default: false)
     *  @return true if the string ends with the character, false otherwise
     */
    bool endsWith(value_type c, bool ignore_case = false) const
    {
        if (stdstr_.empty())
            return false;

        if (ignore_case) {
            return std::tolower(stdstr_.back()) == std::tolower(c);
        }
        else {
            return stdstr_.back() == c;
        }
    }

    /** Check if the string ends with another string
     *  @param str The suffix to check for
     *  @param ignore_case If true, performs case-insensitive comparison (default: false)
     *  @return true if the string ends with str, false otherwise
     */
    bool endsWith(const String& str, bool ignore_case = false) const
    {
        if (str.size() > stdstr_.size())
            return false;

        if (ignore_case) {
            return std::equal(str.stdstr_.rbegin(), str.stdstr_.rend(), stdstr_.rbegin(), [](char a, char b) { return std::tolower(a) == std::tolower(b); });
        }
        else {
            return stdstr_.compare(stdstr_.size() - str.size(), str.size(), str.stdstr_) == 0;
        }
    }

    /** Split string by delimiter
     *  @param delimiter The character to split on
     *  @param keep_empty Whether to keep empty substrings (default: true)
     *  @return Vector of substrings
     *  @note If keep_empty is false, consecutive delimiters will not produce empty strings
     */
    std::vector<String> split(value_type delimiter, bool keep_empty = true) const;

    /** Split string by delimiter
     *  @param delimiters The characters to split on
     *  @param keep_empty Whether to keep empty substrings (default: true)
     *  @return Vector of substrings
     *  @note If keep_empty is false, consecutive delimiters will not produce empty strings
     */
    std::vector<String> split(const std::initializer_list<value_type>& delimiters, bool keep_empty = true) const;

    /** Split string by delimiter string
     *  @param delimiter The string to split on
     *  @param keep_empty Whether to keep empty substrings (default: true)
     *  @return Vector of substrings
     *  @note If keep_empty is false, consecutive delimiters will not produce empty strings
     */
    std::vector<String> split(const String& delimiter, bool keep_empty = true) const;

    /** Get iterator to the beginning of the string
     *  @return Iterator pointing to the first character
     *  @warning Any modification to the string through methods like assign(), insert(), replace(),
     *           clear(), resize(), or any operation that changes capacity invalidates all iterators.
     *           Do not use iterators after any such modification.
     */
    constexpr iterator begin() noexcept
    {
        return stdstr_.begin();
    }

    /** Get iterator to the end of the string
     *  @return Iterator pointing one past the last character
     *  @warning Any modification to the string through methods like assign(), insert(), replace(),
     *           clear(), resize(), or any operation that changes capacity invalidates all iterators.
     *           Do not use iterators after any such modification.
     */
    constexpr iterator end() noexcept
    {
        return stdstr_.end();
    }

    /** Get const iterator to the beginning of the string
     *  @return Const iterator pointing to the first character
     *  @warning Any modification to the string through methods like assign(), insert(), replace(),
     *           clear(), resize(), or any operation that changes capacity invalidates all iterators.
     *           Do not use iterators after any such modification.
     */
    constexpr const_iterator cbegin() const noexcept
    {
        return stdstr_.cbegin();
    }

    /** Get const iterator to the end of the string
     *  @return Const iterator pointing one past the last character
     *  @warning Any modification to the string through methods like assign(), insert(), replace(),
     *           clear(), resize(), or any operation that changes capacity invalidates all iterators.
     *           Do not use iterators after any such modification.
     */
    constexpr const_iterator cend() const noexcept
    {
        return stdstr_.cend();
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return stdstr_.rbegin();
    }

    constexpr reverse_iterator rend() noexcept
    {
        return stdstr_.rend();
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return stdstr_.crbegin();
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return stdstr_.crend();
    }

    /** Convert string to integer
     *  @return The integer value
     *  @param ok Optional pointer to bool, set to true if conversion succeeded, false otherwise
     *  @note If the string cannot be converted to a valid integer, returns 0 and sets ok to false.
     *        The conversion stops at the first non-digit character (after optional leading whitespace).
     *  @example String("123").toInt() returns 123
     *           String("45ab").toInt() returns 45 (stops at 'a')
     *           String("abc").toInt(&ok) returns 0, ok = false
     */
    int toInt(bool* ok = nullptr, int base = 10) const
    {
        auto  str    = reinterpret_cast<const char*>(stdstr_.c_str());
        char* endptr = nullptr;
        auto  result = std::strtol(str, &endptr, base);
        if (ok) {
            *ok = (endptr != str); // Conversion succeeded if endptr moved past the start
        }
        return static_cast<int>(result);
    }

    /** Convert string to double
     *  @return The double value
     *  @param ok Optional pointer to bool, set to true if conversion succeeded, false otherwise
     *  @note If the string cannot be converted to a valid double, returns 0.0 and sets ok to false.
     *        Supports formats: "123", "123.45", "1.23e-4", "-123.45", etc.
     *  @example String("123.45").toDouble() returns 123.45
     *           String("1.23e-4").toDouble() returns 0.000123
     *           String("abc").toDouble(&ok) returns 0.0, ok = false
     */
    double toDouble(bool* ok = nullptr) const
    {
        auto  str    = reinterpret_cast<const char*>(stdstr_.c_str());
        char* endptr = nullptr;
        auto  result = std::strtod(str, &endptr);
        if (ok) {
            *ok = (endptr != str); // Conversion succeeded if endptr moved past the start
        }
        return result;
    }

    /** Convert string to local 8-bit encoded bytes
     *  @return std::string containing local 8-bit encoded data
     *  @note On Windows, uses Win32 API to convert to the system's ANSI code page (CP_ACP)
     *        On Linux/other systems, not implemented yet and will throw an exception
     *  @throws Exception if the encoding is not supported on the platform
     */
    std::string toLocal8Bit() const;

    /** Convert string to UTF-16 encoded code units
     *  @return std::u16string containing UTF-16 encoded data
     *  @note The returned string contains properly encoded UTF-16 surrogates for characters beyond the BMP
     */
    std::u16string toUtf16() const;

    /** Convert string to UTF-32 encoded code units
     *  @return std::u32string containing UTF-32 encoded data
     *  @note The returned string contains properly encoded UTF-32 code units for all characters
     */
    std::u32string toUtf32() const;

  public:
    /** Create a string from local 8-bit encoded data
     *  @param data Pointer to local 8-bit encoded character array
     *  @return A new String with the decoded data
     *  @note The encoding depends on the platform's locale settings
     *  @throws e.g., invalid encoding or unsupported locale
     */
    static String fromLocal8Bit(const char* data, size_type count = std::string::npos);

    /** Create a string from UTF-16 encoded data
     *  @param data Pointer to UTF-16 encoded character array
     *  @param count The number of code units to read (default: NPOS = until null terminator)
     *  @return A new String with the decoded UTF-16 data
     *  @note Properly handles UTF-16 surrogate pairs for characters beyond the BMP
     *        If count is NPOS, processing continues until a null terminator is found
     */
    static String fromUtf16(const char16_t* data, size_type count = std::string::npos);

    /** Create a string from UTF-32 encoded data
     *  @param data Pointer to UTF-32 encoded character array
     *  @param count The number of code units to read (default: NPOS = until null terminator)
     *  @return A new String with the decoded UTF-32 data
     *  @note Properly handles UTF-32 code points
     *        If count is NPOS, processing continues until a null terminator is found
     */
    static String fromUtf32(const char32_t* data, size_type count = std::string::npos);

  public:
    /** Assignment operator
     *  @param right The string to assign from
     *  @return Reference to this string
     *  @note Performs a deep copy of the source string
     */
    constexpr String& operator=(const String& right)
    {
        stdstr_ = right.stdstr_;
        return *this;
    }

    /** Move assignment operator
     *  @param right The string to move from
     *  @return Reference to this string
     *  @note Transfers ownership of the buffer from right to this string
     */
    constexpr String& operator=(String&& right)
    {
        stdstr_ = std::move(right.stdstr_);
        return *this;
    }

    /** Get mutable reference to character at specified index
     *  @param idx The index of the character
     *  @return Reference to the character
     *  @note Does not perform bounds checking
     */
    constexpr value_type& operator[](size_type idx)
    {
        return stdstr_[idx];
    }

    /** Get const reference to character at specified index
     *  @param idx The index of the character
     *  @return Const reference to the character
     *  @note Does not perform bounds checking
     */
    constexpr const value_type& operator[](size_type idx) const
    {
        return stdstr_[idx];
    }

    /** Equality comparison operator
     *  @param right The string to compare with
     *  @return true if the strings are equal, false otherwise
     */
    constexpr bool operator==(const String& right) const
    {
        return stdstr_ == right.stdstr_;
    }

    /** Inequality comparison operator
     *  @param right The string to compare with
     *  @return true if the strings are not equal, false otherwise
     */
    constexpr bool operator!=(const String& right) const
    {
        return stdstr_ != right.stdstr_;
    }

    /** Less than comparison operator
     *  @param right The string to compare with
     *  @return true if this string is lexicographically less than right
     */
    constexpr bool operator<(const String& right) const
    {
        return stdstr_ < right.stdstr_;
    }

    /** Greater than comparison operator
     *  @param right The string to compare with
     *  @return true if this string is lexicographically greater than right
     */
    constexpr bool operator>(const String& right) const
    {
        return stdstr_ > right.stdstr_;
    }

    /** String concatenation operator
     *  @param right The string to concatenate
     *  @return A new string containing this string followed by right
     *  @note This string is not modified
     */
    constexpr String operator+(const String& right) const
    {
        String result;
        result.stdstr_ = stdstr_ + right.stdstr_;
        return result;
    }

    /** Append operator
     *  @param right The string to append
     *  @return Reference to this string
     *  @note Appends right to the end of this string
     */
    constexpr String& operator+=(const String& right)
    {
        stdstr_ += right.stdstr_;
        return *this;
    }

  private:
    std::u8string stdstr_; // Internal storage for string data
};

template <typename T>
size_t cstrlen(const T* data)
{
    const T* p = data;
    while (*p != T(0)) ++p;
    return p - data;
}

VI_CORE_NS_END
