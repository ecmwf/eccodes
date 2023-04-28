/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal_cpp.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_values
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double
   IMPLEMENTS = unpack_float
   IMPLEMENTS = pack_double
   IMPLEMENTS = unpack_double_element;unpack_double_element_set
   IMPLEMENTS = value_count
   MEMBERS=const char*   number_of_values
   MEMBERS=const char*   reference_value
   MEMBERS=const char*   binary_scale_factor
   MEMBERS=const char*   decimal_scale_factor
   MEMBERS=const char*   bits_per_value

   MEMBERS=const char*   number_of_data_points

   MEMBERS=const char*   ccsds_flags
   MEMBERS=const char*   ccsds_block_size
   MEMBERS=const char*   ccsds_rsi

   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_float(grib_accessor*, float* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);
static int unpack_double_element(grib_accessor*, size_t i, double* val);
static int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array);

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

extern grib_accessor_class* grib_accessor_class_values;

static grib_accessor_class _grib_accessor_class_data_ccsds_packing = {
    &grib_accessor_class_values,                      /* super */
    "data_ccsds_packing",                      /* name */
    sizeof(grib_accessor_data_ccsds_packing),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    0,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    0,                  /* pack_long */
    0,                /* unpack_long */
    &pack_double,                /* pack_double */
    0,                 /* pack_float */
    &unpack_double,              /* unpack_double */
    &unpack_float,               /* unpack_float */
    0,                /* pack_string */
    0,              /* unpack_string */
    0,          /* pack_string_array */
    0,        /* unpack_string_array */
    0,                 /* pack_bytes */
    0,               /* unpack_bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    &unpack_double_element,      /* unpack only ith value (double) */
    0,       /* unpack only ith value (float) */
    &unpack_double_element_set,  /* unpack a given set of elements (double) */
    0,   /* unpack a given set of elements (float) */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_data_ccsds_packing = &_grib_accessor_class_data_ccsds_packing;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long v, grib_arguments* args)
{
    grib_accessor_data_ccsds_packing* self = (grib_accessor_data_ccsds_packing*)a;
    grib_handle* h = grib_handle_of_accessor(a);

    self->number_of_values     = grib_arguments_get_name(h, args, self->carg++);
    self->reference_value      = grib_arguments_get_name(h, args, self->carg++);
    self->binary_scale_factor  = grib_arguments_get_name(h, args, self->carg++);
    self->decimal_scale_factor = grib_arguments_get_name(h, args, self->carg++);
    self->bits_per_value       = grib_arguments_get_name(h, args, self->carg++);
    self->number_of_data_points = grib_arguments_get_name(h, args, self->carg++);
    self->ccsds_flags      = grib_arguments_get_name(h, args, self->carg++);
    self->ccsds_block_size = grib_arguments_get_name(h, args, self->carg++);
    self->ccsds_rsi        = grib_arguments_get_name(h, args, self->carg++);

    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

static int value_count(grib_accessor* a, long* count)
{
    grib_accessor_data_ccsds_packing* self = (grib_accessor_data_ccsds_packing*)a;
    *count = 0;
    return grib_get_long_internal(grib_handle_of_accessor(a), self->number_of_values, count);
}

#if defined(HAVE_LIBAEC) || defined(HAVE_AEC)

#include <libaec.h>

#ifndef LIBAEC_DLL_EXPORTED
    #error Version of libaec appears to be too old. Please upgrade.
#endif

const char* aec_get_error_message(int code)
{
    if (code == AEC_MEM_ERROR)    return "AEC_MEM_ERROR";
    if (code == AEC_DATA_ERROR)   return "AEC_DATA_ERROR";
    if (code == AEC_STREAM_ERROR) return "AEC_STREAM_ERROR";
    if (code == AEC_CONF_ERROR)   return "AEC_CONF_ERROR";
    if (code == AEC_OK)           return "AEC_OK";
    return "Unknown error code";
}

void print_aec_stream_info(struct aec_stream* strm, const char* func)
{
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.flags=%u\n",           func, strm->flags);
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.bits_per_sample=%u\n", func, strm->bits_per_sample);
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.block_size=%u\n",      func, strm->block_size);
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.rsi=%u\n",             func, strm->rsi);
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.avail_out=%lu\n",      func, strm->avail_out);
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.avail_in=%lu\n",       func, strm->avail_in);
}

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_data_ccsds_packing* self = (grib_accessor_data_ccsds_packing*)a;

    grib_handle* hand = grib_handle_of_accessor(a);
    int err = GRIB_SUCCESS;
    size_t buflen = 0, i = 0;
    bool is_constant_field = false;

    unsigned char* buf     = NULL;
    unsigned char* encoded = NULL;
    size_t n_vals          = 0;

    long binary_scale_factor  = 0;
    long decimal_scale_factor = 0;
    double reference_value    = 0;
    long bits8                = 0;
    long bits_per_value       = 0;
    double max, min, d, divisor;

    unsigned char* p;
    long number_of_data_points;

    long ccsds_flags;
    long ccsds_block_size;
    long ccsds_rsi;

    struct aec_stream strm;

    self->dirty = 1;

    n_vals = *len;

    if ((err = grib_get_long_internal(hand, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(hand, self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(hand, self->ccsds_flags, &ccsds_flags)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, self->ccsds_block_size, &ccsds_block_size)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, self->ccsds_rsi, &ccsds_rsi)) != GRIB_SUCCESS)
        return err;

    // Special case
    if (*len == 0) {
        grib_buffer_replace(a, NULL, 0, 1, 1);
        return GRIB_SUCCESS;
    }

    max = val[0];
    min = max;
    for (i = 1; i < n_vals; i++) {
        if (val[i] > max)      max = val[i];
        else if (val[i] < min) min = val[i];
    }

    if ((err = grib_check_data_values_range(hand, min, max)) != GRIB_SUCCESS) {
        return err;
    }

    if (min == max) {
        is_constant_field = true;
    } else {
        if (bits_per_value == 0) {
            // ECC-1202: A non-constant field with bitsPerValue==0!
            bits_per_value = 24; //Set sane value
        }
    }

    if (is_constant_field) {
#ifdef DEBUG
        for (i = 1; i < n_vals; i++) {
            Assert(val[i] == val[0]);
        }
#endif
        if (grib_get_nearest_smaller_value(hand, self->reference_value, val[0], &reference_value) != GRIB_SUCCESS) {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                             "data_ccsds_packing %s: unable to find nearest_smaller_value of %g for %s", __func__, min, self->reference_value);
            return GRIB_INTERNAL_ERROR;
        }
        if ((err = grib_set_double_internal(hand, self->reference_value, reference_value)) != GRIB_SUCCESS)
            return err;

        if ((err = grib_set_long_internal(hand, self->number_of_values, n_vals)) != GRIB_SUCCESS)
            return err;
        bits_per_value = 0; // ECC-1387
        if ((err = grib_set_long_internal(hand, self->bits_per_value, bits_per_value)) != GRIB_SUCCESS)
            return err;

        grib_buffer_replace(a, NULL, 0, 1, 1);

        return GRIB_SUCCESS;
    }

    if ((err = grib_get_long_internal(hand, self->number_of_data_points, &number_of_data_points)) != GRIB_SUCCESS)
        return err;

    if (bits_per_value == 0 || (binary_scale_factor == 0 && decimal_scale_factor != 0)) {
        d = grib_power(decimal_scale_factor, 10);
        min *= d;
        max *= d;

        if (grib_get_nearest_smaller_value(hand, self->reference_value, min, &reference_value) != GRIB_SUCCESS) {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                "data_ccsds_packing %s: unable to find nearest_smaller_value of %g for %s", __func__, min, self->reference_value);
            return GRIB_INTERNAL_ERROR;
        }

        if (reference_value > min) {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                "data_ccsds_packing %s: reference_value=%g min_value=%g diff=%g", __func__, reference_value, min, reference_value - min);
            DebugAssert(reference_value <= min);
            return GRIB_INTERNAL_ERROR;
        }
    }
    else {
        int last = 127;  // last must be a parameter coming from the def file
        double range = 0;
        double minrange = 0, maxrange = 0;
        double unscaled_max = 0;
        double unscaled_min = 0;
        double f = 0;
        double decimal = 1;

        range        = (max - min);
        unscaled_min = min;
        unscaled_max = max;
        f            = (grib_power(bits_per_value, 2) - 1);
        minrange     = grib_power(-last, 2) * f;
        maxrange     = grib_power(last, 2) * f;

        while (range < minrange) {
            decimal_scale_factor += 1;
            decimal *= 10;
            min   = unscaled_min * decimal;
            max   = unscaled_max * decimal;
            range = (max - min);
        }
        while (range > maxrange) {
            decimal_scale_factor -= 1;
            decimal /= 10;
            min   = unscaled_min * decimal;
            max   = unscaled_max * decimal;
            range = (max - min);
        }
        if (grib_get_nearest_smaller_value(hand, self->reference_value, min, &reference_value) != GRIB_SUCCESS) {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                             "data_ccsds_packing %s: unable to find nearest_smaller_value of %g for %s", __func__, min, self->reference_value);
            return GRIB_INTERNAL_ERROR;
        }
        d = grib_power(decimal_scale_factor, 10);
    }

    binary_scale_factor = grib_get_binary_scale_fact(max, reference_value, bits_per_value, &err);
    divisor             = grib_power(-binary_scale_factor, 2);

    bits8   = (bits_per_value + 7) / 8 * 8;
    encoded = (unsigned char*)grib_context_buffer_malloc_clear(a->context, bits8 / 8 * n_vals);

    if (!encoded) {
        err = GRIB_OUT_OF_MEMORY;
        goto cleanup;
    }

    buflen = 0;
    p      = encoded;
    for (i = 0; i < n_vals; i++) {
        long blen                  = bits8;
        unsigned long unsigned_val = (unsigned long)((((val[i] * d) - reference_value) * divisor) + 0.5);
        while (blen >= 8) {
            blen -= 8;
            *p = (unsigned_val >> blen);
            p++;
            buflen++;
        }
    }
    // buflen = n_vals*(bits_per_value/8);

    grib_context_log(a->context, GRIB_LOG_DEBUG,"data_ccsds_packing pack_double: packing %s, %d values", a->name, n_vals);

    // ECC-1431: GRIB2: CCSDS encoding failure AEC_STREAM_ERROR
    buflen += buflen / 20 + 256;
    buf = (unsigned char*)grib_context_buffer_malloc_clear(a->context, buflen);

    if (!buf) {
        err = GRIB_OUT_OF_MEMORY;
        goto cleanup;
    }

    if ((err = grib_set_double_internal(hand, self->reference_value, reference_value)) != GRIB_SUCCESS)
        return err;
    {
        // Make sure we can decode it again
        double ref = 1e-100;
        grib_get_double_internal(hand, self->reference_value, &ref);
        if (ref != reference_value) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "data_ccsds_packing %s: %s (ref=%.10e != reference_value=%.10e)",
                            __func__, self->reference_value, ref, reference_value);
            return GRIB_INTERNAL_ERROR;
        }
    }

    if ((err = grib_set_long_internal(hand, self->binary_scale_factor, binary_scale_factor)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_set_long_internal(hand, self->decimal_scale_factor, decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

    strm.flags           = ccsds_flags;
    strm.bits_per_sample = bits_per_value;
    strm.block_size      = ccsds_block_size;
    strm.rsi             = ccsds_rsi;

    strm.next_out  = buf;
    strm.avail_out = buflen;
    strm.next_in   = encoded;
    strm.avail_in  = bits8 / 8 * n_vals;

    // This does not support spherical harmonics, and treats 24 differently than:
    // see http://cdo.sourcearchive.com/documentation/1.5.1.dfsg.1-1/cgribexlib_8c_source.html

    if (hand->context->debug) print_aec_stream_info(&strm, "pack_double");

    if ((err = aec_buffer_encode(&strm)) != AEC_OK) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "data_ccsds_packing %s: aec_buffer_encode error %d (%s)",
                         __func__, err, aec_get_error_message(err));
        err = GRIB_ENCODING_ERROR;
        goto cleanup;
    }

    //printf("n_vals = %ld, bits8 = %ld\n", n_vals, bits8);
    //printf("in %ld out => %zu\n", bits8/8*n_vals, buflen);
    buflen = strm.total_out;
    grib_buffer_replace(a, buf, buflen, 1, 1);

cleanup:
    grib_context_buffer_free(a->context, buf);
    grib_context_buffer_free(a->context, encoded);

    if (err == GRIB_SUCCESS)
        err = grib_set_long_internal(hand, self->number_of_values, *len);

    if (err == GRIB_SUCCESS)
        err = grib_set_long_internal(hand, self->bits_per_value, strm.bits_per_sample);

    return err;
}

