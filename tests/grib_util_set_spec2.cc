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
    grib_dump_content(h, stderr, "wmo", 0, nullptr);
}

// Lambert conformal conic (edition=1)
static grib_handle* lamb_conf_grib1()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    int set_spec_flags = 0;

    size_t outlen  = 0; // test passing in no values
    double* values = NULL;

    auto* handle   = grib_handle_new_from_samples(nullptr, "GRIB1");
    spec.grid_type = GRIB_UTIL_GRID_SPEC_LAMBERT_CONFORMAL;
    spec.Ni        = 2;
    spec.Nj        = 2;

    spec.longitudeOfFirstGridPointInDegrees = -1.;

    // packing_spec.packing_type = GRIB_UTIL_PACKING_TYPE_GRID_SIMPLE;
    // packing_spec.bitsPerValue = 16;
    // packing_spec.accuracy = GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
    // packing_spec.packing  = GRIB_UTIL_PACKING_USE_PROVIDED;

    grib_handle* finalh = codes_grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    ECCODES_ASSERT(err == 0);
    return finalh;
}

// Lambert conformal conic (edition=2)
static grib_handle* lamb_conf_grib2()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4]   = { 1.1, 2.2, 3.3, 0.4 };
    int set_spec_flags = 0;
    size_t outlen      = 4;

    auto* handle   = grib_handle_new_from_samples(nullptr, "GRIB2");
    spec.grid_type = GRIB_UTIL_GRID_SPEC_LAMBERT_CONFORMAL;
    spec.Ni        = 2;
    spec.Nj        = 2;

    // Note: For Lambert: In GRIB1 the longitude can be -ve but not in GRIB2.
    // So codes_grib_util_set_spec should cater for this case
    spec.longitudeOfFirstGridPointInDegrees = -1.;

    // packing_spec.packing_type = GRIB_UTIL_PACKING_TYPE_GRID_SIMPLE;
    // packing_spec.bitsPerValue = 16;
    // packing_spec.accuracy = GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
    // packing_spec.packing  = GRIB_UTIL_PACKING_USE_PROVIDED;

    grib_handle* finalh = codes_grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    ECCODES_ASSERT(err == 0);
    return finalh;
}

// Lambert azimuthal equal area
static grib_handle* lamb_az()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4]   = { 1.1, 2.2, 3.3, 0.4 };
    int set_spec_flags = 0;
    size_t outlen      = 4;

    auto* handle = grib_handle_new_from_samples(nullptr, "GRIB2");
    grib_set_long(handle, "tablesVersion", 32);
    spec.grid_type = GRIB_UTIL_GRID_SPEC_LAMBERT_AZIMUTHAL_EQUAL_AREA;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    ECCODES_ASSERT(err == 0);
    return finalh;
}

// HEALPix
static grib_handle* healpix()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4]   = { 1.1, 2.2, 3.3, 0.4 };
    int set_spec_flags = 0;
    size_t outlen      = 4;

    auto* handle   = grib_handle_new_from_samples(nullptr, "GRIB1");
    spec.grid_type = GRIB_UTIL_GRID_SPEC_HEALPIX;
    spec.N         = 2;

    packing_spec.packing_type  = GRIB_UTIL_PACKING_TYPE_GRID_SECOND_ORDER;
    packing_spec.packing       = GRIB_UTIL_PACKING_USE_PROVIDED;
    packing_spec.editionNumber = 2;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    ECCODES_ASSERT(err == 0);
    return finalh;
}

// Spherical harmonics
static grib_handle* spectral()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    int set_spec_flags = 0;

    size_t outlen  = 0; // test passing in no values
    double* values = NULL;

    auto* handle    = grib_handle_new_from_samples(nullptr, "sh_pl_grib2");
    spec.grid_type  = GRIB_UTIL_GRID_SPEC_SH;
    spec.truncation = 20;

    packing_spec.packing_type = GRIB_UTIL_PACKING_TYPE_SPECTRAL_SIMPLE; //must fail
    packing_spec.bitsPerValue = 16;
    packing_spec.accuracy     = GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
    packing_spec.packing      = GRIB_UTIL_PACKING_USE_PROVIDED;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    ECCODES_ASSERT(err);
    ECCODES_ASSERT(!finalh);

    return handle;
}

