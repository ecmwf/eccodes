/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * Synthetic test for forward and backward geoiterator traversal.
 * Creates a regular_ll GRIB from a sample, iterates forward storing
 * all lat/lon/value triples, then walks backward with previous()
 * verifying each point matches.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "grib_api_internal.h"

int main(int argc, char** argv)
{
    int err = 0;
    grib_handle* h = NULL;
    grib_iterator* iter = NULL;
    long numberOfDataPoints = 0;

    ECCODES_ASSERT(argc == 2);
    const char* sample_path = argv[1];

    FILE* fin = fopen(sample_path, "rb");
    ECCODES_ASSERT(fin);
    h = grib_handle_new_from_file(0, fin, &err);
    ECCODES_ASSERT(!err);
    ECCODES_ASSERT(h);
    fclose(fin);

    /* Set known data values so we can verify them */
    GRIB_CHECK(grib_get_long(h, "numberOfDataPoints", &numberOfDataPoints), 0);
    ECCODES_ASSERT(numberOfDataPoints > 0);

    double* values = (double*)malloc(numberOfDataPoints * sizeof(double));
    ECCODES_ASSERT(values);
    for (long i = 0; i < numberOfDataPoints; i++) {
        values[i] = 100.0 + (double)i;
    }
    size_t vals_len = (size_t)numberOfDataPoints;
    GRIB_CHECK(grib_set_double_array(h, "values", values, vals_len), 0);

    /* Forward iteration: store all lat/lon/value */
    iter = grib_iterator_new(h, 0, &err);
    ECCODES_ASSERT(!err);
    ECCODES_ASSERT(iter);

    double* fwd_lats = (double*)malloc(numberOfDataPoints * sizeof(double));
    double* fwd_lons = (double*)malloc(numberOfDataPoints * sizeof(double));
    double* fwd_vals = (double*)malloc(numberOfDataPoints * sizeof(double));
    ECCODES_ASSERT(fwd_lats && fwd_lons && fwd_vals);

    long n = 0;
    double lat = 0, lon = 0, val = 0;
    while (grib_iterator_next(iter, &lat, &lon, &val)) {
        ECCODES_ASSERT(n < numberOfDataPoints);
        fwd_lats[n] = lat;
        fwd_lons[n] = lon;
        fwd_vals[n] = val;
        n++;
    }
    ECCODES_ASSERT(n == numberOfDataPoints);

    /* Backward iteration using previous(): walk from end to start */
    long matches = 0;
    for (long i = numberOfDataPoints - 1; i >= 0; i--) {
        double prev_lat = 0, prev_lon = 0, prev_val = 0;
        int result = grib_iterator_previous(iter, &prev_lat, &prev_lon, &prev_val);
        ECCODES_ASSERT(result == 1);

        if (fabs(prev_lat - fwd_lats[i]) > 1e-6) {
            fprintf(stderr, "MISMATCH at index %ld: lat forward=%.10f backward=%.10f\n",
                    i, fwd_lats[i], prev_lat);
            return 1;
        }
        if (fabs(prev_lon - fwd_lons[i]) > 1e-6) {
            fprintf(stderr, "MISMATCH at index %ld: lon forward=%.10f backward=%.10f\n",
                    i, fwd_lons[i], prev_lon);
            return 1;
        }
        if (fabs(prev_val - fwd_vals[i]) > 1e-6) {
            fprintf(stderr, "MISMATCH at index %ld: value forward=%.10f backward=%.10f\n",
                    i, fwd_vals[i], prev_val);
            return 1;
        }
        matches++;
    }
    ECCODES_ASSERT(matches == numberOfDataPoints);

    printf("OK: forward and backward iteration match for all %ld points\n", numberOfDataPoints);

    grib_iterator_delete(iter);
    grib_handle_delete(h);
    free(values);
    free(fwd_lats);
    free(fwd_lons);
    free(fwd_vals);

    return 0;
}
