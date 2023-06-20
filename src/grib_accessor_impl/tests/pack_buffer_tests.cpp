#include "pack_buffer_tests.h"
#include "../pack_buffer.h"
#include "grib_api_internal.h"

namespace eccodes {  

    void pack_buffer_test()
    {
        const std::size_t LEN{10};
        grib_context *context = grib_context_get_default();
        int status{GRIB_SUCCESS};

        // Check we can't pass a nullptr for input_values
//#define TEST_PACK_BUFFER_NULLPTR_ARGS_FAILS_TO_COMPILE
#ifdef TEST_PACK_BUFFER_NULLPTR_ARGS_FAILS_TO_COMPILE
         pack_buffer test_buffer(context, nullptr); // Compile error - CORRECT!
#endif // TEST_PACK_BUFFER_NULLPTR_ARGS_FAILS_TO_COMPILE

//#define TEST_PACK_BUFFER_NULLPTR_ASSERTS
#ifdef TEST_PACK_BUFFER_NULLPTR_ASSERTS
        std::size_t null_len{LEN};
        pack_buffer null_buffer(context, const_long_view{nullptr, &null_len});
#endif // TEST_PACK_BUFFER_NULLPTR_ASSERTS

        // Check malloc fails correctly for buffers that are too big!
//#define TEST_PACK_BUFFER_LARGE_BUFFER_MALLOC_ASSERTS
#ifdef TEST_PACK_BUFFER_LARGE_BUFFER_MALLOC_ASSERTS
        long test_long{42};
        std::size_t large_buffer_size_bytes = std::numeric_limits<std::size_t>::max() / sizeof(long);
        pack_buffer large_buffer(context, const_long_view{&test_long, &large_buffer_size_bytes});
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