template <typename T>
static int unpack(grib_accessor* a, T* val, size_t* len)
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
    // unsigned char* p       = NULL;
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

    // ECC-477: Don't call grib_get_long_internal to suppress error message being output
    if ((err = grib_get_long(hand, self->ccsds_flags, &ccsds_flags)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(hand, self->ccsds_block_size, &ccsds_block_size)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, self->ccsds_rsi, &ccsds_rsi)) != GRIB_SUCCESS)
        return err;

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
        grib_context_log(a->context, GRIB_LOG_ERROR, "CCSDS %s: aec_buffer_decode error %d (%s)",
                         __func__, err, aec_get_error_message(err));
        err = GRIB_DECODING_ERROR;
        goto cleanup;
    }

    pos = 0;

    // ECC-1427: Performance improvement
    //grib_decode_float_array(decoded, &pos, bits8 , reference_value, bscale, dscale, n_vals, val);
    grib_decode_array<T>(decoded, &pos, bits8 , reference_value, bscale, dscale, n_vals, val);
    *len = n_vals;

cleanup:
    grib_context_buffer_free(a->context, decoded);
    return err;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    return unpack<double>(a, val, len);
}

static int unpack_float(grib_accessor* a, float* val, size_t* len)
{
    return unpack<float>(a, val, len);
}

