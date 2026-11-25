/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

// The CCSDS packing (GRIB2 data representation template 5.42) selects one of two implementations:
// - DataCcsdsPackingNew.cc: the packing parameters computed with the simple_packing library, the
//   data coded with SpAec (projects/spaec)
// - DataCcsdsPackingLegacy.cc: the packing parameters computed as in legacy ecCodes, the data coded
//   with libaec
// With libaec (ENABLE_AEC=ON) the legacy implementation is the default and ECCODES_USE_SPAEC=1
// selects the new one; without libaec the new implementation is always used. Data with options
// SpAec does not implement (signed samples, no preprocessing, RSI padding, block sizes other than
// 8, 16, 32 and 64 with AEC_NOT_ENFORCE) always use the legacy implementation (which reports them
// as not implemented without libaec).

#include "DataCcsdsPacking.h"

#include <cstring>

namespace eccodes::accessor
{

namespace
{

bool use_new_implementation()
{
#if defined(HAVE_LIBAEC) || defined(HAVE_AEC)
    static const bool useNew = [] {
        const char* env = getenv("ECCODES_USE_SPAEC");
        return env != nullptr && env[0] != '\0' && strcmp(env, "0") != 0;
    }();
    return useNew;
#else
    return true;
#endif
}

// Values of AEC_DATA_SIGNED, AEC_DATA_PREPROCESS and AEC_PAD_RSI in libaec.h
constexpr long aecSignedFlag = 1, aecPreprocessFlag = 8, aecPadRsiFlag = 32;

// SpAec implements the CCSDS options used by GRIB: preprocessed unsigned samples, basic or
// restricted code options, no RSI padding and the block sizes of the standard (8, 16, 32, 64;
// libaec accepts other even block sizes with AEC_NOT_ENFORCE)
bool spaec_supports(long flags, long block_size)
{
    const bool standardBlockSize = block_size == 8 || block_size == 16 || block_size == 32 || block_size == 64;
    return (flags & aecPreprocessFlag) && !(flags & (aecSignedFlag | aecPadRsiFlag)) && standardBlockSize;
}

}  // namespace

void DataCcsdsPacking::init(const long v, grib_arguments* args)
{
    Values::init(v, args);

    number_of_values_ = args->get_name(carg_++);
    reference_value_ = args->get_name(carg_++);
    binary_scale_factor_ = args->get_name(carg_++);
    decimal_scale_factor_ = args->get_name(carg_++);
    optimize_scaling_factor_ = args->get_name(carg_++);
    bits_per_value_ = args->get_name(carg_++);
    number_of_data_points_ = args->get_name(carg_++);
    ccsds_flags_ = args->get_name(carg_++);
    ccsds_block_size_ = args->get_name(carg_++);
    ccsds_rsi_ = args->get_name(carg_++);

    flags_ |= GRIB_ACCESSOR_FLAG_DATA;
}

int DataCcsdsPacking::value_count(long* count)
{
    *count = 0;
    return grib_get_long_internal(get_enclosing_handle(), number_of_values_, count);
}

int DataCcsdsPacking::pack_double(const double* val, size_t* len)
{
    long ccsds_flags = 0, ccsds_block_size = 0;
    int err = grib_get_long_internal(get_enclosing_handle(), ccsds_flags_, &ccsds_flags);
    if (err != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(get_enclosing_handle(), ccsds_block_size_, &ccsds_block_size)) != GRIB_SUCCESS)
        return err;
    if (use_new_implementation() && spaec_supports(ccsds_flags, ccsds_block_size))
        return pack_double_new(val, len);
    return pack_double_legacy(val, len);
}

template <typename T>
int DataCcsdsPacking::unpack(T* val, size_t* len)
{
    // ECC-477: Don't call grib_get_long_internal to suppress error message being output
    long ccsds_flags = 0, ccsds_block_size = 0;
    int err = grib_get_long(get_enclosing_handle(), ccsds_flags_, &ccsds_flags);
    if (err != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(get_enclosing_handle(), ccsds_block_size_, &ccsds_block_size)) != GRIB_SUCCESS)
        return err;
    if (use_new_implementation() && spaec_supports(ccsds_flags, ccsds_block_size))
        return unpack_new<T>(val, len);
    return unpack_legacy<T>(val, len);
}

