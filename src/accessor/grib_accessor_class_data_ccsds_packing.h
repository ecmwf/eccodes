
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_data_ccsds_packing_h
#define eccodes_accessor_data_ccsds_packing_h

#include "grib_accessor_class_values.h"
#include "grib_scaling.h"

#if defined(HAVE_LIBAEC) || defined(HAVE_AEC)
#include <libaec.h>
#endif

#ifndef LIBAEC_DLL_EXPORTED
    #error Version of libaec appears to be too old. Please upgrade.
#endif

class grib_accessor_data_ccsds_packing_t : public grib_accessor_values_t
{
public:
    /* Members defined in data_ccsds_packing */
    const char*   number_of_values;
    const char*   reference_value;
    const char*   binary_scale_factor;
    const char*   decimal_scale_factor;
    const char*   optimize_scaling_factor;
    const char*   bits_per_value;
    const char*   number_of_data_points;
    const char*   ccsds_flags;
    const char*   ccsds_block_size;
    const char*   ccsds_rsi;
};



// TODO(maee): if libaec is not available, this class should not be defined
class grib_accessor_class_data_ccsds_packing_t : public grib_accessor_class_values_t
{
public:
    grib_accessor_class_data_ccsds_packing_t(const char* name) : grib_accessor_class_values_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_data_ccsds_packing_t{}; }
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_float(grib_accessor*, float* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    int unpack_double_element(grib_accessor*, size_t i, double* val) override;
    int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array) override;

private:
    template <typename T> int unpack(grib_accessor*, T*, size_t* );
    void print_aec_stream_info(struct aec_stream*, const char*);
    void modify_aec_flags(long*);
    const char* aec_get_error_message(int);
    bool is_big_endian();
};


template <typename T>
int grib_accessor_class_data_ccsds_packing_t::unpack(grib_accessor* a, T* val, size_t* len){
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
    grib_accessor_data_ccsds_packing_t* self = (grib_accessor_data_ccsds_packing_t*)a;
    grib_handle* hand       = grib_handle_of_accessor(a);
    const char* cclass_name = a->cclass->name;

    int err = GRIB_SUCCESS, i = 0;
    size_t buflen = 0;
    struct aec_stream strm;
    double bscale      = 0;
    double dscale      = 0;
    unsigned char* buf = NULL;
    size_t n_vals      = 0;
    size_t size        = 0;
    unsigned char* decoded = NULL;
    // unsigned char* p       = NULL;
    long nn                = 0;

    long binary_scale_factor  = 0;
    long decimal_scale_factor = 0;
    double reference_value    = 0;
    long bits_per_value       = 0;

    long ccsds_flags;
    long ccsds_block_size;
    long ccsds_rsi;
    size_t nbytes;

    self->dirty = 0;

    if ((err = a->value_count(&nn)) != GRIB_SUCCESS)
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

    // ECC-477: Don't call grib_get_long_internal to suppress error message being output
    if ((err = grib_get_long(hand, self->ccsds_flags, &ccsds_flags)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(hand, self->ccsds_block_size, &ccsds_block_size)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, self->ccsds_rsi, &ccsds_rsi)) != GRIB_SUCCESS)
        return err;

    modify_aec_flags(&ccsds_flags);

    // TODO(masn): This should be called upstream
    if (*len < n_vals)
        return GRIB_ARRAY_TOO_SMALL;

    // Special case
    if (bits_per_value == 0) {
        for (i = 0; i < n_vals; i++)
            val[i] = reference_value;
        *len = n_vals;
        return GRIB_SUCCESS;
    }

    bscale = codes_power<T>(binary_scale_factor, 2);
    dscale = codes_power<T>(-decimal_scale_factor, 10);

    buflen = a->byte_count();    buf = (unsigned char*)hand->buffer->data;
    buf += a->byte_offset();
    strm.flags           = ccsds_flags;
    strm.bits_per_sample = bits_per_value;
    strm.block_size      = ccsds_block_size;
    strm.rsi             = ccsds_rsi;

    strm.next_in  = buf;
    strm.avail_in = buflen;

    nbytes = (bits_per_value + 7) / 8;
    if (nbytes == 3)
        nbytes = 4;

    size    = n_vals * nbytes;
    decoded = (unsigned char*)grib_context_buffer_malloc_clear(a->context, size);
    if (!decoded) {
        err = GRIB_OUT_OF_MEMORY;
        goto cleanup;
    }
    strm.next_out  = decoded;
    strm.avail_out = size;

    if (hand->context->debug) print_aec_stream_info(&strm, "unpack_*");

    if ((err = aec_buffer_decode(&strm)) != AEC_OK) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "%s %s: aec_buffer_decode error %d (%s)",
                       cclass_name, __func__, err, aec_get_error_message(err));
        err = GRIB_DECODING_ERROR;
        goto cleanup;
    }

    // ECC-1427: Performance improvement (replaced by ECC-1602)
    //grib_decode_array<T>(decoded, &pos, bits8 , reference_value, bscale, dscale, n_vals, val);

    // ECC-1602: Performance improvement
    switch (nbytes) {
        case 1:
            for (i = 0; i < n_vals; i++) {
                val[i] = (reinterpret_cast<uint8_t*>(decoded)[i] * bscale + reference_value) * dscale;
            }
            break;
        case 2:
            for (i = 0; i < n_vals; i++) {
                val[i] = (reinterpret_cast<uint16_t*>(decoded)[i] * bscale + reference_value) * dscale;
            }
            break;
        case 4:
            for (i = 0; i < n_vals; i++) {
                val[i] = (reinterpret_cast<uint32_t*>(decoded)[i] * bscale + reference_value) * dscale;
            }
            break;
        default:
            grib_context_log(a->context, GRIB_LOG_ERROR, "%s %s: unpacking %s, bits_per_value=%ld (max 32)",
               cclass_name, __func__, a->name, bits_per_value);
            err = GRIB_INVALID_BPV;
            goto cleanup;
    }

    *len = n_vals;

cleanup:
    grib_context_buffer_free(a->context, decoded);
    return err;
}

#endif /* eccodes_accessor_data_ccsds_packing_h */
