/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stdio.h>
#include <stdlib.h>
#include "eccodes.h"

/* See JIRA issue GRIB-361 */
int main()
{
    const int ni = 2880;
    const int nj = 2880;
    double *values;
    int i = 0;
    codes_handle *h = NULL;
    const char* filename = "bigfile.grib";

    values = (double*) malloc(ni*nj*sizeof(double));
    if (!values) {
        printf ("Malloc failed\n");
        exit(1);
    }

    for (i=0; i<ni*nj; i++) {
        double rand = random();
        values[i] = 10*rand;
    }

    h = codes_grib_handle_new_from_samples(0, "GRIB1");

    CODES_CHECK(codes_set_long(h, "Ni", ni), 0);
    CODES_CHECK(codes_set_long(h, "Nj", nj), 0);
    CODES_CHECK(codes_set_long(h, "centre", 86), 0);
    CODES_CHECK(codes_set_long(h, "process", 100), 0);
    CODES_CHECK(codes_set_long(h, "indicatorOfTypeOfLevel", 105), 0);
    CODES_CHECK(codes_set_long(h, "level", 2), 0);
    CODES_CHECK(codes_set_long(h, "indicatorOfParameter", 1), 0);
    CODES_CHECK(codes_set_long(h, "table2Version", 1), 0);
    CODES_CHECK(codes_set_long(h, "dataDate", 20130424), 0);
    CODES_CHECK(codes_set_long(h, "dataTime", 0), 0);
    CODES_CHECK(codes_set_long(h, "startStep", 0), 0);
    CODES_CHECK(codes_set_long(h, "endStep", 0), 0);

    CODES_CHECK(codes_set_long(h, "bitmapPresent", 1), 0);

    CODES_CHECK(codes_set_double(h, "iDirectionIncrementInDegrees", 0.125), 0);
    CODES_CHECK(codes_set_double(h, "jDirectionIncrementInDegrees", 0.125), 0);
    CODES_CHECK(codes_set_long(h,   "iScansNegatively", 0), 0);
    CODES_CHECK(codes_set_long(h,   "jScansPositively", 1), 0);

    CODES_CHECK(codes_set_double(h, "latitudeOfFirstGridPointInDegrees", -90), 0);
    CODES_CHECK(codes_set_double(h, "latitudeOfLastGridPointInDegrees", 90), 0);
    CODES_CHECK(codes_set_double(h, "longitudeOfFirstGridPointInDegrees", -180), 0);
    CODES_CHECK(codes_set_double(h, "longitudeOfLastGridPointInDegrees", 180), 0);

    CODES_CHECK(codes_set_double_array(h, "values", values, ni*nj), 0);

    codes_write_message(h, filename, "w");
    /*printf("Wrote file %s\n", filename);*/

    codes_handle_delete(h);
    free (values);

    return 0;
}
