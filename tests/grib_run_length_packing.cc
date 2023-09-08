/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include <stdio.h>
#include "eccodes.h"
#undef NDEBUG
#include <assert.h>

// Provided by Tatsuya Noyori

int main(int argc, char* argv[])
{
    size_t str_len        = 0;
    long number_of_values = 4, bits_per_value = 8, max_level_value = 3, number_of_level_values = 5, decimal_scale_factor = 1;
    size_t level_values_length = number_of_level_values;
    long level_values[]        = { 101L, 102L, 103L, 104L, 105L };
    double* values             = NULL;
    size_t values_length       = number_of_values;
    values                     = (double*)malloc(values_length * sizeof(double));
    values[0]                  = 9999.0;

    assert(argc == 2);

    for (size_t i = 1; i < values_length; i++) {
        values[i] = (double)((100.0 + i) / 10.0);
    }
    codes_handle* h = codes_grib_handle_new_from_samples(0, "GRIB2");

    CODES_CHECK(codes_set_long(h, "numberOfDataPoints", number_of_values), 0);
    CODES_CHECK(codes_set_long(h, "Ni", sqrt(number_of_values)), 0);
    CODES_CHECK(codes_set_long(h, "Nj", sqrt(number_of_values)), 0);

    CODES_CHECK(codes_set_double(h, "latitudeOfFirstGridPointInDegrees", 35.000000), 0);
    CODES_CHECK(codes_set_double(h, "longitudeOfFirstGridPointInDegrees", 135.000000), 0);
    CODES_CHECK(codes_set_double(h, "latitudeOfLastGridPointInDegrees", 35.000000 + (0.012500 * sqrt(number_of_values))), 0);
    CODES_CHECK(codes_set_double(h, "longitudeOfLastGridPointInDegrees", 135.000000 + (0.008333 * sqrt(number_of_values))), 0);
    CODES_CHECK(codes_set_double(h, "iDirectionIncrementInDegrees", 0.012500), 0);
    CODES_CHECK(codes_set_double(h, "jDirectionIncrementInDegrees", 0.008333), 0);
    CODES_CHECK(codes_set_long(h, "jScansPositively", 1), 0);

    CODES_CHECK(codes_set_string(h, "shortName", "t", &str_len), 0);
    CODES_CHECK(codes_set_long(h, "numberOfValues", 1), 0);
    CODES_CHECK(codes_set_string(h, "packingType", "grid_run_length", &str_len), 0);
    CODES_CHECK(codes_set_long(h, "numberOfValues", number_of_values), 0);
    CODES_CHECK(codes_set_long(h, "bitsPerValue", bits_per_value), 0);
    CODES_CHECK(codes_set_long(h, "maxLevelValue", max_level_value), 0);
    CODES_CHECK(codes_set_long(h, "numberOfLevelValues", number_of_level_values), 0);
    CODES_CHECK(codes_set_long(h, "decimalScaleFactor", decimal_scale_factor), 0);
    CODES_CHECK(codes_set_long_array(h, "levelValues", level_values, level_values_length), 0);
    CODES_CHECK(codes_set_double_array(h, "values", values, values_length), 0);

    CODES_CHECK(codes_write_message(h, argv[1], "w"), 0);
    codes_handle_delete(h);

    free(values);
    return 0;
}
