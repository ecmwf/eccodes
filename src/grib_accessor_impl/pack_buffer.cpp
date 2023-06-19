#include "pack_buffer.h"
#include <type_traits>

#include "grib_api_internal.h"

namespace eccodes {  

    // is_variant_member 
    //
    // This template allows us to check if the type of IN_PTR and OUT_PTR
    // used below are valid. This is done by checking if the type has been 
    // defined as part of the appropriate std::variant
    template <class T, class... Us>
    struct is_one_of : std::false_type {};

    template <class T, class U>
    struct is_one_of<T, U> : std::is_same<T, U> {};

    template <class T, class U, class... Us>
    struct is_one_of<T, U, Us...> : std::bool_constant<std::is_same<T, U>::value || is_one_of<T, Us...>::value> {};

    template <class T, class Variant>
    struct is_variant_member;

    template <class T, class... Us>
    struct is_variant_member<T, std::variant<Us...>> : is_one_of<T, Us...> {};

    // Helper function to take the input pointer and transform it to a buffer of the desired type
    //
    // IN_VIEW is the active type_view held by pack_buffer::in_view, as passed into std::visit()
    // OUT_VIEW is the type_view to write the results to, e.g. double_buffer_view for the doubles member
    //
    // Returns a GRIB status code
    template<typename IN_VIEW, typename OUT_VIEW>
    int transform(IN_VIEW const& in_view, OUT_VIEW& out_view, grib_context *c)
    {
        static_assert(is_variant_member<IN_VIEW, const_view_type>::value,
            "IN_VIEW must be one of the types defined in const_view_type");

        static_assert(is_variant_member<OUT_VIEW, view_type>::value,
            "OUT_VIEW must be one of the types defined in const_view_type");

        Assert(in_view.ptr && !out_view.ptr);

        using out_type = typename OUT_VIEW::type;
        std::size_t out_buffer_size_bytes = *in_view.len * sizeof(out_type);

        if(out_view.ptr = (out_type*)grib_context_malloc(c, out_buffer_size_bytes); !out_view.ptr)
        {
            grib_context_log(c, GRIB_LOG_ERROR, "Unable to allocate %ld bytes\n", out_buffer_size_bytes);
            return GRIB_OUT_OF_MEMORY;
        }

        for(std::size_t index = 0; index < *in_view.len; ++index)
        {
            out_view.ptr[index] = static_cast<out_type>(in_view.ptr[index]);
        }

        out_view.len = in_view.len;

        return GRIB_SUCCESS;
    }

    // transform() overload for string to double
    // Note: we don't specialise the template: https://www.modernescpp.com/index.php/full-specialization-of-function-templates
    int transform(const_char_view const& in_view, double_view& out_view, grib_context *c)
    {
        Assert(in_view.ptr && !out_view.ptr);

        std::size_t dlen = 1;
        std::size_t out_buffer_size_bytes = dlen*sizeof(double);
        if(out_view.ptr = (double*)grib_context_malloc(c, out_buffer_size_bytes); !out_view.ptr)
        {
            grib_context_log(c, GRIB_LOG_ERROR, "Unable to allocate %ld bytes\n", out_buffer_size_bytes);
            return GRIB_OUT_OF_MEMORY;
        }

        char* endPtr = NULL; /* for error handling */
        *out_view.ptr = strtod(in_view.ptr, &endPtr);
        if (*endPtr) {
// TODO - We don't have the accessor's name here
//            grib_context_log(c, GRIB_LOG_ERROR,
//                             "pack_string: Invalid value (%s) for %s. String cannot be converted to a double",
//                             v, a->name);
            grib_context_log(c, GRIB_LOG_ERROR,
                             "pack_string: Invalid value (%s). String cannot be converted to a double",
                             *in_view.ptr);
            return GRIB_WRONG_TYPE;
        }
        *out_view.len = dlen;

        return GRIB_SUCCESS;
    }

