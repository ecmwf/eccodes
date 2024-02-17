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
#include "eccodes.h"

static void dump_it(grib_handle* h)
{
    int dump_flags = GRIB_DUMP_FLAG_CODED | GRIB_DUMP_FLAG_OCTET | GRIB_DUMP_FLAG_VALUES | GRIB_DUMP_FLAG_READ_ONLY;
    grib_dump_content(h, stdout, "wmo", dump_flags, NULL);
}

// Lambert conformal
static void test0()
{
    int err = 0;
    grib_handle* finalh     = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4] = {1.1, 2.2, 3.3, 0.4};
    int set_spec_flags = 0;
    size_t outlen = 4;

    grib_handle* handle = grib_handle_new_from_samples(0, "GRIB2");
    grib_set_long(handle, "tablesVersion", 32);
    spec.grid_type = GRIB_UTIL_GRID_SPEC_LAMBERT_CONFORMAL;
    spec.N = 2;

    // packing_spec.packing_type = GRIB_UTIL_PACKING_TYPE_GRID_SIMPLE;
    // packing_spec.bitsPerValue = 16;
    // packing_spec.accuracy     = GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
    // packing_spec.packing  = GRIB_UTIL_PACKING_USE_PROVIDED;

    finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    Assert(finalh);
    Assert(err == 0);
    dump_it(finalh);
}

// Lambert azimuthal
static void test1()
{
    int err = 0;
    grib_handle* finalh     = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4] = {1.1, 2.2, 3.3, 0.4};
    int set_spec_flags = 0;
    size_t outlen = 4;

    grib_handle* handle = grib_handle_new_from_samples(0, "GRIB2");
    grib_set_long(handle, "tablesVersion", 32);
    spec.grid_type = GRIB_UTIL_GRID_SPEC_LAMBERT_AZIMUTHAL_EQUAL_AREA;

    finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    Assert(finalh);
    Assert(err == 0);
    dump_it(finalh);
}

// HEALPix
static void test2()
{
    int err = 0;
    grib_handle* finalh     = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4] = {1.1, 2.2, 3.3, 0.4};
    int set_spec_flags = 0;
    size_t outlen = 0;

    grib_handle* handle = grib_handle_new_from_samples(0, "GRIB2");
    grib_set_long(handle, "tablesVersion", 32);
    spec.grid_type = GRIB_UTIL_GRID_SPEC_HEALPIX;
    spec.N = 2;
    outlen = 4;

    packing_spec.packing_type = GRIB_UTIL_PACKING_TYPE_GRID_SIMPLE;
    packing_spec.bitsPerValue = 16;
    packing_spec.accuracy = GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
    packing_spec.packing  = GRIB_UTIL_PACKING_USE_PROVIDED;

    finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    Assert(finalh);
    Assert(err == 0);
    dump_it(finalh);
}

// Spherical harmonics
static void test3()
{
    int err = 0;
    grib_handle* finalh     = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4] = {1.1, 2.2};
    int set_spec_flags = 0;
    size_t outlen = 0;

    grib_handle* handle = grib_handle_new_from_samples(0, "sh_pl_grib2");
    grib_set_long(handle, "tablesVersion", 32);
    spec.grid_type = GRIB_UTIL_GRID_SPEC_SH;
    spec.truncation = 20;
    outlen = 2;

    packing_spec.packing_type = GRIB_UTIL_PACKING_TYPE_SPECTRAL_SIMPLE;
    packing_spec.bitsPerValue = 16;
    packing_spec.accuracy = GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
    packing_spec.packing  = GRIB_UTIL_PACKING_USE_PROVIDED;

    finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    Assert(finalh);
    Assert(err == 0);
    dump_it(finalh);
}

// Polar stereo
static void test4()
{
    int err = 0;
    grib_handle* finalh     = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4] = {1.1, 2.2, 3.3, 0.4};
    int set_spec_flags = 0;
    size_t outlen = 0;

    grib_handle* handle = grib_handle_new_from_samples(0, "GRIB2");
    grib_set_long(handle, "tablesVersion", 32);
    spec.grid_type = GRIB_UTIL_GRID_SPEC_POLAR_STEREOGRAPHIC;
    outlen = 4;

    finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    Assert(finalh);
    Assert(err == 0);
    dump_it(finalh);
}

int main()
{
    test0();
    test1();
    test2();
    test3();
    test4();
    return 0;
}

#if 0
int main(int argc, char* argv[])
{
    int i = 0, remove_local_def = 0;
    int edition        = 0;
    char* packingType  = NULL;
    const char* prog   = argv[0];
    char* infile_name  = NULL;
    char* outfile_name = NULL;

    if (argc == 1 || argc > 8) usage(prog);

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            packingType = argv[i + 1];
            ++i;
        }
        else if (strcmp(argv[i], "-e") == 0) {
            edition = atoi(argv[i + 1]);
            ++i;
        }
        else if (strcmp(argv[i], "-r") == 0) {
            remove_local_def = 1;
        }
        else {
            /* Expect 2 filenames */
            infile_name  = argv[i];
            outfile_name = argv[i + 1];
            break;
        }
    }
#if 0
    printf("DEBUG remove_local_def = %d\n", remove_local_def);
    printf("DEBUG edition          = %d\n", edition);
    printf("DEBUG packingType      = %s\n", packingType);
    printf("DEBUG infile_name      = %s\n", infile_name);
    printf("DEBUG outfile_name     = %s\n", outfile_name);
#endif
    test_regular_ll(remove_local_def, edition, packingType, infile_name, outfile_name);
    test_reduced_gg(remove_local_def, edition, packingType, infile_name, outfile_name);
    /*test_grid_complex_spatial_differencing(remove_local_def, edition, packingType, infile_name, outfile_name);*/

    return 0;
}
#endif
