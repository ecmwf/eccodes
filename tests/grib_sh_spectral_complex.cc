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

#include "grib_sh_values.h" // array 'values' defined here

#define ILCHAM 992
#define MTRONC 30
#define STRONC 10
#define TOLERANCE 1e-5

int main(int argc, char* argv[])
{
    size_t len, size;
    grib_handle* h = NULL;
    double zval[ILCHAM];
    int i, m, n, k, err;
    FILE* fin = NULL;
    FILE* fout = NULL;
    const void* buffer = NULL;
    char* in_filename = NULL;
    char* out_filename = NULL;
    char packingType[50] = {0,};
    bool do_compare = true;

    Assert(argc == 3);
    in_filename = argv[1];
    out_filename = argv[2];

    printf("Opening file '%s'...\n", in_filename);
    fin = fopen(in_filename, "rb");
    Assert(fin);

    h = grib_handle_new_from_file(0, fin, &err);
    Assert(h);
    Assert(!err);

    len = sizeof(packingType);
    GRIB_CHECK(grib_get_string(h, "packingType", packingType, &len), 0);
    Assert( STR_EQUAL(packingType, "spectral_complex") || STR_EQUAL(packingType, "spectral_simple") );

    GRIB_CHECK(grib_set_long(h, "pentagonalResolutionParameterJ", MTRONC), 0);
    GRIB_CHECK(grib_set_long(h, "pentagonalResolutionParameterK", MTRONC), 0);
    GRIB_CHECK(grib_set_long(h, "pentagonalResolutionParameterM", MTRONC), 0);

    GRIB_CHECK(grib_set_long(h, "bitsPerValue", 16), 0);

    if (STR_EQUAL(packingType, "spectral_complex")) {
        GRIB_CHECK(grib_set_long(h, "subSetJ", STRONC), 0);
        GRIB_CHECK(grib_set_long(h, "subSetK", STRONC), 0);
        GRIB_CHECK(grib_set_long(h, "subSetM", STRONC), 0);
        GRIB_CHECK(grib_set_long(h, "unpackedSubsetPrecision", 1), 0);
    } else {
        do_compare = false;
    }

    printf("Encode values...\n");
    GRIB_CHECK(grib_set_double_array(h, "values", values, ILCHAM), 0);

    // Write to a temporary GRIB file
    printf("Save to file '%s'...\n", out_filename);
    fout = fopen(out_filename, "wb");
    GRIB_CHECK(grib_get_message(h, &buffer, &size), 0);
    if (fwrite(buffer, 1, size, fout) != size) {
        Assert(!"Failed to write data");
    }
    fclose(fout);

    if (do_compare) {
        printf("Decode values and compare...\n");
        len = ILCHAM;
        GRIB_CHECK(grib_get_double_array(h, "values", zval, &len), 0);

        // Compare our values
        for (i = 0; i < ILCHAM; ++i) {
            const double diff = fabs(zval[i] - values[i]);
            if (diff > TOLERANCE) {
                fprintf(stderr, "Unpacked value different: i=%d values[i]=%.10g zval[i]=%.10g (diff=%.10g)\n", i, values[i], zval[i], diff);
                return 1;
            }
        }

        for (m = 0, k = 0; m < MTRONC + 1; m++) {
            for (n = m; n < MTRONC + 1; k++, n++) {
                // Check sub-truncation was fully preserved in IEEE-32
                if ((m < STRONC + 1) && (n < STRONC + 1) && (((float)zval[2 * k] != (float)values[2 * k]) || ((float)zval[2 * k + 1] != (float)values[2 * k + 1]))) {
                    printf("Unpacked sub-truncation was not fully preserved; coefficients for wave number (m=%d,n=%d) have been modified\n", m, n);
                    return 1;
                }
            }
        }
    }
    GRIB_CHECK(grib_handle_delete(h), 0);
    fclose(fin);

    // Read in the saved GRIB file
    if (do_compare) {
        printf("Load values from saved file and compare....\n");
        fin = fopen(out_filename, "rb"); Assert(fin);
        h = grib_handle_new_from_file(0, fin, &err); Assert(h);
        GRIB_CHECK(grib_get_double_array(h, "values", zval, &len), 0);
        for (i = 0; i < ILCHAM; ++i) {
            const double diff = fabs(zval[i] - values[i]);
            if (diff > TOLERANCE) {
                fprintf(stderr, "Unpacked value different: i=%d values[i]=%.10g zval[i]=%.10g (diff=%.10g)\n", i, values[i], zval[i], diff);
                return 1;
            }
        }
        GRIB_CHECK(grib_handle_delete(h), 0);
        fclose(fin);
    }

    printf("OK\n");

    return 0;
}
