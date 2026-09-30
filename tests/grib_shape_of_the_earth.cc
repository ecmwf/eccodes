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
 * The earth size keys in template.3.shape_of_the_earth.def are created
 * conditionally on shapeOfTheEarth. This checks that they follow the shape:
 * changing shapeOfTheEarth must rebuild 'radius' (and the oblate spheroid
 * axes) rather than leaving the value the handle was created with.
 */

#undef NDEBUG
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

#include "eccodes.h"

static int failures = 0;

static void fail(const char* what, const char* fmt, ...)
{
    va_list ap;
    fprintf(stderr, "FAIL (%s): ", what);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    ++failures;
}

static codes_handle* new_sample()
{
    codes_handle* h = codes_grib_handle_new_from_samples(0, "GRIB2");
    assert(h);
    return h;
}

/*
 * Encode the handle and read it back. A handle built from an existing
 * message is what a caller decoding a GRIB file actually gets, and it is
 * the state the conditional keys must describe.
 */
static codes_handle* encode_and_reread(codes_handle* h)
{
    const void* buffer = NULL;
    size_t size        = 0;
    CODES_CHECK(codes_get_message(h, &buffer, &size), 0);
    codes_handle* reread = codes_handle_new_from_message_copy(0, buffer, size);
    assert(reread);
    return reread;
}

/* Read a double key, returning the eccodes error code */
static int get_double(codes_handle* h, const char* key, double* value)
{
    *value = 0;
    return codes_get_double(h, key, value);
}

/* The radius values are exact in double precision, but compare with a
 * tolerance so a future scaled/computed representation does not trip us up */
static int same(double a, double b)
{
    return fabs(a - b) < 1e-6;
}

/* Each spherical shape has its own fixed radius (Code table 3.2) */
static void check_spherical(long shape, double expected)
{
    codes_handle* sample = new_sample();
    double radius        = 0;
    int err              = 0;

    CODES_CHECK(codes_set_long(sample, "shapeOfTheEarth", shape), 0);
    codes_handle* h = encode_and_reread(sample);
    codes_handle_delete(sample);

    err = get_double(h, "radius", &radius);
    if (err) {
        fail("spherical", "shapeOfTheEarth=%ld: radius not found (%s)",
             shape, codes_get_error_message(err));
    }
    else if (!same(radius, expected)) {
        fail("spherical", "shapeOfTheEarth=%ld: radius=%.10g, expected %.10g",
             shape, radius, expected);
    }

    /* A spherical earth is not oblate, and has no axes */
    long oblate = -1;
    CODES_CHECK(codes_get_long(h, "earthIsOblate", &oblate), 0);
    if (oblate != 0)
        fail("spherical", "shapeOfTheEarth=%ld: earthIsOblate=%ld, expected 0", shape, oblate);

    double axis = 0;
    if (get_double(h, "earthMajorAxis", &axis) == 0)
        fail("spherical", "shapeOfTheEarth=%ld: earthMajorAxis should not exist", shape);

    codes_handle_delete(h);
}

/* Each oblate shape has major/minor axes and no radius */
static void check_oblate(long shape, double major, double minor)
{
    codes_handle* sample = new_sample();
    double value         = 0;
    int err              = 0;

    CODES_CHECK(codes_set_long(sample, "shapeOfTheEarth", shape), 0);
    codes_handle* h = encode_and_reread(sample);
    codes_handle_delete(sample);

    err = get_double(h, "earthMajorAxis", &value);
    if (err)
        fail("oblate", "shapeOfTheEarth=%ld: earthMajorAxis not found (%s)",
             shape, codes_get_error_message(err));
    else if (!same(value, major))
        fail("oblate", "shapeOfTheEarth=%ld: earthMajorAxis=%.10g, expected %.10g",
             shape, value, major);

    err = get_double(h, "earthMinorAxis", &value);
    if (err)
        fail("oblate", "shapeOfTheEarth=%ld: earthMinorAxis not found (%s)",
             shape, codes_get_error_message(err));
    else if (!same(value, minor))
        fail("oblate", "shapeOfTheEarth=%ld: earthMinorAxis=%.10g, expected %.10g",
             shape, value, minor);

    long oblate = -1;
    CODES_CHECK(codes_get_long(h, "earthIsOblate", &oblate), 0);
    if (oblate != 1)
        fail("oblate", "shapeOfTheEarth=%ld: earthIsOblate=%ld, expected 1", shape, oblate);

    /* Switching to an oblate spheroid must remove 'radius' altogether */
    if (get_double(h, "radius", &value) == 0)
        fail("oblate", "shapeOfTheEarth=%ld: radius should not exist (got %.10g)", shape, value);

    codes_handle_delete(h);
}