    // transform() overload for string to long
    // Note: we don't specialise the template: https://www.modernescpp.com/index.php/full-specialization-of-function-templates
    int transform(const_char_view const& in_view, long_view& out_view, grib_context *c)
    {
        Assert(in_view.ptr && !out_view.ptr);

        std::size_t llen = 1;
        std::size_t out_buffer_size_bytes = llen*sizeof(long);
        if(out_view.ptr = (long*)grib_context_malloc(c, out_buffer_size_bytes); !out_view.ptr)
        {
            grib_context_log(c, GRIB_LOG_ERROR, "Unable to allocate %ld bytes\n", out_buffer_size_bytes);
            return GRIB_OUT_OF_MEMORY;
        }

        *out_view.ptr = atol(in_view.ptr);
        *out_view.len = llen;
        
        return GRIB_SUCCESS;
    }

	// std::visit helper (with deduction guide as we're compiling as C++17)
	template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
	template<class... Ts> overload(Ts...) -> overload<Ts...>;

    pack_buffer::pack_buffer(grib_context *context_ptr, const_view_type input_view) : 
        context{context_ptr}, in_view{input_view} 
    {
        // Check that in_value_type is not a nullptr
        bool valid_input_values_ptr = std::visit(
			overload{
				[](const_byte_view cbv)   { return cbv.ptr!=nullptr; },
				[](const_double_view cdv) { return cdv.ptr!=nullptr; },
				[](const_float_view cfv)  { return cfv.ptr!=nullptr; },
				[](const_long_view clv)   { return clv.ptr!=nullptr; },
				[](const_char_view ccv)   { return ccv.ptr!=nullptr; },
			},
			in_view
		);
        Assert(valid_input_values_ptr);
    }

    pack_buffer::~pack_buffer()
    {
        if(bytes.ptr)   { grib_context_free(context, bytes.ptr); }
        if(doubles.ptr) { grib_context_free(context, doubles.ptr); }
        if(floats.ptr)  { grib_context_free(context, floats.ptr); }
        if(longs.ptr)   { grib_context_free(context, longs.ptr); }
        if(chars.ptr)   { grib_context_free(context, chars.ptr); }
   }

    const_byte_view pack_buffer::to_bytes()
    {
        if(!bytes.ptr) { 
            status = std::visit(
                overload{
                    [](const_byte_view cbv)   { return GRIB_NOT_IMPLEMENTED; },
                    [](const_double_view cdv) { return GRIB_NOT_IMPLEMENTED; },
                    [](const_float_view cfv)  { return GRIB_NOT_IMPLEMENTED; },
                    [](const_long_view clv)   { return GRIB_NOT_IMPLEMENTED; },
                    [](const_char_view ccv)   { return GRIB_NOT_IMPLEMENTED; },
                },
                in_view
            );
        }

        return const_byte_view{bytes.ptr, bytes.len};
    }
 
    const_double_view pack_buffer::to_doubles()
    {
        if(std::holds_alternative<const_double_view>(in_view)) { return std::get<const_double_view>(in_view); }

        if(!doubles.ptr) { 
            status = std::visit(
                overload{
                    [](const_byte_view cbv)     { return GRIB_NOT_IMPLEMENTED; },
                    [](const_double_view cdv)   { Assert(false); return GRIB_INTERNAL_ERROR; },
                    [](const_float_view cfv)    { return GRIB_NOT_IMPLEMENTED; },
                    [this](const_long_view clv) { return transform(clv, doubles, context); },
                    [this](const_char_view ccv) { return transform(ccv, doubles, context); },
                },
                in_view
            );
        }

        return const_double_view{doubles.ptr, doubles.len};
    }

    const_float_view pack_buffer::to_floats()
    {
        if(std::holds_alternative<const_float_view>(in_view)) { return std::get<const_float_view>(in_view); }

        if(!floats.ptr) {
            status = std::visit(
                overload{
                    [](const_byte_view cbv)     { return GRIB_NOT_IMPLEMENTED; },
                    [](const_double_view cdv)   { return GRIB_NOT_IMPLEMENTED; },
                    [](const_float_view cfv)    { Assert(false); return GRIB_INTERNAL_ERROR; },
                    [this](const_long_view clv) { return transform(clv, floats, context); },
                    [](const_char_view ccv)     { return GRIB_NOT_IMPLEMENTED; },
                },
                in_view
            );
        }

        return const_float_view{floats.ptr, floats.len};
    }

