#ifndef _ACCESSOR_BUFFER_H_20230817_
#define _ACCESSOR_BUFFER_H_20230817_

#include <string>
#include <memory>
#include <vector>
#include <cstddef>

// Provides a non-owning view to a contiguous block of memory, accessed as std::byte
//
// Required because we have to use C++17, so don't have access to std::span
//
// Note: Ensure that the underlying data being viewed doesn't go out of scope before
//       this view (c.f. std::string_view)
//

namespace eccodes::accessor {

class AccessorBuffer {
public:
    using value_type = char;
    using pointer = value_type*;
    using const_pointer = value_type const*;
    using reference = value_type&;
    using const_reference = value_type const&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    constexpr AccessorBuffer() noexcept : data_{ nullptr }, size_{ 0 } {};

    constexpr AccessorBuffer(AccessorBuffer const&) noexcept = default;
    constexpr AccessorBuffer& operator=(AccessorBuffer const&) noexcept = default;

    template<typename T>
    constexpr AccessorBuffer(T *const buffer, const size_type num_elements) noexcept
        : data_(reinterpret_cast<pointer>(buffer)), size_(sizeof(T) * num_elements) {}

    [[nodiscard]] constexpr size_type size_bytes() const noexcept {
        return size_;
    }
    
    [[nodiscard]] constexpr bool empty() const noexcept {
        return size_ == 0;
    }

    [[nodiscard]] constexpr pointer data() const noexcept { 
        return data_; 
    }

private:
    pointer data_;
    size_type size_;
};

}

#endif // _ACCESSOR_BUFFER_H_20230817_
