/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

// The legacy implementation of the CCSDS packing: the packing parameters are computed as in
// legacy ecCodes and the data are coded with libaec. Without libaec only the parameters are
// computed and the coding is reported as not implemented (DataCcsdsPacking.cc selects the new
// implementation for all data SpAec supports).

#include "DataCcsdsPacking.h"

#include <cmath>
#include <cstring>

#if defined(HAVE_LIBAEC) || defined(HAVE_AEC)
    #include <libaec.h>
    #ifndef LIBAEC_DLL_EXPORTED
        #error Version of libaec appears to be too old. Please upgrade.
    #endif
#endif

namespace eccodes::accessor
{

#if defined(HAVE_LIBAEC) || defined(HAVE_AEC)
static bool is_big_endian()
{
    unsigned char is_big_endian = 0;
    unsigned short endianess_test = 1;
    return reinterpret_cast<const char*>(&endianess_test)[0] == is_big_endian;
}

static void modify_aec_flags(long* flags)
{
    // ECC-1602: Performance improvement: enabled the use of native data types
    *flags &= ~AEC_DATA_3BYTE;  // disable support for 3-bytes per value
    if (is_big_endian())
        *flags |= AEC_DATA_MSB;  // enable big-endian
    else
        *flags &= ~AEC_DATA_MSB;  // enable little-endian
}

static const char* aec_get_error_message(int code)
{
    if (code == AEC_MEM_ERROR) return "AEC_MEM_ERROR";
    if (code == AEC_DATA_ERROR) return "AEC_DATA_ERROR";
    if (code == AEC_STREAM_ERROR) return "AEC_STREAM_ERROR";
    if (code == AEC_CONF_ERROR) return "AEC_CONF_ERROR";
    if (code == AEC_OK) return "AEC_OK";
    return "Unknown error code";
}

static void print_aec_stream_info(struct aec_stream* strm, const char* func)
{
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.flags=%u\n", func, strm->flags);
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.bits_per_sample=%u\n", func, strm->bits_per_sample);
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.block_size=%u\n", func, strm->block_size);
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.rsi=%u\n", func, strm->rsi);
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.avail_out=%lu\n", func, strm->avail_out);
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.avail_in=%lu\n", func, strm->avail_in);
}
#endif /* HAVE_LIBAEC || HAVE_AEC */

#define MAX_BITS_PER_VALUE 32
int DataCcsdsPacking::pack_double_legacy(const double* val, size_t* len)
{
    grib_handle* hand = get_enclosing_handle();
    int err = GRIB_SUCCESS;
    size_t buflen = 0, i = 0;
    bool is_constant_field = false;

    unsigned char* buf = NULL;
    unsigned char* encoded = NULL;
    size_t n_vals = 0;

    long binary_scale_factor = 0;
    long decimal_scale_factor = 0;
    long optimize_scaling_factor = 0;
    double reference_value = 0;
    long bits_per_value = 0;
    double max, min, d, divisor;

    long number_of_data_points;

    long ccsds_flags;
    long ccsds_block_size;
    long ccsds_rsi;

    dirty_ = 1;

    n_vals = *len;

    if ((err = grib_get_long_internal(hand, bits_per_value_, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(hand, reference_value_, &reference_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, binary_scale_factor_, &binary_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, decimal_scale_factor_, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, optimize_scaling_factor_, &optimize_scaling_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, ccsds_flags_, &ccsds_flags)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, ccsds_block_size_, &ccsds_block_size)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, ccsds_rsi_, &ccsds_rsi)) != GRIB_SUCCESS)
        return err;

    // Special case
    if (*len == 0) {
        grib_buffer_replace(this, NULL, 0, 1, 1);
        return GRIB_SUCCESS;
    }

    max = val[0];
    min = max;
    for (i = 1; i < n_vals; i++) {
        if (val[i] > max)
            max = val[i];
        else if (val[i] < min)
            min = val[i];
    }

    if ((err = grib_check_data_values_minmax(hand, min, max)) != GRIB_SUCCESS) {
        return err;
    }

