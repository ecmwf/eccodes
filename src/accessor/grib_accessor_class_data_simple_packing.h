
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_data_simple_packing_h
#define eccodes_accessor_data_simple_packing_h

#include "grib_accessor_class_values.h"
#include "grib_bits_any_endian_simple.h"
#include "grib_scaling.h"

class grib_accessor_data_simple_packing_t : public grib_accessor_values_t
{
public:
    /* Members defined in data_simple_packing */
    int edition;
    const char*  units_factor;
    const char*  units_bias;
    const char*  changing_precision;
    const char*  number_of_values;
    const char*  bits_per_value;
    const char*  reference_value;
    const char*  binary_scale_factor;
    const char*  decimal_scale_factor;
    const char*  optimize_scaling_factor;
};

class grib_accessor_class_data_simple_packing_t : public grib_accessor_class_values_t
{
public:
    grib_accessor_class_data_simple_packing_t(const char* name) : grib_accessor_class_values_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_data_simple_packing_t{}; }
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_float(grib_accessor*, float* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    int unpack_double_element(grib_accessor*, size_t i, double* val) override;
    int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array) override;
    int unpack_double_subarray(grib_accessor*, double* val, size_t start, size_t len) override;

private:
    template <typename T> int unpack(grib_accessor*, T*, size_t*);
};


template <typename T>
int grib_accessor_class_data_simple_packing_t::unpack(grib_accessor* a, T* val, size_t* len){
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");

    grib_accessor_data_simple_packing_t* self = (grib_accessor_data_simple_packing_t*)a;
    const char* cclass_name                 = a->cclass->name;
    grib_handle* gh                         = grib_handle_of_accessor(a);
    unsigned char* buf                      = (unsigned char*)grib_handle_of_accessor(a)->buffer->data;

    size_t i      = 0;
    int err       = 0;
    size_t n_vals = 0;
    long pos      = 0;
    long count    = 0;

    double reference_value;
    long binary_scale_factor;
    long bits_per_value;
    long decimal_scale_factor;
    long offsetBeforeData;
    double s            = 0;
    double d            = 0;
    double units_factor = 1.0;
    double units_bias   = 0.0;

    err = a->value_count(&count);    if (err)
        return err;
    n_vals = count;

    if (*len < n_vals) {
        *len = (long)n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if ((err = grib_get_long_internal(gh, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;

    /*
     * check we don't decode bpv > max(ulong) as it is
     * not currently supported by the algorithm
     */
    if (bits_per_value > (sizeof(long) * 8)) {
        return GRIB_INVALID_BPV;
    }

    if (self->units_factor &&
        (grib_get_double_internal(gh, self->units_factor, &units_factor) == GRIB_SUCCESS)) {
        grib_set_double_internal(gh, self->units_factor, 1.0);
    }

    if (self->units_bias &&
        (grib_get_double_internal(gh, self->units_bias, &units_bias) == GRIB_SUCCESS)) {
        grib_set_double_internal(gh, self->units_bias, 0.0);
    }

    if (n_vals == 0) {
        *len = 0;
        return GRIB_SUCCESS;
    }

    self->dirty = 0;

    if ((err = grib_get_double_internal(gh, self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(gh, self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(gh, self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

    /* Special case */

    if (bits_per_value == 0) {
        for (i = 0; i < n_vals; i++)
            val[i] = reference_value;
        *len = n_vals;
        return GRIB_SUCCESS;
    }

    s = codes_power<T>(binary_scale_factor, 2);
    d = codes_power<T>(-decimal_scale_factor, 10);

    grib_context_log(a->context, GRIB_LOG_DEBUG,
                     "%s %s: Creating %s, %zu values", cclass_name, __func__, a->name, n_vals);

    offsetBeforeData = a->byte_offset();    buf += offsetBeforeData;

    /*Assert(((bits_per_value*n_vals)/8) < (1<<29));*/ /* See GRIB-787 */

    /* ECC-941 */
    if (!a->context->ieee_packing) {
        /* Must turn off this check when the environment variable ECCODES_GRIB_IEEE_PACKING is on */
        long offsetAfterData = 0;
        err                  = grib_get_long(gh, "offsetAfterData", &offsetAfterData);
        if (!err && offsetAfterData > offsetBeforeData) {
            const long valuesSize = (bits_per_value * n_vals) / 8; /*in bytes*/
            if (offsetBeforeData + valuesSize > offsetAfterData) {
                grib_context_log(a->context, GRIB_LOG_ERROR,
                                "%s: Data section size mismatch: "
                                "offset before data=%ld, offset after data=%ld (num values=%zu, bits per value=%ld)",
                                cclass_name, offsetBeforeData, offsetAfterData, n_vals, bits_per_value);
                return GRIB_DECODING_ERROR;
            }
        }
        //         if (offsetBeforeData == offsetAfterData) {
        //             /* Crazy case: Constant field with bitsPerValue > 0 */
        //             for (i = 0; i < n_vals; i++)
        //                 val[i] = reference_value;
        //             *len = n_vals;
        //             return GRIB_SUCCESS;
        //         }
    }

    grib_context_log(a->context, GRIB_LOG_DEBUG,
                     "%s %s: calling outline function: bpv: %ld, rv: %g, bsf: %ld, dsf: %ld",
                     cclass_name, __func__, bits_per_value, reference_value, binary_scale_factor, decimal_scale_factor);
    grib_decode_array<T>(buf, &pos, bits_per_value, reference_value, s, d, n_vals, val);

    *len = (long)n_vals;

    if (units_factor != 1.0) {
        if (units_bias != 0.0) {
            for (i = 0; i < n_vals; i++) {
                val[i] = val[i] * units_factor + units_bias;
            }
        } else {
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
    return err;
}


#endif /* eccodes_accessor_data_simple_packing_h */
