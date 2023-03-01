#pragma once

#include "grib_api_internal_cpp.h"
#include <type_traits>

#if defined(HAVE_LIBAEC) || defined(HAVE_AEC)
#include <libaec.h>
const char* aec_get_error_message(int code);
void print_aec_stream_info(struct aec_stream* strm, const char* func);
#endif

void print_error_feature_not_enabled(grib_context* c);

typedef struct grib_accessor_data_ccsds_packing
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in values */
    int  carg;
    const char* seclen;
    const char* offsetdata;
    const char* offsetsection;
    int dirty;
    /* Members defined in data_ccsds_packing */
    const char*   number_of_values;
    const char*   reference_value;
    const char*   binary_scale_factor;
    const char*   decimal_scale_factor;
    const char*   bits_per_value;
    const char*   number_of_data_points;
    const char*   ccsds_flags;
    const char*   ccsds_block_size;
    const char*   ccsds_rsi;
} grib_accessor_data_ccsds_packing;


template <typename T>
class GribAccessorDataCcsdsPacking {
public:
    static int unpack(grib_accessor* a, T* val, size_t* len);
};

#if defined(HAVE_LIBAEC) || defined(HAVE_AEC)
template <typename T>
int GribAccessorDataCcsdsPacking<T>::unpack(grib_accessor* a, T* val, size_t* len)
{
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
    grib_accessor_data_ccsds_packing* self = (grib_accessor_data_ccsds_packing*)a;
    grib_handle* hand = grib_handle_of_accessor(a);

    int err = GRIB_SUCCESS, i = 0;
    size_t buflen = 0;
    struct aec_stream strm;
    double bscale      = 0;
    double dscale      = 0;
    unsigned char* buf = NULL;
    size_t n_vals      = 0;
    size_t size        = 0;
    unsigned char* decoded = NULL;
    /*unsigned char* p       = NULL;*/
    long pos               = 0;
    long nn                = 0;

    long binary_scale_factor  = 0;
    long decimal_scale_factor = 0;
    double reference_value    = 0;
    long bits_per_value       = 0;
    long bits8;

    long ccsds_flags;
    long ccsds_block_size;
    long ccsds_rsi;

    self->dirty = 0;

    if ((err = grib_value_count(a, &nn)) != GRIB_SUCCESS)
        return err;
    n_vals = nn;

    if ((err = grib_get_long_internal(hand, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(hand, self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

    /* ECC-477: Don't call grib_get_long_internal to suppress error message being output */
    if ((err = grib_get_long(hand, self->ccsds_flags, &ccsds_flags)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(hand, self->ccsds_block_size, &ccsds_block_size)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, self->ccsds_rsi, &ccsds_rsi)) != GRIB_SUCCESS)
        return err;

    /* TODO: This should be called upstream */
    if (*len < n_vals)
        return GRIB_ARRAY_TOO_SMALL;

    /* Special case */
    if (bits_per_value == 0) {
        for (i = 0; i < n_vals; i++)
            val[i] = reference_value;
        *len = n_vals;
        return GRIB_SUCCESS;
    }

    bscale = grib_power(binary_scale_factor, 2);
    dscale = grib_power(-decimal_scale_factor, 10);

    buflen = grib_byte_count(a);
    buf = (unsigned char*)hand->buffer->data;
    buf += grib_byte_offset(a);

strm.flags           = ccsds_flags;
strm.bits_per_sample = bits_per_value;
strm.block_size      = ccsds_block_size;
strm.rsi             = ccsds_rsi;

strm.next_in  = buf;
strm.avail_in = buflen;

    bits8   = ((bits_per_value + 7) / 8) * 8;
    size    = n_vals * ((bits_per_value + 7) / 8);
    decoded = (unsigned char*)grib_context_buffer_malloc_clear(a->context, size);
    if (!decoded) {
        err = GRIB_OUT_OF_MEMORY;
        goto cleanup;
    }
    strm.next_out  = decoded;
    strm.avail_out = size;

    if (hand->context->debug) print_aec_stream_info(&strm, "unpack_*");

    if ((err = aec_buffer_decode(&strm)) != AEC_OK) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "CCSDS %s: aec_buffer_decode error %d (%s)\n",
                         __PRETTY_FUNCTION__, err, aec_get_error_message(err));
        err = GRIB_ENCODING_ERROR;
        goto cleanup;
}

pos = 0;

/* ECC-1427: Performance improvement */
//grib_decode_float_array(decoded, &pos, bits8 , reference_value, bscale, dscale, n_vals, val);
    grib_decode_array<T>(decoded, &pos, bits8 , reference_value, bscale, dscale, n_vals, val);
    *len = n_vals;

cleanup:
    grib_context_buffer_free(a->context, decoded);
    return err;
}

#else
template <typename T>
int GribAccessorDataCcsdsPacking<T>::unpack(grib_accessor* a, T* val, size_t* len)
{
    print_error_feature_not_enabled(a->context);
    return GRIB_FUNCTIONALITY_NOT_ENABLED;
}
#endif