    if (min == max) {
        is_constant_field = true;
    }
    else {
        if (bits_per_value == 0) {
            // ECC-1202: A non-constant field with bitsPerValue==0!
            bits_per_value = 24;  // Set sane value
        }
    }

    if (is_constant_field) {
#ifdef DEBUG
        for (i = 1; i < n_vals; i++) {
            ECCODES_ASSERT(val[i] == val[0]);
        }
#endif
        if (grib_get_nearest_smaller_value(hand, reference_value_, val[0], &reference_value) != GRIB_SUCCESS) {
            grib_context_log(context_, GRIB_LOG_ERROR,
                             "%s %s: Unable to find nearest_smaller_value of %g for %s", accessor_type().c_str(), __func__, min, reference_value_);
            return GRIB_INTERNAL_ERROR;
        }
        if ((err = grib_set_double_internal(hand, reference_value_, reference_value)) != GRIB_SUCCESS)
            return err;

        if ((err = grib_set_long_internal(hand, number_of_values_, n_vals)) != GRIB_SUCCESS)
            return err;

        // ECC-2012
        if ((err = grib_set_long_internal(hand, binary_scale_factor_, 0)) != GRIB_SUCCESS)
            return err;

        bits_per_value = 0;  // ECC-1387
        if ((err = grib_set_long_internal(hand, bits_per_value_, bits_per_value)) != GRIB_SUCCESS)
            return err;

        grib_buffer_replace(this, NULL, 0, 1, 1);

        return GRIB_SUCCESS;
    }

    if ((err = grib_get_long_internal(hand, number_of_data_points_, &number_of_data_points)) != GRIB_SUCCESS)
        return err;

    if (bits_per_value == 0 || (binary_scale_factor == 0 && decimal_scale_factor != 0)) {
        d = codes_power<double>(decimal_scale_factor, 10);
        min *= d;
        max *= d;

        if (grib_get_nearest_smaller_value(hand, reference_value_, min, &reference_value) != GRIB_SUCCESS) {
            grib_context_log(context_, GRIB_LOG_ERROR,
                             "%s %s: Unable to find nearest_smaller_value of %g for %s", accessor_type().c_str(), __func__, min, reference_value_);
            return GRIB_INTERNAL_ERROR;
        }

        if (reference_value > min) {
            grib_context_log(context_, GRIB_LOG_ERROR,
                             "%s %s: reference_value=%g min_value=%g diff=%g", accessor_type().c_str(), __func__, reference_value, min, reference_value - min);
            DEBUG_ASSERT(reference_value <= min);
            return GRIB_INTERNAL_ERROR;
        }

        binary_scale_factor = grib_get_binary_scale_fact(max, reference_value, bits_per_value, &err);
        if (err) return err;
    }
    else if (optimize_scaling_factor) {
        if ((err = grib_optimize_decimal_factor(this, reference_value_,
                                                max, min, bits_per_value,
                                                0, 1,
                                                &decimal_scale_factor, &binary_scale_factor, &reference_value)) != GRIB_SUCCESS)
            return err;
        d = codes_power<double>(decimal_scale_factor, 10);
    }
    else {
        int last = 127;  // last must be a parameter coming from the def file
        double range = 0;
        double minrange = 0, maxrange = 0;
        double unscaled_max = 0;
        double unscaled_min = 0;
        double f = 0;
        double decimal = 1;

        decimal_scale_factor = 0;
        range = (max - min);
        unscaled_min = min;
        unscaled_max = max;
        f = (codes_power<double>(bits_per_value, 2) - 1);
        minrange = codes_power<double>(-last, 2) * f;
        maxrange = codes_power<double>(last, 2) * f;

        while (range < minrange) {
            decimal_scale_factor += 1;
            decimal *= 10;
            min = unscaled_min * decimal;
            max = unscaled_max * decimal;
            range = (max - min);
        }
        while (range > maxrange) {
            decimal_scale_factor -= 1;
            decimal /= 10;
            min = unscaled_min * decimal;
            max = unscaled_max * decimal;
            range = (max - min);
        }

        if (grib_get_nearest_smaller_value(hand, reference_value_, min, &reference_value) != GRIB_SUCCESS) {
            grib_context_log(context_, GRIB_LOG_ERROR,
                             "%s %s: Unable to find nearest_smaller_value of %g for %s", accessor_type().c_str(), __func__, min, reference_value_);
            return GRIB_INTERNAL_ERROR;
        }
        d = codes_power<double>(decimal_scale_factor, 10);

        binary_scale_factor = grib_get_binary_scale_fact(max, reference_value, bits_per_value, &err);
        if (err) return err;
    }

