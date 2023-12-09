/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api.h"

/*
 * Test that packing sets the correct numberOfValues
 * philippe.marguinaud@meteo.fr, 2016/02
 */
#include "grib_gridded_values.h" // array 'values' defined here

#define NLON 81
#define NLAT 166

int main(int argc, char* argv[])
{
    size_t len, slen;
    grib_handle* h = NULL;
    const char* packingType[] = { "grid_second_order", "grid_complex_spatial_differencing", "grid_complex" };
    const size_t numTypes = sizeof(packingType)/sizeof(packingType[0]);
    int ipackingType = 0;
    const double zmiss = 9999999999.;
    long numberOfValues, numberOfDataPoints;
    int i, numberOfMissing;

    for (ipackingType = 0; ipackingType < numTypes; ipackingType++) {
        GRIB_CHECK(((h = grib_handle_new_from_samples(NULL, "regular_ll_pl_grib2")) == NULL), 0);
        GRIB_CHECK(grib_set_long(h, "Ni", NLON), 0);
        GRIB_CHECK(grib_set_long(h, "Nj", NLAT), 0);
        GRIB_CHECK(grib_set_double(h, "latitudeOfFirstGridPointInDegrees", 80.), 0);
        GRIB_CHECK(grib_set_double(h, "longitudeOfFirstGridPointInDegrees", -100.), 0);
        GRIB_CHECK(grib_set_double(h, "latitudeOfLastGridPointInDegrees", 0.), 0);
        GRIB_CHECK(grib_set_double(h, "longitudeOfLastGridPointInDegrees", 65.), 0);
        GRIB_CHECK(grib_set_long(h, "iScansNegatively", 0), 0);
        GRIB_CHECK(grib_set_long(h, "jScansPositively", 0), 0);
        GRIB_CHECK(grib_set_long(h, "NV", 0), 0);
        slen = strlen("sfc");
        GRIB_CHECK(grib_set_string(h, "typeOfFirstFixedSurface", "sfc", &slen), 0);

        GRIB_CHECK(grib_set_long(h, "bitsPerValue", 16), 0);
        slen = strlen(packingType[ipackingType]);
        printf("Doing packingType=%s\n", packingType[ipackingType]);
        GRIB_CHECK(grib_set_string(h, "packingType", packingType[ipackingType], &slen), 0);

        GRIB_CHECK(grib_set_long(h, "bitmapPresent", 1), 0);
        GRIB_CHECK(grib_set_double(h, "missingValue", zmiss), 0);

        for (i = 0, numberOfMissing = 0; i < NLON * NLAT; i += 100, numberOfMissing++) {
            values[i] = zmiss;
        }

        len = NLON * NLAT;
        GRIB_CHECK(grib_set_double_array(h, "values", values, len), 0);

        GRIB_CHECK(grib_get_long(h, "numberOfValues", &numberOfValues), 0);
        GRIB_CHECK(grib_get_long(h, "numberOfDataPoints", &numberOfDataPoints), 0);

        if (numberOfValues + numberOfMissing != numberOfDataPoints) {
            printf("ERROR: numberOfValues = %ld, numberOfDataPoints = %ld, numberOfMissing = %d\n",
                        numberOfValues, numberOfDataPoints, numberOfMissing);
            printf("ERROR: numberOfValues appears to be incorrect\n");
            return 1;
        }

        GRIB_CHECK(grib_handle_delete(h), 0);
    }

    printf("ALL OK\n");
    return 0;
}