// Polar stereo
static grib_handle* polar_stereo()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4]   = { 1.1, 2.2, 3.3, 0.4 };
    int set_spec_flags = 0;
    size_t outlen      = 0;

    auto* handle = grib_handle_new_from_samples(nullptr, "GRIB2");
    // grib_set_long(handle, "tablesVersion", 32);
    spec.grid_type = GRIB_UTIL_GRID_SPEC_POLAR_STEREOGRAPHIC;
    outlen         = 4;
    spec.longitudeOfFirstGridPointInDegrees = -1.;

    packing_spec.extra_settings_count         = 1;
    packing_spec.extra_settings[0].type       = GRIB_TYPE_LONG;
    packing_spec.extra_settings[0].name       = "tablesVersion";
    packing_spec.extra_settings[0].long_value = 32;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    ECCODES_ASSERT(err == 0);
    return finalh;
}

// Regular Gaussian
static grib_handle* regular_gg()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4]   = { 1.1, 2.2, 3.3, 0.4 };
    int set_spec_flags = 0;
    size_t outlen      = 0;

    auto* handle   = grib_handle_new_from_samples(nullptr, "GRIB2");
    spec.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_GG;
    spec.Ni = spec.Nj = 2;
    outlen            = 4;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    ECCODES_ASSERT(err == 0);
    return finalh;
}

// Reduced LL
static grib_handle* reduced_ll()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4]   = { 1.1, 2.2, 3.3, 0.4 };
    int set_spec_flags = 0;
    size_t outlen      = 0;

    auto* handle   = grib_handle_new_from_samples(nullptr, "GRIB2");
    spec.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_LL;
    spec.Nj        = 2;
    outlen         = 4;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    ECCODES_ASSERT(err == 0);
    return finalh;
}

// Unstructured
static grib_handle* unstruct()
{
    fprintf(stderr, "Doing test %s\n-----------------\n", __func__);
    int err = 0;
    grib_util_grid_spec spec = {0,};
    grib_util_packing_spec packing_spec = {0,};
    double values[4]   = { 1.1, 2.2, 3.3, 0.4 };
    int set_spec_flags = 0;
    size_t outlen      = 4;

    auto* handle   = grib_handle_new_from_samples(nullptr, "GRIB2");
    size_t alen = 1000;
    CODES_CHECK(codes_set_string(handle, "gridType", "unstructured_grid", &alen), 0);
    CODES_CHECK(codes_set_long(handle, "numberOfGridInReference", 4), 0);
    CODES_CHECK(codes_set_long(handle, "numberOfGridUsed", 2), 0);
    CODES_CHECK(codes_set_long(handle, "shapeOfTheEarth", 6), 0);

    spec.grid_type = GRIB_UTIL_GRID_SPEC_UNSTRUCTURED;

    grib_handle* finalh = grib_util_set_spec(
        handle, &spec, &packing_spec, set_spec_flags,
        values, outlen, &err);
    ECCODES_ASSERT(err == 0);

    long numberOfGridInReference=-1, numberOfGridUsed=-1, shapeOfTheEarth=-1;
    CODES_CHECK(codes_get_long(finalh, "numberOfGridInReference", &numberOfGridInReference), 0);
    ECCODES_ASSERT( numberOfGridInReference == 4 );
    CODES_CHECK(codes_get_long(finalh, "numberOfGridUsed", &numberOfGridUsed), 0);
    ECCODES_ASSERT( numberOfGridUsed == 2 );

    CODES_CHECK(codes_get_long(finalh, "shapeOfTheEarth", &shapeOfTheEarth), 0);
    ECCODES_ASSERT( shapeOfTheEarth == 6 );

    return finalh;
}

int main()
{
    // using test_func = grib_handle* (*)();
    for (auto func : { lamb_conf_grib1, lamb_conf_grib2, lamb_az, healpix, spectral, polar_stereo, regular_gg, reduced_ll, unstruct }) {
        auto* result = func();
        ECCODES_ASSERT(result != nullptr);
        dump_it(result);
    }

    return 0;
}
