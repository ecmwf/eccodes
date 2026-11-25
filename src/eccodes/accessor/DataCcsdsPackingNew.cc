/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

// The new implementation of the CCSDS packing: the packing parameters are computed with the
// simple_packing library (projects/simple_packing) and the data are coded with SpAec
// (projects/spaec). The parameters and the coded data are the same as those of the legacy
// implementation (DataCcsdsPackingLegacy.cc), with one intended exception: with 31 or more bits
// per value and optimizeScaleFactor=1, legacy grib_optimize_decimal_factor stores the largest
// coded value in an int, which overflows (from 32 bits on always), so legacy falls back to the
// parameters of optimizeScaleFactor=0; BinDecOptimizer uses long and optimises (see
// BinDecOptimizer.h).
// The selection: ECCODES_USE_SPAEC=1 (see DataCcsdsPacking.cc); ECCODES_USE_SP is not needed.

#include "DataCcsdsPacking.h"

#include <cmath>
#include <cstring>
#include <stdexcept>

#include "simple_packing/simple_packing.h"
#include "spaec/spaec.h"

namespace eccodes::accessor
{

namespace
{

constexpr long maxBitsPerValue = 32;

// Values of AEC_DATA_PREPROCESS and AEC_RESTRICTED in libaec.h
constexpr long aecPreprocessFlag = 8, aecRestrictedFlag = 16;

// The options of SpAec for the CCSDS keys. The code options are applied like libaec does: the
// restricted set for 1 to 4 bits per value, an error for 5 to 8 bits, the basic set for more bits.
// Without AEC_DATA_PREPROCESS the samples are coded without preprocessor.
spaec::Options spaec_options(long flags, long bits_per_value, long block_size, long rsi)
{
    spaec::Options options;
    options.bitsPerSample = static_cast<unsigned>(bits_per_value);
    options.blockSize = static_cast<unsigned>(block_size);
    options.referenceInterval = static_cast<unsigned>(rsi);
    options.codeOptions = (flags & aecRestrictedFlag) ? spaec::CodeOptions::Restricted : spaec::CodeOptions::Basic;
    options.preprocessor = (flags & aecPreprocessFlag) ? spaec::Preprocessor::UnitDelay : spaec::Preprocessor::None;
    return options;
}

void print_spaec_info(const char* func, long flags, long bits_per_value, long block_size, long rsi)
{
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.flags=%ld\n", func, flags);
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.bits_per_sample=%ld\n", func, bits_per_value);
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.block_size=%ld\n", func, block_size);
    fprintf(stderr, "ECCODES DEBUG CCSDS %s aec_stream.rsi=%ld\n", func, rsi);
}

}  // namespace

int DataCcsdsPacking::pack_double_new(const double* val, size_t* len)
{
    grib_handle* hand = get_enclosing_handle();
    int err = GRIB_SUCCESS;
    const size_t n_vals = *len;

    long binary_scale_factor = 0;
    long decimal_scale_factor = 0;
    long optimize_scaling_factor = 0;
    long bits_per_value = 0;
    long ccsds_flags = 0;
    long ccsds_block_size = 0;
    long ccsds_rsi = 0;

    dirty_ = 1;

    if ((err = grib_get_long_internal(hand, bits_per_value_, &bits_per_value)) != GRIB_SUCCESS)
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
    if (n_vals == 0) {
        grib_buffer_replace(this, NULL, 0, 1, 1);
        return GRIB_SUCCESS;
    }

    const sp::Range<double> range = sp::range(val, n_vals);
    const double min = range.min, max = range.max;
    if ((err = grib_check_data_values_minmax(hand, min, max)) != GRIB_SUCCESS) {
        return err;
    }

    // Constant field: no data, the reference value is the value (the decimal scale factor is kept)
    if (min == max) {
        // The scaling of a constant field: its reference value is the nearest float not above the value
        double reference = 0;
        try {
            reference = sp::scaling(min, max, sp::Method::bits(16)).referenceValue;
        }
        catch (const std::exception&) {
            grib_context_log(context_, GRIB_LOG_ERROR,
                             "%s %s: Unable to find nearest_smaller_value of %g for %s", accessor_type().c_str(), __func__, min, reference_value_);
            return GRIB_INTERNAL_ERROR;
        }
        if ((err = grib_set_double_internal(hand, reference_value_, reference)) != GRIB_SUCCESS)
            return err;
        if ((err = grib_set_long_internal(hand, number_of_values_, n_vals)) != GRIB_SUCCESS)
            return err;
        // ECC-2012
        if ((err = grib_set_long_internal(hand, binary_scale_factor_, 0)) != GRIB_SUCCESS)
            return err;
        // ECC-1387
        if ((err = grib_set_long_internal(hand, bits_per_value_, 0)) != GRIB_SUCCESS)
            return err;
        grib_buffer_replace(this, NULL, 0, 1, 1);
        return GRIB_SUCCESS;
    }

    if (bits_per_value == 0) {
        // ECC-1202: A non-constant field with bitsPerValue==0!
        bits_per_value = 24;  // Set sane value
    }

    // The packing parameters (the reference value is stored as IEEE float in GRIB edition 2)
    sp::Scaling scaling;
    try {
        const int bits = static_cast<int>(bits_per_value);
        // Given: the decimal scale factor and the bits per value, or the bits per value alone
        // (optimizeScaleFactor: with the decimal scale factor that uses them best)
        const sp::Method method = (binary_scale_factor == 0 && decimal_scale_factor != 0) ? sp::Method::bitsAndDecimals(bits, static_cast<int>(decimal_scale_factor))
                                : optimize_scaling_factor                                 ? sp::Method::bestPrecision(bits)
                                                                                          : sp::Method::bits(bits);
        scaling = sp::scaling(min, max, method);
    }
    // The error codes of the legacy code (grib_get_binary_scale_fact)
    catch (const std::underflow_error& e) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s %s: unable to compute the packing parameters: %s",
                         accessor_type().c_str(), __func__, e.what());
        return GRIB_UNDERFLOW;
    }
    catch (const std::exception& e) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s %s: unable to compute the packing parameters: %s",
                         accessor_type().c_str(), __func__, e.what());
        const bool outOfRange = dynamic_cast<const std::invalid_argument*>(&e) || dynamic_cast<const std::range_error*>(&e);
        return outOfRange ? GRIB_OUT_OF_RANGE : GRIB_ENCODING_ERROR;
    }
    const double reference_value = scaling.referenceValue;
    binary_scale_factor = scaling.binaryScaleFactor;
    decimal_scale_factor = scaling.decimalScaleFactor;

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

    if (bits_per_value < 1 || bits_per_value > maxBitsPerValue) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s pack_double: packing %s, bitsPerValue=%ld (max %ld)",
                         accessor_type().c_str(), name_, bits_per_value, maxBitsPerValue);
        return GRIB_INVALID_BPV;
    }

    grib_context_log(context_, GRIB_LOG_DEBUG, "%s pack_double: packing %s, %zu values (SpAec)", accessor_type().c_str(), name_, n_vals);
    if (hand->context->debug)
        print_spaec_info("pack_double", ccsds_flags, bits_per_value, ccsds_block_size, ccsds_rsi);

    unsigned char* buf = nullptr;
    size_t buflen = 0;
    try {
        const spaec::Options options = spaec_options(ccsds_flags, bits_per_value, ccsds_block_size, ccsds_rsi);
        const spaec::Scaling sampleScaling{ reference_value, static_cast<int>(binary_scale_factor), static_cast<int>(decimal_scale_factor) };

        // Worst-case output buffer
        buflen = spaec::maxCompressedSize(n_vals, options);
        buf = (unsigned char*)grib_context_buffer_malloc(context_, buflen);
        if (!buf) {
            return GRIB_OUT_OF_MEMORY;
        }
        buflen = spaec::compress(val, n_vals, sampleScaling, options, buf, buflen);
    }
    catch (const std::exception& e) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s %s: SpAec encode error: %s",
                         accessor_type().c_str(), __func__, e.what());
        grib_context_buffer_free(context_, buf);
        return GRIB_ENCODING_ERROR;
    }

    grib_buffer_replace(this, buf, buflen, 1, 1);
    grib_context_buffer_free(context_, buf);

    if ((err = grib_set_long_internal(hand, number_of_values_, n_vals)) != GRIB_SUCCESS)
        return err;
    return grib_set_long_internal(hand, bits_per_value_, bits_per_value);
}

