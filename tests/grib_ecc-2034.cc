/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#undef NDEBUG
#include <assert.h>
#include "eccodes.h"

int main(int argc, char** argv)
{
    int err = 0;
    double d, e;
    long count = 1;
    codes_context* c = codes_context_get_default();

    // This has parameter 167 which is 2m temperature
    codes_handle* h = codes_grib_handle_new_from_samples(c,  "regular_ll_pl_grib1");
    assert(h);

    size_t values_len = 0;
    CODES_CHECK(codes_get_size(h, "values", &values_len), 0);

    // Field values ranging from 0 to 1
    double* values = (double*)malloc(values_len * sizeof(double));
    e = d = 10e-8;
    for (size_t i = 0; i < values_len; i++) {
        if (count > 100) {
            e *= 10;
            count = 1;
        }
        values[i] = d;
        d += e;
        count++;
    }

    CODES_CHECK(codes_set_long(h, "bitsPerValue", 16), 0);

    // Set data values without any data quality checks
    codes_context_set_data_quality_checks(c, 0);
    err = codes_set_double_array(h, "values", values, values_len);
    assert(!err);

    // Enable checks: Should fail
    codes_context_set_data_quality_checks(c, 1);
    err = codes_set_double_array(h, "values", values, values_len);
    assert(err == GRIB_OUT_OF_RANGE);

    // Change paramId. Now should pass
    CODES_CHECK(codes_set_long(h, "paramId", 49), 0);
    err = codes_set_double_array(h, "values", values, values_len);
    assert(!err);

    free(values);
    codes_handle_delete(h);

    return 0;
}
