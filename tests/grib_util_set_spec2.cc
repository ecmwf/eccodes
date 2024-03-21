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
    grib_dump_content(h, stderr, "wmo", dump_flags, NULL);
}

// Lambert conformal
static grib_handle* test0()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4] = {1.1, 2.2, 3.3, 0.4};
    int set_spec_flags = 0;
    size_t outlen = 4;

    grib_handle* handle = grib_handle_new_from_samples(0, "GRIB1");
    spec.grid_type = GRIB_UTIL_GRID_SPEC_LAMBERT_CONFORMAL;
    spec.Ni = 2;
    spec.Nj = 2;

    // packing_spec.packing_type = GRIB_UTIL_PACKING_TYPE_GRID_SIMPLE;
    // packing_spec.bitsPerValue = 16;
    // packing_spec.accuracy = GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
    // packing_spec.packing  = GRIB_UTIL_PACKING_USE_PROVIDED;

    grib_handle* finalh = codes_grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    Assert(err == 0);
    return finalh;
}

// Lambert azimuthal
static grib_handle* test1()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4] = {1.1, 2.2, 3.3, 0.4};
    int set_spec_flags = 0;
    size_t outlen = 4;

    grib_handle* handle = grib_handle_new_from_samples(0, "GRIB2");
    grib_set_long(handle, "tablesVersion", 32);
    spec.grid_type = GRIB_UTIL_GRID_SPEC_LAMBERT_AZIMUTHAL_EQUAL_AREA;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    Assert(err == 0);
    return finalh;
}

// HEALPix
static grib_handle* test2()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4] = {1.1, 2.2, 3.3, 0.4};
    int set_spec_flags = 0;
    size_t outlen = 4;

    grib_handle* handle = grib_handle_new_from_samples(0, "GRIB1");
    spec.grid_type = GRIB_UTIL_GRID_SPEC_HEALPIX;
    spec.N = 2;

    packing_spec.packing_type = GRIB_UTIL_PACKING_TYPE_GRID_SECOND_ORDER;
    packing_spec.packing  = GRIB_UTIL_PACKING_USE_PROVIDED;
    packing_spec.editionNumber = 2;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    Assert(err == 0);
    return finalh;
}

// Spherical harmonics
static grib_handle* test3()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4] = {1.1, 2.2};
    int set_spec_flags = 0;
    size_t outlen = 0;

    grib_handle* handle = grib_handle_new_from_samples(0, "sh_pl_grib2");
    spec.grid_type = GRIB_UTIL_GRID_SPEC_SH;
    spec.truncation = 20;
    outlen = 2;

    packing_spec.packing_type = GRIB_UTIL_PACKING_TYPE_SPECTRAL_SIMPLE;
    packing_spec.bitsPerValue = 16;
    packing_spec.accuracy = GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
    packing_spec.packing  = GRIB_UTIL_PACKING_USE_PROVIDED;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    Assert(err == 0);
    return finalh;
}

// Polar stereo
static grib_handle* test4()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4] = {1.1, 2.2, 3.3, 0.4};
    int set_spec_flags = 0;
    size_t outlen = 0;

    grib_handle* handle = grib_handle_new_from_samples(0, "GRIB2");
    //grib_set_long(handle, "tablesVersion", 32);
    spec.grid_type = GRIB_UTIL_GRID_SPEC_POLAR_STEREOGRAPHIC;
    outlen = 4;

    packing_spec.extra_settings_count = 1;
    packing_spec.extra_settings[0].type = GRIB_TYPE_LONG;
    packing_spec.extra_settings[0].name = "tablesVersion";
    packing_spec.extra_settings[0].long_value = 32;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    Assert(err == 0);
    return finalh;
}

// Regular Gaussian
static grib_handle* test5()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4] = {1.1, 2.2, 3.3, 0.4};
    int set_spec_flags = 0;
    size_t outlen = 0;

    grib_handle* handle = grib_handle_new_from_samples(0, "GRIB2");
    spec.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_GG;
    spec.Ni = spec.Nj = 2;
    outlen = 4;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    Assert(err == 0);
    return finalh;
}

// Reduced LL
static grib_handle* test6()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4] = {1.1, 2.2, 3.3, 0.4};
    int set_spec_flags = 0;
    size_t outlen = 0;

    grib_handle* handle = grib_handle_new_from_samples(0, "GRIB2");
    spec.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_LL;
    spec.Nj = 2;
    outlen = 4;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    Assert(err == 0);
    return finalh;
}

// Unstructured
static grib_handle* test7()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4] = {1.1, 2.2, 3.3, 0.4};
    int set_spec_flags = 0;
    size_t outlen = 4;

    grib_handle* handle = grib_handle_new_from_samples(0, "GRIB2");
    spec.grid_type = GRIB_UTIL_GRID_SPEC_UNSTRUCTURED;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    Assert(err == 0);
    return finalh;
}

int main()
{
    typedef grib_handle* (*test_func)(void);
    test_func funcs[] = {test0, test1, test2, test3, test4, test5, test6, test7};

    //grib_handle* (*p[8]) (void) = {test0, test1, test2, test3, test4, test5, test6, test7};

    const size_t num_tests = sizeof(funcs)/sizeof(funcs[0]);
    for (size_t i = 0; i < num_tests; i++) {
        grib_handle* result = funcs[i]();
        Assert(result);
        dump_it(result);
    }
    return 0;
}
