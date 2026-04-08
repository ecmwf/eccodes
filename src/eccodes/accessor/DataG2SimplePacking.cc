/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "DataG2SimplePacking.h"
#include "grib_scaling.h"

#ifdef HAVE_SIMPLE_PACKING_SP
    #include "simple/Binary.h"
    #include "Parameters.h"
    #include "BPVOptimizer.h"
    #include "BinDecOptimizer.h"
    #include "DecOptimizer.h"
#endif

eccodes::accessor::DataG2SimplePacking _grib_accessor_data_g2simple_packing;
eccodes::Accessor* grib_accessor_data_g2simple_packing = &_grib_accessor_data_g2simple_packing;

namespace eccodes::accessor
{

#ifdef HAVE_SIMPLE_PACKING_SP
static bool use_sp()
{
    const char* env = getenv("ECCODES_USE_LEGACY_SIMPLE_PACKING");
    return (env == nullptr || env[0] == '\0');
}
#endif

void DataG2SimplePacking::init(const long v, grib_arguments* args)
{
    DataSimplePacking::init(v, args);
    flags_ |= GRIB_ACCESSOR_FLAG_DATA;
    edition_ = 2;
}

int DataG2SimplePacking::value_count(long* n_vals)
{
    *n_vals = 0;
    return grib_get_long_internal(get_enclosing_handle(), number_of_values_, n_vals);
}

int DataG2SimplePacking::pack_double(const double* cval, size_t* len)
{
    // grib_accessor* super                = *(cclass_ ->super);
    size_t n_vals             = *len;
    double reference_value    = 0;
    long binary_scale_factor  = 0;
    long bits_per_value       = 0;
    long decimal_scale_factor = 0;
    double decimal            = 1;
    size_t buflen             = 0;
    unsigned char* buf        = NULL;
    unsigned char* encoded    = NULL;
    double divisor            = 1;
    long off                  = 0;
    int ret                   = 0;
    double units_factor       = 1.0;
    double units_bias         = 0.0;
    double* val               = (double*)cval;
    int i;
    grib_context* c = context_;

    if (*len == 0) {
        grib_buffer_replace(this, NULL, 0, 1, 1);
        return GRIB_SUCCESS;
    }

    if (ret == GRIB_SUCCESS)
        ret = grib_set_long_internal(get_enclosing_handle(), number_of_values_, *len);

    if (ret != GRIB_SUCCESS)
        return ret;

    if (units_factor_ &&
        (grib_get_double_internal(get_enclosing_handle(), units_factor_, &units_factor) == GRIB_SUCCESS)) {
        grib_set_double_internal(get_enclosing_handle(), units_factor_, 1.0);
    }

    if (units_bias_ &&
        (grib_get_double_internal(get_enclosing_handle(), units_bias_, &units_bias) == GRIB_SUCCESS)) {
        grib_set_double_internal(get_enclosing_handle(), units_bias_, 0.0);
    }

    if (units_factor != 1.0) {
        if (units_bias != 0.0) {
            for (i = 0; i < n_vals; i++) {
                val[i] = val[i] * units_factor + units_bias;
            }
        }
        else {
            for (i = 0; i < n_vals; i++) {
                val[i] *= units_factor;
            }
        }
    }
    else if (units_bias != 0.0) {
        for (i = 0; i < n_vals; i++) {
            val[i] += units_bias;
        }
    }

    /* IEEE packing */
    if (c->ieee_packing) {
        grib_handle* h = get_enclosing_handle();
        long precision = 0; /* Either 1(=32 bits) or 2(=64 bits) */
        size_t lenstr  = 10;
        if ((ret = codes_check_grib_ieee_packing_value(c->ieee_packing)) != GRIB_SUCCESS)
            return ret;
        precision = c->ieee_packing == 32 ? 1 : 2;
        if ((ret = grib_set_string(h, "packingType", "grid_ieee", &lenstr)) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_set_long(h, "precision", precision)) != GRIB_SUCCESS)
            return ret;

        return grib_set_double_array(h, "values", val, *len);
    }

#ifdef HAVE_SIMPLE_PACKING_SP
    if (use_sp() && n_vals > 0) {
        double min_val = val[0], max_val = val[0];
        for (size_t j = 1; j < n_vals; j++) {
            if (val[j] < min_val) min_val = val[j];
            if (val[j] > max_val) max_val = val[j];
        }

        if (min_val != max_val) {
            grib_context_log(context_, GRIB_LOG_DEBUG,
                             "DataG2simplePacking : pack_double : packing %s, %zu values (SP optimizer)", name_, n_vals);
            try {
                grib_handle* gh = get_enclosing_handle();
                long bpv_l = 0, dsf_get = 0, bsf_l = 0, osf_l = 0, changing_prec = 0;
                if ((ret = grib_get_long_internal(gh, bits_per_value_, &bpv_l)) != GRIB_SUCCESS) return ret;
                if ((ret = grib_get_long_internal(gh, decimal_scale_factor_, &dsf_get)) != GRIB_SUCCESS) return ret;
                if ((ret = grib_get_long_internal(gh, binary_scale_factor_, &bsf_l)) != GRIB_SUCCESS) return ret;
                if ((ret = grib_get_long_internal(gh, optimize_scaling_factor_, &osf_l)) != GRIB_SUCCESS) return ret;
                if ((ret = grib_get_long_internal(gh, changing_precision_, &changing_prec)) != GRIB_SUCCESS) return ret;

                if (bpv_l > (long)(sizeof(long) * 8 - 1)) {
                    grib_context_log(context_, GRIB_LOG_ERROR, "Unable to compute packing parameters. Invalid bits per value");
                    return GRIB_INVALID_BPV;
                }

                if ((ret = grib_check_data_values_minmax(gh, min_val, max_val)) != GRIB_SUCCESS) {
                    grib_context_log(context_, GRIB_LOG_ERROR, "GRIB2 simple packing: unable to set values (%s)", grib_get_error_message(ret));
                    return ret;
                }

                dirty_ = 1;

                if (changing_prec == 0 && bpv_l == 0 && dsf_get == 0) {
                    grib_context_log(context_, GRIB_LOG_WARNING,
                                     "%s==0 and %s==0 (setting %s=24)",
                                     bits_per_value_, decimal_scale_factor_, bits_per_value_);
                    bpv_l = 24;
                }

                std::unique_ptr<Optimizer> optimizer;
                if (bpv_l == 0 || (bsf_l == 0 && dsf_get != 0)) {
                    optimizer = std::make_unique<BPVOptimizer>(static_cast<int>(dsf_get));
                } else if (osf_l) {
                    int compat_gribex = c->gribex_mode_on && edition_ == 1;
                    optimizer = std::make_unique<BinDecOptimizer>(compat_gribex, 1, static_cast<int>(bpv_l));
                } else {
                    int last = 127;
                    if (c->gribex_mode_on && edition_ == 1) last = 99;
                    optimizer = std::make_unique<DecOptimizer>(bpv_l, last);
                }

                SimplePackingBinary<double> sp(std::move(optimizer));
                std::vector<double> values(val, val + n_vals);
                auto [params, encoded_buf] = sp.pack(values);

                if ((ret = grib_set_double_internal(gh, reference_value_, params.referenceValue())) != GRIB_SUCCESS) return ret;
                if ((ret = grib_set_long_internal(gh, changing_precision_, 0)) != GRIB_SUCCESS) return ret;
                if ((ret = grib_set_long_internal(gh, binary_scale_factor_, params.binaryScaleFactor())) != GRIB_SUCCESS) return ret;
                if ((ret = grib_set_long_internal(gh, decimal_scale_factor_, params.decimalScaleFactor())) != GRIB_SUCCESS) return ret;
                if ((ret = grib_set_long_internal(gh, bits_per_value_, params.bitsPerValue())) != GRIB_SUCCESS) return ret;

                grib_buffer_replace(this, encoded_buf.data(), encoded_buf.size(), 1, 1);
                return GRIB_SUCCESS;
            }
            catch (const std::exception& e) {
                grib_context_log(context_, GRIB_LOG_ERROR, "%s %s: SP encode error: %s",
                                 class_name_, __func__, e.what());
                return GRIB_ENCODING_ERROR;
            }
        }
    }
#endif

