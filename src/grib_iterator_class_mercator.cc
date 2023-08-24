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
#include <cmath>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = iterator
   SUPER      = grib_iterator_class_gen
   IMPLEMENTS = destroy
   IMPLEMENTS = init;next
   MEMBERS    = double *lats
   MEMBERS    = double *lons
   MEMBERS    = long Nj
   END_CLASS_DEF
*/

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "iterator.class" and rerun ./make_class.pl

*/


static void init_class              (grib_iterator_class*);

static int init               (grib_iterator* i,grib_handle*,grib_arguments*);
static int next               (grib_iterator* i, double *lat, double *lon, double *val);
static int destroy            (grib_iterator* i);


typedef struct grib_iterator_mercator{
  grib_iterator it;
    /* Members defined in gen */
    int carg;
    const char* missingValue;
    /* Members defined in mercator */
    double *lats;
    double *lons;
    long Nj;
} grib_iterator_mercator;

extern grib_iterator_class* grib_iterator_class_gen;

static grib_iterator_class _grib_iterator_class_mercator = {
    &grib_iterator_class_gen,                    /* super                     */
    "mercator",                    /* name                      */
    sizeof(grib_iterator_mercator),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    &destroy,                  /* destructor                */
    &next,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_mercator = &_grib_iterator_class_mercator;


static void init_class(grib_iterator_class* c)
{
    c->previous    =    (*(c->super))->previous;
    c->reset    =    (*(c->super))->reset;
    c->has_next    =    (*(c->super))->has_next;
}
/* END_CLASS_IMP */

#define ITER "Mercator Geoiterator"
#define EPSILON 1.0e-10

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* Whole pie */
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923 /* Half a pie */
#endif

#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962 /* Quarter of a pie */
#endif

#define RAD2DEG 57.29577951308232087684 /* 180 over pi */
#define DEG2RAD 0.01745329251994329576  /* pi over 180 */

/* Adjust longitude (in radians) to range -180 to 180 */
static double adjust_lon_radians(double lon)
{
    if (lon > M_PI)  lon -= 2 * M_PI;
    if (lon < -M_PI) lon += 2 * M_PI;
    return lon;
}

/* Function to compute the latitude angle, phi2, for the inverse
 * From the book "Map Projections-A Working Manual-John P. Snyder (1987)"
 * Equation (7-9) involves rapidly converging iteration: Calculate t from (15-11)
 * Then, assuming an initial trial phi equal to (pi/2 - 2*arctan t) in the right side of equation (7-9),
 * calculate phi on the left side. Substitute the calculated phi into the right side,
 * calculate a new phi, etc., until phi does not change significantly from the preceding trial value of phi
 */
static double compute_phi(
    double eccent, /* Spheroid eccentricity */
    double ts,     /* Constant value t */
    int* error)
{
    double eccnth, phi, con, dphi, sinpi;
    int i, MAX_ITER = 15;

    eccnth = 0.5 * eccent;
    phi    = M_PI_2 - 2 * atan(ts);
    for (i = 0; i <= MAX_ITER; i++) {
        sinpi = sin(phi);
        con   = eccent * sinpi;
        dphi  = M_PI_2 - 2 * atan(ts * (pow(((1.0 - con) / (1.0 + con)), eccnth))) - phi;
        phi += dphi;
        if (fabs(dphi) <= EPSILON)
            return phi;
    }
    *error = GRIB_INTERNAL_ERROR;
    return 0;
}

/* Compute the constant small t for use in the forward computations */
static double compute_t(
    double eccent, /* Eccentricity of the spheroid */
    double phi,    /* Latitude phi */
    double sinphi) /* Sine of the latitude */
{
    double con = eccent * sinphi;
    double com = 0.5 * eccent;
    con        = pow(((1.0 - con) / (1.0 + con)), com);
    return (tan(0.5 * (M_PI_2 - phi)) / con);
}

static int init_mercator(grib_handle* h,
                         grib_iterator_mercator* self,
                         size_t nv, long nx, long ny,
                         double DiInMetres, double DjInMetres,
                         double earthMinorAxisInMetres, double earthMajorAxisInMetres,
                         double latFirstInRadians, double lonFirstInRadians,
                         double latLastInRadians, double lonLastInRadians,
                         double LaDInRadians, double orientationInRadians)
{
    int i, j, err = 0;
    double x0, y0, x, y, latRad, lonRad, latDeg, lonDeg, sinphi, ts;
    double false_easting;  /* x offset in meters */
    double false_northing; /* y offset in meters */
    double m1;             /* small value m */
    double temp, e, es;

    temp = earthMinorAxisInMetres / earthMajorAxisInMetres;
    es   = 1.0 - (temp * temp);
    e    = sqrt(es);
    m1   = cos(LaDInRadians) / (sqrt(1.0 - es * sin(LaDInRadians) * sin(LaDInRadians)));

    /* Forward projection: convert lat,lon to x,y */
    if (fabs(fabs(latFirstInRadians) - M_PI_2) <= EPSILON) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Transformation cannot be computed at the poles", ITER);
        return GRIB_GEOCALCULUS_PROBLEM;
    }
    else {
        sinphi = sin(latFirstInRadians);
        ts     = compute_t(e, latFirstInRadians, sinphi);
        x0     = earthMajorAxisInMetres * m1 * adjust_lon_radians(lonFirstInRadians - orientationInRadians);
        y0     = 0 - earthMajorAxisInMetres * m1 * log(ts);
    }
    x0 = -x0;
    y0 = -y0;

    /* Allocate latitude and longitude arrays */
    self->lats = (double*)grib_context_malloc(h->context, nv * sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, nv * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    self->lons = (double*)grib_context_malloc(h->context, nv * sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, nv * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }

    /* Populate our arrays */
    false_easting  = x0;
    false_northing = y0;
    for (j = 0; j < ny; j++) {
        y = j * DiInMetres;
        for (i = 0; i < nx; i++) {
            const int index = i + j * nx;
            double _x, _y;
            x = i * DiInMetres;
            /* Inverse projection to convert from x,y to lat,lon */
            _x     = x - false_easting;
            _y     = y - false_northing;
            ts     = exp(-_y / (earthMajorAxisInMetres * m1));
            latRad = compute_phi(e, ts, &err);
            if (err) {
                grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Failed to compute the latitude angle, phi2, for the inverse", ITER);
                grib_context_free(h->context, self->lats);
                grib_context_free(h->context, self->lons);
                return err;
            }
            lonRad = adjust_lon_radians(orientationInRadians + _x / (earthMajorAxisInMetres * m1));
            if (i == 0 && j == 0) {
                DEBUG_ASSERT(fabs(latFirstInRadians - latRad) <= EPSILON);
            }
            latDeg            = latRad * RAD2DEG; /* Convert to degrees */
            lonDeg            = normalise_longitude_in_degrees(lonRad * RAD2DEG);
            self->lons[index] = lonDeg;
            self->lats[index] = latDeg;
        }
    }
    return GRIB_SUCCESS;
}

static int init(grib_iterator* iter, grib_handle* h, grib_arguments* args)
{
    int err = 0;
    long ni, nj, iScansNegatively, jScansPositively, jPointsAreConsecutive, alternativeRowScanning;
    double latFirstInDegrees, lonFirstInDegrees, LaDInDegrees;
    double latLastInDegrees, lonLastInDegrees, orientationInDegrees, DiInMetres, DjInMetres, radius = 0;
    double latFirstInRadians, lonFirstInRadians, latLastInRadians, lonLastInRadians,
        LaDInRadians, orientationInRadians;
    double earthMajorAxisInMetres = 0, earthMinorAxisInMetres = 0;

    grib_iterator_mercator* self = (grib_iterator_mercator*)iter;

    const char* sRadius               = grib_arguments_get_name(h, args, self->carg++);
    const char* sNi                   = grib_arguments_get_name(h, args, self->carg++);
    const char* sNj                   = grib_arguments_get_name(h, args, self->carg++);
    const char* sLatFirstInDegrees    = grib_arguments_get_name(h, args, self->carg++);
    const char* sLonFirstInDegrees    = grib_arguments_get_name(h, args, self->carg++);
    const char* sLaDInDegrees         = grib_arguments_get_name(h, args, self->carg++);
    const char* sLatLastInDegrees     = grib_arguments_get_name(h, args, self->carg++);
    const char* sLonLastInDegrees     = grib_arguments_get_name(h, args, self->carg++);
    const char* sOrientationInDegrees = grib_arguments_get_name(h, args, self->carg++);
    /* Dx and Dy are in Metres */
    const char* sDi                     = grib_arguments_get_name(h, args, self->carg++);
    const char* sDj                     = grib_arguments_get_name(h, args, self->carg++);
    const char* siScansNegatively       = grib_arguments_get_name(h, args, self->carg++);
    const char* sjScansPositively       = grib_arguments_get_name(h, args, self->carg++);
    const char* sjPointsAreConsecutive  = grib_arguments_get_name(h, args, self->carg++);
    const char* sAlternativeRowScanning = grib_arguments_get_name(h, args, self->carg++);

    if ((err = grib_get_long_internal(h, sNi, &ni)) != GRIB_SUCCESS) return err;
    if ((err = grib_get_long_internal(h, sNj, &nj)) != GRIB_SUCCESS) return err;

    if (grib_is_earth_oblate(h)) {
        if ((err = grib_get_double_internal(h, "earthMinorAxisInMetres", &earthMinorAxisInMetres)) != GRIB_SUCCESS) return err;
        if ((err = grib_get_double_internal(h, "earthMajorAxisInMetres", &earthMajorAxisInMetres)) != GRIB_SUCCESS) return err;
    }
    else {
        if ((err = grib_get_double_internal(h, sRadius, &radius)) != GRIB_SUCCESS) return err;
        earthMinorAxisInMetres = earthMajorAxisInMetres = radius;
    }

    if (iter->nv != ni * nj) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Wrong number of points (%zu!=%ldx%ld)", ITER, iter->nv, ni, nj);
        return GRIB_WRONG_GRID;
    }

    if ((err = grib_get_double_internal(h, sLaDInDegrees, &LaDInDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, sLatFirstInDegrees, &latFirstInDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, sLonFirstInDegrees, &lonFirstInDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, sLatLastInDegrees, &latLastInDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, sLonLastInDegrees, &lonLastInDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, sOrientationInDegrees, &orientationInDegrees)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_double_internal(h, sDi, &DiInMetres)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, sDj, &DjInMetres)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, sjPointsAreConsecutive, &jPointsAreConsecutive)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, sjScansPositively, &jScansPositively)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, siScansNegatively, &iScansNegatively)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, sAlternativeRowScanning, &alternativeRowScanning)) != GRIB_SUCCESS)
        return err;

    latFirstInRadians    = latFirstInDegrees * DEG2RAD;
    lonFirstInRadians    = lonFirstInDegrees * DEG2RAD;
    latLastInRadians     = latLastInDegrees * DEG2RAD;
    lonLastInRadians     = lonLastInDegrees * DEG2RAD;
    LaDInRadians         = LaDInDegrees * DEG2RAD;
    orientationInRadians = orientationInDegrees * DEG2RAD;

    err = init_mercator(h, self, iter->nv, ni, nj,
                        DiInMetres, DjInMetres, earthMinorAxisInMetres, earthMajorAxisInMetres,
                        latFirstInRadians, lonFirstInRadians,
                        latLastInRadians, lonLastInRadians,
                        LaDInRadians, orientationInRadians);
    if (err) return err;

    iter->e = -1;

    /* Apply the scanning mode flags which may require data array to be transformed */
    err = transform_iterator_data(h->context, iter->data,
                                  iScansNegatively, jScansPositively, jPointsAreConsecutive, alternativeRowScanning,
                                  iter->nv, ni, nj);
    return err;
}

static int next(grib_iterator* iter, double* lat, double* lon, double* val)
{
    grib_iterator_mercator* self = (grib_iterator_mercator*)iter;

    if ((long)iter->e >= (long)(iter->nv - 1))
        return 0;
    iter->e++;

    *lat = self->lats[iter->e];
    *lon = self->lons[iter->e];
    if (val && iter->data) {
        *val = iter->data[iter->e];
    }
    return 1;
}

static int destroy(grib_iterator* iter)
{
    grib_iterator_mercator* self = (grib_iterator_mercator*)iter;
    const grib_context* c        = iter->h->context;

    grib_context_free(c, self->lats);
    grib_context_free(c, self->lons);
    return GRIB_SUCCESS;
}