static int unpack_double_element(grib_accessor* a, size_t idx, double* val)
{
    // The index idx relates to codedValues NOT values!
    grib_accessor_data_ccsds_packing* self = (grib_accessor_data_ccsds_packing*)a;
    grib_handle* hand = grib_handle_of_accessor(a);
    int err = 0;
    size_t size    = 0;
    long bits_per_value = 0;
    double reference_value = 0;
    double* values = NULL;

    if ((err = grib_get_long_internal(hand, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(hand, self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;

    // Special case of constant field
    if (bits_per_value == 0) {
        *val = reference_value;
        return GRIB_SUCCESS;
    }

    err = grib_get_size(hand, "codedValues", &size);
    if (err) return err;
    if (idx > size) return GRIB_INVALID_ARGUMENT;

    values = (double*)grib_context_malloc_clear(a->context, size * sizeof(double));
    err    = grib_get_double_array(hand, "codedValues", values, &size);
    if (err) {
        grib_context_free(a->context, values);
        return err;
    }
    *val = values[idx];
    grib_context_free(a->context, values);
    return GRIB_SUCCESS;
}

static int unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array)
{
    grib_accessor_data_ccsds_packing* self = (grib_accessor_data_ccsds_packing*)a;
    grib_handle* hand = grib_handle_of_accessor(a);
    size_t size = 0, i = 0;
    double* values = NULL;
    int err = 0;
    long bits_per_value = 0;
    double reference_value = 0;

    if ((err = grib_get_long_internal(hand, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(hand, self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;

    // Special case of constant field
    if (bits_per_value == 0) {
        for (i = 0; i < len; i++) {
            val_array[i] = reference_value;
        }
        return GRIB_SUCCESS;
    }

    // GRIB-564: The indexes in index_array relate to codedValues NOT values!
    err = grib_get_size(grib_handle_of_accessor(a), "codedValues", &size);
    if (err)
        return err;

    for (i = 0; i < len; i++) {
        if (index_array[i] > size) return GRIB_INVALID_ARGUMENT;
    }

    values = (double*)grib_context_malloc_clear(a->context, size * sizeof(double));
    err    = grib_get_double_array(grib_handle_of_accessor(a), "codedValues", values, &size);
    if (err) {
        grib_context_free(a->context, values);
        return err;
    }
    for (i = 0; i < len; i++) {
        val_array[i] = values[index_array[i]];
    }

    grib_context_free(a->context, values);
    return GRIB_SUCCESS;
}

#else

static void print_error_feature_not_enabled(grib_context* c)
{
    grib_context_log(c, GRIB_LOG_ERROR,
                     "CCSDS support not enabled. "
                     "Please rebuild with -DENABLE_AEC=ON (Adaptive Entropy Coding library)");
}
static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    print_error_feature_not_enabled(a->context);
    return GRIB_FUNCTIONALITY_NOT_ENABLED;
}
static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    print_error_feature_not_enabled(a->context);
    return GRIB_FUNCTIONALITY_NOT_ENABLED;
}
static int unpack_float(grib_accessor* a, float* val, size_t* len)
{
    print_error_feature_not_enabled(a->context);
    return GRIB_FUNCTIONALITY_NOT_ENABLED;
}
static int unpack_double_element(grib_accessor* a, size_t idx, double* val)
{
    print_error_feature_not_enabled(a->context);
    return GRIB_FUNCTIONALITY_NOT_ENABLED;
}
static int unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array)
{
    print_error_feature_not_enabled(a->context);
    return GRIB_FUNCTIONALITY_NOT_ENABLED;
}

#endif
