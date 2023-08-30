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

int main(int argc, char *argv[])
{
    size_t str_len = 0;
    long number_of_values=4, bits_per_value=8, max_level_value=3, number_of_level_values=5, decimal_scale_factor=1;
    size_t level_values_length = number_of_level_values;
    long level_values[] = {101L, 102L, 103L, 104L, 105L};
    double *values = NULL;
    size_t values_length = number_of_values;
    values = (double*)malloc(values_length * sizeof(double));
    int i = 0;
    values[0] = 9999.0;

    assert(argc == 2);

    for (i=1; i<values_length; i++) {
        values[i] = (double)((100.0 + i) / 10.0) ;
    }
    codes_handle *sample_handle = codes_grib_handle_new_from_samples(0, "GRIB2");
    codes_handle *clone_handle = codes_handle_clone(sample_handle);
    codes_handle_delete(sample_handle);
    codes_set_long(clone_handle, "centre", 34);
    codes_set_long(clone_handle, "significanceOfReferenceTime", 0);
    codes_set_long(clone_handle, "dataDate", 20230830);
    codes_set_long(clone_handle, "dataTime", 0);
    codes_set_long(clone_handle, "productionStatusOfProcessedData", 1);
    codes_set_long(clone_handle, "typeOfProcessedData", 0);
    codes_set_long(clone_handle, "numberOfDataPoints", number_of_values);
    codes_set_long(clone_handle, "interpretationOfNumberOfPoints", 0);
    codes_set_long(clone_handle, "Ni", sqrt(number_of_values));
    codes_set_long(clone_handle, "Nj", sqrt(number_of_values));

    codes_set_long(clone_handle, "latitudeOfFirstGridPointInDegrees", 35.000000);
    codes_set_long(clone_handle, "longitudeOfFirstGridPointInDegrees", 135.000000);
    codes_set_long(clone_handle, "latitudeOfLastGridPointInDegrees", 35.000000 + (0.012500 * sqrt(number_of_values)));
    codes_set_long(clone_handle, "longitudeOfLastGridPointInDegrees", 135.000000 + (0.008333 * sqrt(number_of_values)));
    codes_set_long(clone_handle, "iDirectionIncrementInDegrees", 0.012500);
    codes_set_long(clone_handle, "jDirectionIncrementInDegrees", 0.008333);

    codes_set_long(clone_handle, "indicatorOfUnitOfTimeRange", 0);
    codes_set_long(clone_handle, "stepUnits", 1);
    codes_set_long(clone_handle, "forecastTime", 0);
    codes_set_long(clone_handle, "stepRange", 0);

    codes_set_string(clone_handle, "shortName", "t", &str_len);
    codes_set_long(clone_handle, "numberOfValues", 1);
    codes_set_string(clone_handle, "packingType", "grid_run_length", &str_len);
    codes_set_long(clone_handle, "numberOfValues", number_of_values);
    codes_set_long(clone_handle, "bitsPerValue", bits_per_value);
    codes_set_long(clone_handle, "maxLevelValue", max_level_value);
    codes_set_long(clone_handle, "numberOfLevelValues", number_of_level_values);
    codes_set_long(clone_handle, "decimalScaleFactor", decimal_scale_factor);
    codes_set_long_array(clone_handle, "levelValues", level_values, level_values_length);
    //codes_set_long(clone_handle, "bitMapIndicator", 255);
    //CODES_CHECK(codes_set_long(clone_handle, "bitmapPresent", 0), 0);
    CODES_CHECK(codes_set_double_array(clone_handle, "values", values, values_length), 0);
    codes_write_message(clone_handle, argv[1], "w");
    codes_handle_delete(clone_handle);

    free(values);
    return 0;
}