/*
 * The main check: read the shape and radius, change the shape, and the
 * radius read back from the same handle must have changed with it.
 */
static void check_radius_follows_shape()
{
    codes_handle* h = new_sample();
    long shape1     = 0;
    double radius1  = 0;

    CODES_CHECK(codes_get_long(h, "shapeOfTheEarth", &shape1), 0);
    CODES_CHECK(get_double(h, "radius", &radius1), 0);

    /* Pick a different spherical shape */
    const long shape2 = (shape1 == 8) ? 0 : 8;

    CODES_CHECK(codes_set_long(h, "shapeOfTheEarth", shape2), 0);

    double radius2 = 0;
    CODES_CHECK(get_double(h, "radius", &radius2), 0);

    if (same(radius1, radius2)) {
        fail("radius follows shape",
             "shapeOfTheEarth %ld -> %ld left radius unchanged at %.10g",
             shape1, shape2, radius1);
    }

    /* And the shape really is the one we asked for */
    long shape_now = 0;
    CODES_CHECK(codes_get_long(h, "shapeOfTheEarth", &shape_now), 0);
    if (shape_now != shape2)
        fail("radius follows shape", "shapeOfTheEarth=%ld, expected %ld", shape_now, shape2);

    codes_handle_delete(h);
}

/* Shape 1: the radius is supplied by the data producer */
static void check_producer_radius()
{
    codes_handle* h = new_sample();
    double radius   = 0;

    CODES_CHECK(codes_set_long(h, "shapeOfTheEarth", 1), 0);
    CODES_CHECK(codes_set_long(h, "scaleFactorOfRadiusOfSphericalEarth", 0), 0);
    CODES_CHECK(codes_set_long(h, "scaledValueOfRadiusOfSphericalEarth", 6371000), 0);

    CODES_CHECK(get_double(h, "radius", &radius), 0);
    if (!same(radius, 6371000.0))
        fail("producer radius", "radius=%.10g, expected 6371000", radius);

    codes_handle_delete(h);
}

/*
 * Shapes 3 and 7 both take the axes from the producer, but shape 3 gives
 * them in km and shape 7 in metres. Only the *InMetres keys differ.
 */
static void check_axis_units()
{
    struct
    {
        long shape;
        double major_in_metres;
        double minor_in_metres;
    } cases[] = {
        { 3, 6378137.0, 6356752.0 }, /* km in, metres out */
        { 7, 6378.137, 6356.752 },   /* metres in, metres out (unscaled) */
    };

    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        codes_handle* h = new_sample();
        double value    = 0;

        CODES_CHECK(codes_set_long(h, "shapeOfTheEarth", cases[i].shape), 0);
        CODES_CHECK(codes_set_long(h, "scaleFactorOfEarthMajorAxis", 3), 0);
        CODES_CHECK(codes_set_long(h, "scaledValueOfEarthMajorAxis", 6378137), 0);
        CODES_CHECK(codes_set_long(h, "scaleFactorOfEarthMinorAxis", 3), 0);
        CODES_CHECK(codes_set_long(h, "scaledValueOfEarthMinorAxis", 6356752), 0);

        CODES_CHECK(get_double(h, "earthMajorAxisInMetres", &value), 0);
        if (!same(value, cases[i].major_in_metres))
            fail("axis units", "shapeOfTheEarth=%ld: earthMajorAxisInMetres=%.10g, expected %.10g",
                 cases[i].shape, value, cases[i].major_in_metres);

        CODES_CHECK(get_double(h, "earthMinorAxisInMetres", &value), 0);
        if (!same(value, cases[i].minor_in_metres))
            fail("axis units", "shapeOfTheEarth=%ld: earthMinorAxisInMetres=%.10g, expected %.10g",
                 cases[i].shape, value, cases[i].minor_in_metres);

        codes_handle_delete(h);
    }
}

int main(int argc, char* argv[])
{
    check_radius_follows_shape();

    /* Spherical earth models */
    check_spherical(0, 6367470.0);
    check_spherical(6, 6371229.0);
    check_spherical(8, 6371200.0);
    check_producer_radius();

    /* Oblate spheroid earth models */
    check_oblate(2, 6378160.0, 6356775.0);  /* IAU 1965  */
    check_oblate(4, 6378137.0, 6356752.314); /* IAG-GRS80 */
    check_oblate(5, 6378137.0, 6356752.314); /* WGS84     */
    check_oblate(9, 6377563.396, 6356256.909); /* Airy 1830 */

    check_axis_units();

    if (failures) {
        fprintf(stderr, "%d check(s) failed\n", failures);
        return 1;
    }
    printf("All shapeOfTheEarth checks passed\n");
    return 0;
}
