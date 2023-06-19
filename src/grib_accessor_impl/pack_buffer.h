#ifndef _PACK_BUFFER_H_20230613_
#define _PACK_BUFFER_H_20230613_

#include <variant>
#include <vector>
#include <type_traits>

struct grib_context;

namespace eccodes {
 
    template<typename T>
    struct type_view{
        using type = T;
        T* ptr{};
        std::size_t len{};
    };

    using const_byte_view   = type_view<const unsigned char>;
    using const_double_view = type_view<const double>;
    using const_float_view  = type_view<const float>;
    using const_long_view   = type_view<const long>;
    using const_char_view   = type_view<const char>;

    using const_view_type = std::variant<const_byte_view, const_double_view, const_float_view, const_long_view, const_char_view>;

    using byte_view   = type_view<unsigned char>;
    using double_view = type_view<double>;
    using float_view  = type_view<float>;
    using long_view   = type_view<long>;
    using char_view   = type_view<char>;

    using view_type = std::variant<byte_view, double_view, float_view, long_view, char_view>;

    class pack_buffer{
        grib_context *context; // To give us access to the Grib memory functions
        const_view_type in_view;
        int status{};   // Default value 0 = GRIB_SUCCESS;

        byte_view bytes{};
        double_view doubles{};
        float_view floats{};
        long_view longs{};
        char_view chars{};

    public:
        pack_buffer(grib_context *context_ptr, const_view_type input_view);
        ~pack_buffer();

        const_view_type input_view() const { return in_view; }
        int current_status() const { return status; }

        const_byte_view to_bytes();
        const_double_view to_doubles();
        const_float_view to_floats();
        const_long_view to_longs();
        const_char_view to_string();
    };

    void pack_buffer_test();
 }

#endif // _PACK_BUFFER_H_20230613_
