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
#include <stdlib.h>

#include "grib_api_internal.h"

typedef enum IterMode
{
    WITH_VALUES,
    NO_VALUES
} IterMode;

int main(int argc, char** argv)
{
    int err  = 0;
    double lat = 0, lon = 0, value = 0;
    long n = 0, numberOfDataPoints = 0;
    char* filename = NULL;
    FILE* fin = NULL;
    grib_handle* h = NULL;
    grib_iterator* iter = NULL;
    char* option = NULL;
    IterMode mode = WITH_VALUES;
    int flags = 0;
    double* pValue = NULL;
    bool verbose = false;

    Assert(argc == 3);
    option = argv[1];
    filename = argv[2];
    if (strcmp(option, "-n")==0) {
        mode = NO_VALUES;
    }

    fin = fopen(filename, "rb");
    Assert(fin);
    h = grib_handle_new_from_file(0, fin, &err);
    Assert(!err);
    Assert(h);

    GRIB_CHECK(grib_get_long(h,"numberOfDataPoints", &numberOfDataPoints),0);

    flags = (mode == NO_VALUES) ? GRIB_GEOITERATOR_NO_VALUES : 0;
    iter = grib_iterator_new(h, flags, &err);
    Assert(!err);
    Assert(iter);

    Assert(grib_iterator_has_next(iter));
    n = 0;

    pValue = (mode == NO_VALUES) ? NULL : &value;
    while (grib_iterator_next(iter, &lat, &lon, pValue)) {
        if (n < numberOfDataPoints - 1) {
            Assert(grib_iterator_has_next(iter));
        }
        if (verbose) {
            printf("%9.3f %9.3f ",lat, lon);
            if (pValue) printf(" %.10e\n", *pValue);
            else printf("\n");
        }
        n++;
    }
    Assert(n == numberOfDataPoints);

    Assert(grib_iterator_has_next(iter) == 0);

    {
        // Test getting the previous value from a geoiterator.
        // Note: Only implemented for regular grids
        char gridType[128] = {0,};
        size_t len = sizeof(gridType);
        GRIB_CHECK(grib_get_string(h, "gridType", gridType, &len), 0);
        if (strstr(gridType, "regular_")) {
            double last_lat = 0, last_lon = 0, last_value = 0;
            int result = grib_iterator_previous(iter, &last_lat, &last_lon, &last_value);
            Assert(result == 1);
            Assert(lat == last_lat);
            Assert(lon == last_lon);
            Assert(value == last_value);
        }
    }

    grib_iterator_delete(iter);

    grib_handle_delete(h);

    return 0;
}