    divisor = codes_power<double>(-binary_scale_factor, 2);

    if ((err = grib_set_double_internal(hand, reference_value_, reference_value)) != GRIB_SUCCESS)
        return err;
    {
        // Make sure we can decode it again
        double ref = 1e-100;
        grib_get_double_internal(hand, reference_value_, &ref);
        if (ref != reference_value) {
            grib_context_log(context_, GRIB_LOG_ERROR, "%s %s: %s (ref=%.10e != reference_value=%.10e)",
                             accessor_type().c_str(), __func__, reference_value_, ref, reference_value);
            return GRIB_INTERNAL_ERROR;
        }
    }

    if ((err = grib_set_long_internal(hand, binary_scale_factor_, binary_scale_factor)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_set_long_internal(hand, decimal_scale_factor_, decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

#if defined(HAVE_LIBAEC) || defined(HAVE_AEC)
    {
        struct aec_stream strm;
        modify_aec_flags(&ccsds_flags);

        size_t nbytes = (bits_per_value + 7) / 8;
        // ECC-1602: use native a data type (4 bytes for uint32_t) for values that require only 3 bytes
        if (nbytes == 3)
            nbytes = 4;

        encoded = reinterpret_cast<unsigned char*>(grib_context_buffer_malloc_clear(context_, nbytes * n_vals));

        if (!encoded) {
            err = GRIB_OUT_OF_MEMORY;
            goto cleanup;
        }

        // ECC-1602: Performance improvement
        switch (nbytes) {
            case 1:
                for (i = 0; i < n_vals; i++) {
                    encoded[i] = static_cast<uint8_t>(((val[i] * d - reference_value) * divisor) + 0.5);
                }
                break;
            case 2:
                for (i = 0; i < n_vals; i++) {
                    reinterpret_cast<uint16_t*>(encoded)[i] = static_cast<uint16_t>(((val[i] * d - reference_value) * divisor) + 0.5);
                }
                break;
            case 4:
                for (i = 0; i < n_vals; i++) {
                    reinterpret_cast<uint32_t*>(encoded)[i] = static_cast<uint32_t>(((val[i] * d - reference_value) * divisor) + 0.5);
                }
                break;
            default:
                grib_context_log(context_, GRIB_LOG_ERROR, "%s pack_double: packing %s, bitsPerValue=%ld (max %d)",
                                 accessor_type().c_str(), name_, bits_per_value, MAX_BITS_PER_VALUE);
                err = GRIB_INVALID_BPV;
                goto cleanup;
        }

        grib_context_log(context_, GRIB_LOG_DEBUG, "%s pack_double: packing %s, %zu values (libaec)", accessor_type().c_str(), name_, n_vals);

        // ECC-1431: GRIB2: CCSDS encoding failure AEC_STREAM_ERROR
        buflen = (nbytes * n_vals) * 67 / 64 + 256;
        buf = (unsigned char*)grib_context_buffer_malloc_clear(context_, buflen);

        if (!buf) {
            err = GRIB_OUT_OF_MEMORY;
            goto cleanup;
        }

        strm.flags = ccsds_flags;
        strm.bits_per_sample = bits_per_value;
        strm.block_size = ccsds_block_size;
        strm.rsi = ccsds_rsi;

        strm.next_out = buf;
        strm.avail_out = buflen;
        strm.next_in = encoded;
        strm.avail_in = nbytes * n_vals;

        if (hand->context->debug) print_aec_stream_info(&strm, "pack_double");

        if ((err = aec_buffer_encode(&strm)) != AEC_OK) {
            grib_context_log(context_, GRIB_LOG_ERROR, "%s %s: aec_buffer_encode error %d (%s)",
                             accessor_type().c_str(), __func__, err, aec_get_error_message(err));
            err = GRIB_ENCODING_ERROR;
            goto cleanup;
        }

        buflen = strm.total_out;
        grib_buffer_replace(this, buf, buflen, 1, 1);

    cleanup:
        grib_context_buffer_free(context_, buf);
        grib_context_buffer_free(context_, encoded);

        if (err == GRIB_SUCCESS)
            err = grib_set_long_internal(hand, number_of_values_, *len);

        if (err == GRIB_SUCCESS)
            err = grib_set_long_internal(hand, bits_per_value_, strm.bits_per_sample);

        return err;
    }
#else
    // Without libaec, SpAec is the only implementation
    grib_context_log(context_, GRIB_LOG_ERROR,
                     "%s %s: ccsdsFlags=%ld, ccsdsBlockSize=%ld (signed, RSI padded or not preprocessed data, or block sizes "
                     "other than 8, 16, 32 and 64) are not supported by SpAec. "
                     "Please rebuild with -DENABLE_AEC=ON",
                     accessor_type().c_str(), __func__, ccsds_flags, ccsds_block_size);
    return GRIB_NOT_IMPLEMENTED;
#endif
}

template <typename T>
int DataCcsdsPacking::unpack_legacy(T* val, size_t* len)
{
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
    grib_handle* hand = get_enclosing_handle();

    int err = GRIB_SUCCESS;
    size_t buflen = 0;
    double bscale = 0;
    double dscale = 0;
    unsigned char* buf = NULL;
    size_t n_vals = 0;
    long nn = 0;

    long binary_scale_factor = 0;
    long decimal_scale_factor = 0;
    double reference_value = 0;
    long bits_per_value = 0;

    long ccsds_flags;
    long ccsds_block_size;
    long ccsds_rsi;

    dirty_ = 0;

    if ((err = value_count(&nn)) != GRIB_SUCCESS)
        return err;
    n_vals = nn;

    if ((err = grib_get_long_internal(hand, bits_per_value_, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(hand, reference_value_, &reference_value)) != GRIB_SUCCESS)
        return err;
    if (!std::isfinite(reference_value)) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s %s: referenceValue=%g is not finite (corrupt message)",
                         accessor_type().c_str(), __func__, reference_value);
        return GRIB_DECODING_ERROR;
    }
    if ((err = grib_get_long_internal(hand, binary_scale_factor_, &binary_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, decimal_scale_factor_, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

    // ECC-477: Don't call grib_get_long_internal to suppress error message being output
    if ((err = grib_get_long(hand, ccsds_flags_, &ccsds_flags)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(hand, ccsds_block_size_, &ccsds_block_size)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(hand, ccsds_rsi_, &ccsds_rsi)) != GRIB_SUCCESS)
        return err;

    // TODO(masn): This should be called upstream
    if (*len < n_vals)
        return GRIB_ARRAY_TOO_SMALL;

    // Special case
    if (bits_per_value == 0) {
        for (size_t i = 0; i < n_vals; i++)
            val[i] = reference_value;
        *len = n_vals;
        return GRIB_SUCCESS;
    }

    bscale = codes_power<T>(binary_scale_factor, 2);
    dscale = codes_power<T>(-decimal_scale_factor, 10);

    buflen = byte_count();
    buf = hand->buffer->data;
    buf += byte_offset();

    if (buflen == 0) {
        // An empty data section decodes as a constant field of the reference value (all samples
        // are 0), as ecCodes always did with libaec
        grib_context_log(context_, GRIB_LOG_DEBUG, "%s %s: empty data section, constant field of %zu values",
                         accessor_type().c_str(), __func__, n_vals);
        const T constant = static_cast<T>((0 * bscale + reference_value) * dscale);
        for (size_t i = 0; i < n_vals; i++)
            val[i] = constant;
        *len = n_vals;
        return GRIB_SUCCESS;
    }

#if defined(HAVE_LIBAEC) || defined(HAVE_AEC)
    {
        struct aec_stream strm;
        unsigned char* decoded = NULL;
        size_t nbytes, size;

        modify_aec_flags(&ccsds_flags);

        strm.flags = ccsds_flags;
        strm.bits_per_sample = bits_per_value;
        strm.block_size = ccsds_block_size;
        strm.rsi = ccsds_rsi;

        strm.next_in = buf;
        strm.avail_in = buflen;

        nbytes = (bits_per_value + 7) / 8;
        if (nbytes == 3)
            nbytes = 4;

        size = n_vals * nbytes;
        decoded = (unsigned char*)grib_context_buffer_malloc_clear(context_, size);
        if (!decoded) {
            return GRIB_OUT_OF_MEMORY;
        }
        strm.next_out = decoded;
        strm.avail_out = size;

        if (hand->context->debug) print_aec_stream_info(&strm, "unpack_*");

        if ((err = aec_buffer_decode(&strm)) != AEC_OK) {
            grib_context_log(context_, GRIB_LOG_ERROR, "%s %s: aec_buffer_decode error %d (%s)",
                             accessor_type().c_str(), __func__, err, aec_get_error_message(err));
            grib_context_buffer_free(context_, decoded);
            return GRIB_DECODING_ERROR;
        }
        // libaec does not report truncated data: it stops early without an error
        if (strm.total_out != size) {
            grib_context_log(context_, GRIB_LOG_ERROR,
                             "%s %s: CCSDS decoding failed: the compressed data is truncated or corrupt "
                             "(decoded %zu of %zu values from %zu bytes)",
                             accessor_type().c_str(), __func__, strm.total_out / nbytes, n_vals, buflen);
            grib_context_buffer_free(context_, decoded);
            return GRIB_DECODING_ERROR;
        }

        // ECC-1602: Performance improvement
        switch (nbytes) {
            case 1:
                for (size_t i = 0; i < n_vals; i++) {
                    val[i] = (reinterpret_cast<uint8_t*>(decoded)[i] * bscale + reference_value) * dscale;
                }
                break;
            case 2:
                for (size_t i = 0; i < n_vals; i++) {
                    val[i] = (reinterpret_cast<uint16_t*>(decoded)[i] * bscale + reference_value) * dscale;
                }
                break;
            case 4:
                for (size_t i = 0; i < n_vals; i++) {
                    val[i] = (reinterpret_cast<uint32_t*>(decoded)[i] * bscale + reference_value) * dscale;
                }
                break;
            default:
                grib_context_log(context_, GRIB_LOG_ERROR, "%s %s: unpacking %s, bitsPerValue=%ld (max %d)",
                                 accessor_type().c_str(), __func__, name_, bits_per_value, MAX_BITS_PER_VALUE);
                grib_context_buffer_free(context_, decoded);
                return GRIB_INVALID_BPV;
        }

        *len = n_vals;
        grib_context_buffer_free(context_, decoded);
        return GRIB_SUCCESS;
    }
#else
    // Without libaec, SpAec is the only implementation
    grib_context_log(context_, GRIB_LOG_ERROR,
                     "%s %s: ccsdsFlags=%ld, ccsdsBlockSize=%ld (signed, RSI padded or not preprocessed data, or block sizes "
                     "other than 8, 16, 32 and 64) are not supported by SpAec. "
                     "Please rebuild with -DENABLE_AEC=ON",
                     accessor_type().c_str(), __func__, ccsds_flags, ccsds_block_size);
    return GRIB_NOT_IMPLEMENTED;
#endif
}

template int DataCcsdsPacking::unpack_legacy<double>(double* val, size_t* len);
template int DataCcsdsPacking::unpack_legacy<float>(float* val, size_t* len);

}  // namespace eccodes::accessor
