#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cstddef>

// Provides a non-owning view to a contiguous block of memory, accessed as unsigned char
//
// Required because we have to use C++17, so don't have access to std::span
//
// Note: Ensure that the underlying data being viewed doesn't go out of scope before
//       this view (c.f. std::string_view)
//

namespace eccodes::accessor {

class AccessorDataView {
public:
    using value_type = unsigned char;
    using pointer = value_type*;
    using const_pointer = value_type const*;
    using size_type = size_t;

    constexpr AccessorDataView() noexcept : data_{ nullptr }, size_{ 0 } {};

    constexpr AccessorDataView(AccessorDataView const&) noexcept = default;
    constexpr AccessorDataView& operator=(AccessorDataView const&) noexcept = default;

    AccessorDataView(value_type *const buffer, const size_type num_elements) noexcept
        : data_(reinterpret_cast<pointer>(buffer)), size_(sizeof(value_type) * num_elements) {}

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

using AccessorDataPointer = unsigned char*;

using AccessorDataBuffer = std::vector<unsigned char>;

}