    ret = DataSimplePacking::pack_double(cval, len);
    switch (ret) {
        case GRIB_CONSTANT_FIELD:
            grib_buffer_replace(this, NULL, 0, 1, 1);
            return GRIB_SUCCESS;
        case GRIB_SUCCESS:
            break;
        default:
            grib_context_log(context_, GRIB_LOG_ERROR, "GRIB2 simple packing: unable to set values (%s)", grib_get_error_message(ret));
            return ret;
    }

    if ((ret = grib_get_double_internal(get_enclosing_handle(), reference_value_, &reference_value)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), binary_scale_factor_, &binary_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), bits_per_value_, &bits_per_value)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), decimal_scale_factor_, &decimal_scale_factor)) != GRIB_SUCCESS)
        return ret;

    decimal = codes_power<double>(decimal_scale_factor, 10);
    divisor = codes_power<double>(-binary_scale_factor, 2);

    buflen  = (((bits_per_value * n_vals) + 7) / 8) * sizeof(unsigned char);

    buf     = (unsigned char*)grib_context_buffer_malloc_clear(context_, buflen);
    encoded = buf;

    grib_encode_double_array(n_vals, val, bits_per_value, reference_value, decimal, divisor, encoded, &off);

    grib_context_log(context_, GRIB_LOG_DEBUG,
                     "DataG2simplePacking : pack_double : packing %s, %d values", name_, n_vals);

    grib_buffer_replace(this, buf, buflen, 1, 1);

    grib_context_buffer_free(context_, buf);

    return ret;
}

int DataG2SimplePacking::pack_bytes(const unsigned char* val, size_t* len)
{
    size_t length = *len;
    grib_buffer_replace(this, val, length, 1, 1);
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
