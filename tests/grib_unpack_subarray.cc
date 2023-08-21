/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

int main(int argc, char** argv)
{
    Assert(argc == 2);

    int err              = 0;
    size_t nvalues       = 0;
    const char* filename = argv[1];
    grib_context* c      = grib_context_get_default();

    FILE* fin = fopen(filename, "r");
    Assert(fin);
    grib_handle* h = grib_handle_new_from_file(0, fin, &err);
    Assert(h);
    Assert(!err);

    grib_accessor* a = grib_find_accessor(h, "codedValues");
    Assert(a);
    GRIB_CHECK(grib_get_size(h, "codedValues", &nvalues), 0);
    double* values = (double*)grib_context_malloc(c, sizeof(double) * nvalues);
    Assert(values);

    size_t start = nvalues / 10;
    size_t len   = nvalues / 5;
    printf("nvalues=%zu, start=%zu, len=%zu\n", nvalues, start, len);
    GRIB_CHECK(grib_unpack_double_subarray(a, values, start, len), 0);
    for (size_t i = 0; i < len; ++i) {
        printf("v[%zu]=%.10e\n", start + i, values[i]);
    }

    grib_context_free(c, values);
    grib_handle_delete(h);
    fclose(fin);

    return 0;
}
