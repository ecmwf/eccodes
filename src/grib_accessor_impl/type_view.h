#ifndef _TYPE_VIEW_H_20230619_
#define _TYPE_VIEW_H_20230619_

#include <cstddef>

namespace eccodes {
 
    // Helper object to wrap a pointer and its size

    template<typename T>
    struct type_view{
        using type = T;
        T* ptr{};
        std::size_t* len{};
    };

    using const_byte_view   = type_view<const unsigned char>;
    using const_double_view = type_view<const double>;
    using const_float_view  = type_view<const float>;
    using const_long_view   = type_view<const long>;
    using const_char_view   = type_view<const char>;
 
    using byte_view         = type_view<unsigned char>;
    using double_view       = type_view<double>;
    using float_view        = type_view<float>;
    using long_view         = type_view<long>;
    using char_view         = type_view<char>;
}

#endif // _TYPE_VIEW_H_20230619_