int DataCcsdsPacking::unpack_double(double* val, size_t* len)
{
    return unpack<double>(val, len);
}

int DataCcsdsPacking::unpack_float(float* val, size_t* len)
{
    return unpack<float>(val, len);
}

int DataCcsdsPacking::unpack_double_element(size_t idx, double* val)
{
    // The index idx relates to codedValues NOT values!
    grib_handle* hand = get_enclosing_handle();
    int err = 0;
    size_t size = 0;
    long bits_per_value = 0;
    double reference_value = 0;
    double* values = NULL;

    if ((err = grib_get_long_internal(hand, bits_per_value_, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(hand, reference_value_, &reference_value)) != GRIB_SUCCESS)
        return err;
    if (!std::isfinite(reference_value)) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s %s: referenceValue=%g is not finite (corrupt message)",
                         accessor_type().c_str(), __func__, reference_value);
        return GRIB_DECODING_ERROR;
    }

    err = grib_get_size(hand, "codedValues", &size);
    if (err) return err;
    if (idx >= size) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s: %s: Invalid index %zu (should be less than %zu)",
                         accessor_type().c_str(), __func__, idx, size);
        return GRIB_INVALID_ARGUMENT;
    }

    // Special case of constant field
    if (bits_per_value == 0) {
        *val = reference_value;
        return GRIB_SUCCESS;
    }

    values = (double*)grib_context_malloc_clear(context_, size * sizeof(double));
    err = grib_get_double_array(hand, "codedValues", values, &size);
    if (err) {
        grib_context_free(context_, values);
        return err;
    }
    *val = values[idx];
    grib_context_free(context_, values);
    return GRIB_SUCCESS;
}

int DataCcsdsPacking::unpack_double_element_set(const size_t* index_array, size_t len, double* val_array)
{
    grib_handle* hand = get_enclosing_handle();
    size_t size = 0, i = 0;
    double* values = NULL;
    int err = 0;
    long bits_per_value = 0;
    double reference_value = 0;

    if ((err = grib_get_long_internal(hand, bits_per_value_, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(hand, reference_value_, &reference_value)) != GRIB_SUCCESS)
        return err;
    if (!std::isfinite(reference_value)) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s %s: referenceValue=%g is not finite (corrupt message)",
                         accessor_type().c_str(), __func__, reference_value);
        return GRIB_DECODING_ERROR;
    }

    // GRIB-564: The indexes in index_array relate to codedValues NOT values!
    err = grib_get_size(get_enclosing_handle(), "codedValues", &size);
    if (err)
        return err;

    for (i = 0; i < len; i++) {
        if (index_array[i] >= size) {
            grib_context_log(context_, GRIB_LOG_ERROR, "%s: %s: Invalid index %zu (should be less than %zu)",
                             accessor_type().c_str(), __func__, index_array[i], size);
            return GRIB_INVALID_ARGUMENT;
        }
    }

    // Special case of constant field
    if (bits_per_value == 0) {
        for (i = 0; i < len; i++) {
            val_array[i] = reference_value;
        }
        return GRIB_SUCCESS;
    }

    values = (double*)grib_context_malloc_clear(context_, size * sizeof(double));
    err = grib_get_double_array(get_enclosing_handle(), "codedValues", values, &size);
    if (err) {
        grib_context_free(context_, values);
        return err;
    }
    for (i = 0; i < len; i++) {
        val_array[i] = values[index_array[i]];
    }

    grib_context_free(context_, values);
    return GRIB_SUCCESS;
}

int DataCcsdsPacking::pack_bytes(const unsigned char* val, size_t* len)
{
    const size_t length = *len;
    return grib_buffer_replace(this, val, length, 1, 1);
}

}  // namespace eccodes::accessor