    const_long_view pack_buffer::to_longs()
    {
        if(std::holds_alternative<const_long_view>(in_view)) { return std::get<const_long_view>(in_view); }

        if(!longs.ptr) {
            status = std::visit(
                overload{
                    [](const_byte_view cbv)       { return GRIB_NOT_IMPLEMENTED; },
                    [this](const_double_view cdv) { return transform(cdv, longs, context);  },
                    [](const_float_view cfv)      { return GRIB_NOT_IMPLEMENTED; },
                    [](const_long_view clv)       { Assert(false); return GRIB_INTERNAL_ERROR; },
                    [this](const_char_view ccv)   { return transform(ccv, longs, context); },
                },
                in_view
            );
        }

        return const_long_view{longs.ptr, longs.len};
    }

    const_char_view pack_buffer::to_string()
    {
        if(std::holds_alternative<const_char_view>(in_view)) { return std::get<const_char_view>(in_view); }

        if(!chars.ptr) {
            status = std::visit(
                overload{
                    [](const_byte_view cbv)       { return GRIB_NOT_IMPLEMENTED; },
                    [this](const_double_view cdv) { return GRIB_NOT_IMPLEMENTED; },
                    [](const_float_view cfv)      { return GRIB_NOT_IMPLEMENTED; },
                    [this](const_long_view clv)   { return GRIB_NOT_IMPLEMENTED; },
                    [](const_char_view ccv)       { Assert(false); return GRIB_INTERNAL_ERROR; },
                },
                in_view
            );
        }

        return const_char_view{chars.ptr, chars.len};
    }

    void pack_buffer_test()
    {
        const std::size_t LEN{10};
        grib_context *context = grib_context_get_default();
        int status{GRIB_SUCCESS};

        // Check we can't pass a nullptr for input_values
#ifdef TEST_PACK_BUFFER_NULLPTR_ARGS_FAILS_TO_COMPILE
         pack_buffer test_buffer(context, nullptr); // Compile error - CORRECT!
#endif // TEST_PACK_BUFFER_NULLPTR_ARGS_FAILS_TO_COMPILE

#ifdef TEST_PACK_BUFFER_NULLPTR_ASSERTS
        pack_buffer null_buffer(context, const_long_view{nullptr, LEN});
#endif // TEST_PACK_BUFFER_NULLPTR_ASSERTS

        // Check malloc fails correctly for buffers that are too big!
#ifdef TEST_PACK_BUFFER_LARGE_BUFFER_MALLOC_ASSERTS
        long test_long{42};
        const std::size_t large_buffer_size_bytes = std::numeric_limits<std::size_t>::max() / sizeof(long);
        pack_buffer large_buffer(context, const_long_view{&test_long, large_buffer_size_bytes});
        auto test_d_ptr = large_buffer.to_doubles();
#endif // TEST_PACK_BUFFER_LARGE_BUFFER_MALLOC_ASSERTS

        // Long to double
        long long_buf[LEN] = {1,1,2,3,5,8,13,21,34,55};
        std::size_t long_len{LEN};
        pack_buffer pb_long(context, const_long_view{long_buf, &long_len});
        auto pb_long_to_doubles = pb_long.to_doubles();
        if(!pb_long_to_doubles.ptr)
        {
            status = pb_long.current_status();
        }

        // double to long
        double double_buf[LEN] = {0.5,1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9};
        std::size_t double_len{LEN};
        pack_buffer pb_double(context, const_double_view{double_buf, &double_len});
        auto pb_double_to_longs = pb_double.to_longs();
        if(!pb_double_to_longs.ptr)
        {
            status = pb_double.current_status();
        }

        // char to double and long
        char const pch_long[LEN] = "-3.141593";
        std::size_t char_len{LEN};
        pack_buffer pb_char(context, const_char_view{pch_long, &char_len});
        auto pb_char_as_doubles = pb_char.to_doubles();
        if(!pb_char_as_doubles.ptr)
        {
            status = pb_char.current_status();
        }
        auto pb_char_as_longs = pb_char.to_longs();
        if(!pb_char_as_longs.ptr)
        {
            status = pb_char.current_status();
        }
    }

 }
