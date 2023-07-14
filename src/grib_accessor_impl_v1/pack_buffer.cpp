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

    template<typename OUT_VIEW>
    int init_view(OUT_VIEW& out_view, std::size_t out_buffer_size_bytes, grib_context *c)
    {
        static_assert(is_variant_member<OUT_VIEW, view_type>::value,
            "OUT_VIEW must be one of the types defined in const_view_type");

        Assert(!out_view.ptr);

        using out_type = typename OUT_VIEW::type;

        if(out_view.ptr = (out_type*)grib_context_malloc(c, out_buffer_size_bytes); !out_view.ptr)
        {
            grib_context_log(c, GRIB_LOG_ERROR, "Unable to allocate %ld bytes\n", out_buffer_size_bytes);
            return GRIB_OUT_OF_MEMORY;
        }

        std::size_t len_buffer_size_bytes = sizeof(std::size_t);
        if(out_view.len = (std::size_t*)grib_context_malloc(c, len_buffer_size_bytes); !out_view.len)
        {
            grib_context_log(c, GRIB_LOG_ERROR, "Unable to allocate %ld bytes\n", len_buffer_size_bytes);
            grib_context_free(c, out_view.ptr);
            return GRIB_OUT_OF_MEMORY;
        }

        return GRIB_SUCCESS;
    }

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

        if(int ret = init_view(out_view, out_buffer_size_bytes, c); ret != GRIB_SUCCESS) { return ret;}

        for(std::size_t index = 0; index < *in_view.len; ++index)
        {
            out_view.ptr[index] = static_cast<out_type>(in_view.ptr[index]);
        }

        *out_view.len = *in_view.len;

        return GRIB_SUCCESS;
    }

    // transform() overload for string to double
    // Note: we don't specialise the template: https://www.modernescpp.com/index.php/full-specialization-of-function-templates
    int transform(const_char_view const& in_view, double_view& out_view, grib_context *c)
    {
        Assert(in_view.ptr && !out_view.ptr);

        std::size_t dlen = 1;
        std::size_t out_buffer_size_bytes = dlen*sizeof(double);

        if(int ret = init_view(out_view, out_buffer_size_bytes, c); ret != GRIB_SUCCESS) { return ret;}

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

        if(int ret = init_view(out_view, out_buffer_size_bytes, c); ret != GRIB_SUCCESS) { return ret;}

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
        if(bytes.len)   { grib_context_free(context, bytes.len); }
        if(doubles.ptr) { grib_context_free(context, doubles.ptr); }
        if(doubles.len) { grib_context_free(context, doubles.len); }
        if(floats.ptr)  { grib_context_free(context, floats.ptr); }
        if(floats.len)  { grib_context_free(context, floats.len); }
        if(longs.ptr)   { grib_context_free(context, longs.ptr); }
        if(longs.len)   { grib_context_free(context, longs.len); }
        if(chars.ptr)   { grib_context_free(context, chars.ptr); }
        if(chars.len)   { grib_context_free(context, chars.len); }
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

}