template <typename T>
int DataCcsdsPacking::unpack_new(T* val, size_t* len)
{
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
    grib_handle* hand = get_enclosing_handle();
    int err = GRIB_SUCCESS;

    long nn = 0;
    long binary_scale_factor = 0;
    long decimal_scale_factor = 0;
    double reference_value = 0;
    long bits_per_value = 0;
    long ccsds_flags = 0;
    long ccsds_block_size = 0;
    long ccsds_rsi = 0;

    dirty_ = 0;

    if ((err = value_count(&nn)) != GRIB_SUCCESS)
        return err;
    const size_t n_vals = nn;

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

    if (*len < n_vals)
        return GRIB_ARRAY_TOO_SMALL;

    // Special case
    if (bits_per_value == 0) {
        for (size_t i = 0; i < n_vals; i++)
            val[i] = reference_value;
        *len = n_vals;
        return GRIB_SUCCESS;
    }

    const size_t buflen = byte_count();
    unsigned char* buf = hand->buffer->data + byte_offset();

    if (buflen == 0) {
        // An empty data section decodes as a constant field of the reference value (all samples
        // are 0), as ecCodes always did with libaec
        grib_context_log(context_, GRIB_LOG_DEBUG, "%s %s: empty data section, constant field of %zu values",
                         accessor_type().c_str(), __func__, n_vals);
        const T bscale = codes_power<T>(binary_scale_factor, 2);
        const T dscale = codes_power<T>(-decimal_scale_factor, 10);
        const T constant = static_cast<T>((0 * bscale + reference_value) * dscale);
        for (size_t i = 0; i < n_vals; i++)
            val[i] = constant;
        *len = n_vals;
        return GRIB_SUCCESS;
    }

    grib_context_log(context_, GRIB_LOG_DEBUG, "%s unpack: unpacking %s, %zu values (SpAec)", accessor_type().c_str(), name_, n_vals);
    if (hand->context->debug)
        print_spaec_info("unpack_*", ccsds_flags, bits_per_value, ccsds_block_size, ccsds_rsi);

    try {
        const spaec::Options options = spaec_options(ccsds_flags, bits_per_value, ccsds_block_size, ccsds_rsi);
        const spaec::Scaling sampleScaling{ reference_value, static_cast<int>(binary_scale_factor), static_cast<int>(decimal_scale_factor) };
        spaec::decompress(buf, buflen, sampleScaling, options, val, n_vals);
    }
    catch (const std::exception& e) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s %s: SpAec decode error: %s",
                         accessor_type().c_str(), __func__, e.what());
        return GRIB_DECODING_ERROR;
    }
    *len = n_vals;
    return GRIB_SUCCESS;
}

template int DataCcsdsPacking::unpack_new<double>(double* val, size_t* len);
template int DataCcsdsPacking::unpack_new<float>(float* val, size_t* len);

}  // namespace eccodes::accessor
