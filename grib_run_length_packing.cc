#include <stdio.h>
#include "eccodes.h"

static void usage(const char *app)
{
    fprintf(stderr,"Usage is: %s ouput_file\n", app);
}

int main(int argc, char *argv[])
{
    int err = 0;
    const char *res_name = "GRIB2";
    size_t str_len = 0;
    long number_of_values = 4, bits_per_value = 8, max_level_value = 3, number_of_level_values = 5, decimal_scale_factor = 1;
    size_t level_values_length = number_of_level_values;
    long level_values[level_values_length] = {101L, 102L, 103L, 104L, 105L};
    double *values = NULL;
    size_t values_length = number_of_values;
    values = (double*)malloc(values_length * sizeof(double));
    int i = 0;
    values[0] = 9999.0;
    for (i=1; i<values_length; i++) {
        values[i] = (double)((100.0 + i) / 10.0) ;
    }
    codes_handle *sample_handle = codes_grib_handle_new_from_samples(0, res_name);
    codes_handle *clone_handle = codes_handle_clone(sample_handle);
    codes_handle_delete(sample_handle);
    codes_set_long(clone_handle, "discipline", 0);
    codes_set_long(clone_handle, "editionNumber", 2);
    codes_set_long(clone_handle, "centre", 34);
    codes_set_long(clone_handle, "subCentre", 0);
    codes_set_long(clone_handle, "significanceOfReferenceTime", 0);
    codes_set_long(clone_handle, "dataDate", 20230830);
    codes_set_long(clone_handle, "dataTime", 0);
    codes_set_long(clone_handle, "productionStatusOfProcessedData", 1);
    codes_set_long(clone_handle, "typeOfProcessedData", 0);
    codes_set_long(clone_handle, "numberOfDataPoints", number_of_values);
    codes_set_long(clone_handle, "interpretationOfNumberOfPoints", 0);
    codes_set_long(clone_handle, "gridDefinitionTemplateNumber", 0);
    codes_set_long(clone_handle, "Ni", sqrt(number_of_values));
    codes_set_long(clone_handle, "Nj", sqrt(number_of_values));
    codes_set_long(clone_handle, "iScansNegatively", 0);
    codes_set_long(clone_handle, "jScansPositively", 0);
    codes_set_long(clone_handle, "jPointsAreConsecutive", 0);
    codes_set_long(clone_handle, "alternativeRowScanning", 0);
    codes_set_long(clone_handle, "latitudeOfFirstGridPointInDegrees", 35.000000);
    codes_set_long(clone_handle, "longitudeOfFirstGridPointInDegrees", 135.000000);
    codes_set_long(clone_handle, "latitudeOfLastGridPointInDegrees", 35.000000 + (0.012500 * sqrt(number_of_values)));
    codes_set_long(clone_handle, "longitudeOfLastGridPointInDegrees", 135.000000 + (0.008333 * sqrt(number_of_values)));
    codes_set_long(clone_handle, "iDirectionIncrementInDegrees", 0.012500);
    codes_set_long(clone_handle, "jDirectionIncrementInDegrees", 0.008333);
    codes_set_string(clone_handle, "gridType", "regular_ll", &str_len);
    codes_set_long(clone_handle, "productDefinitionTemplateNumber", 0);
    codes_set_long(clone_handle, "parameterCategory", 0);
    codes_set_long(clone_handle, "parameterNumber", 0);
    codes_set_long(clone_handle, "typeOfGeneratingProcess", 0);
    codes_set_long(clone_handle, "generatingProcessIdentifier", 255);
    codes_set_long(clone_handle, "indicatorOfUnitOfTimeRange", 0);
    codes_set_long(clone_handle, "stepUnits", 1);
    codes_set_long(clone_handle, "forecastTime", 0);
    codes_set_long(clone_handle, "stepRange", 0);
    codes_set_long(clone_handle, "typeOfFirstFixedSurface", 1);
    codes_set_missing(clone_handle, "scaleFactorOfFirstFixedSurface");
    codes_set_missing(clone_handle, "scaledValueOfFirstFixedSurface");
    codes_set_long(clone_handle, "typeOfSecondFixedSurface", 255);
    codes_set_missing(clone_handle, "scaleFactorOfSecondFixedSurface");
    codes_set_missing(clone_handle, "scaledValueOfSecondFixedSurface");
    codes_set_long(clone_handle, "level", 0);
    codes_set_string(clone_handle, "shortName", "t", &str_len);
    codes_set_long(clone_handle, "numberOfValues", 1);
    codes_set_string(clone_handle, "packingType", "grid_run_length", &str_len);
    codes_set_long(clone_handle, "numberOfValues", number_of_values);
    codes_set_long(clone_handle, "bitsPerValue", bits_per_value);
    codes_set_long(clone_handle, "maxLevelValue", max_level_value);
    codes_set_long(clone_handle, "numberOfLevelValues", number_of_level_values);
    codes_set_long(clone_handle, "decimalScaleFactor", decimal_scale_factor);
    codes_set_long_array(clone_handle, "levelValues", level_values, level_values_length);
    codes_set_long(clone_handle, "bitMapIndicator", 255);
    codes_set_long(clone_handle, "bitmapPresent", 0);
    codes_set_double_array(clone_handle, "values", values, values_length);
    codes_write_message(clone_handle, argv[1], "w");
    codes_handle_delete(clone_handle);

    return 0;
}