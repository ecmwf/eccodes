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
   MEMBERS     =   double *lats
   MEMBERS     =   double *lons
   MEMBERS     =   long Nj
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


typedef struct grib_iterator_space_view{
  grib_iterator it;
    /* Members defined in gen */
    int carg;
    const char* missingValue;
    /* Members defined in space_view */
    double *lats;
    double *lons;
    long Nj;
} grib_iterator_space_view;

extern grib_iterator_class* grib_iterator_class_gen;

static grib_iterator_class _grib_iterator_class_space_view = {
    &grib_iterator_class_gen,                    /* super                     */
    "space_view",                    /* name                      */
    sizeof(grib_iterator_space_view),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    &destroy,                  /* destructor                */
    &next,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_space_view = &_grib_iterator_class_space_view;


static void init_class(grib_iterator_class* c)
{
    c->previous    =    (*(c->super))->previous;
    c->reset    =    (*(c->super))->reset;
    c->has_next    =    (*(c->super))->has_next;
}
/* END_CLASS_IMP */

#define ITER "Space view Geoiterator"

static int next(grib_iterator* iter, double* lat, double* lon, double* val)
{
    grib_iterator_space_view* self = (grib_iterator_space_view*)iter;

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

// static void adjustBadlyEncodedEcmwfGribs(grib_handle* h,
//                                     long* nx, long* ny, double* dx, double* dy, double* xp, double* yp)
// {
//     /* Correct the information provided in the headers of certain satellite imagery that
//      * we have available. This is specific to ECMWF.
//      * Obtained through trial-and-error to get the best match with the coastlines.
//      *
//      * Copied from Magics GribSatelliteInterpretor::AdjustBadlyEncodedGribs()
//      */
//     long centre = 0;
//     int err     = grib_get_long(h, "centre", &centre);
//     if (!err && centre == 98) {
//         int err1 = 0, err2 = 0, err3 = 0;
//         long satelliteIdentifier, channelNumber, functionCode;
//         /* These keys are defined in the ECMWF local definition 24 - Satellite image simulation */
//         err1 = grib_get_long(h, "satelliteIdentifier", &satelliteIdentifier);
//         err2 = grib_get_long(h, "channelNumber", &channelNumber);
//         err3 = grib_get_long(h, "functionCode", &functionCode);
//         if (!err1 && !err2 && !err3) {
//             if (satelliteIdentifier == 54 && channelNumber == 2 && *dx == 1179) { /* Meteosat 7, channel 2 */
//                 *nx = *ny = 900;
//                 *dx = *dy = 853;
//                 *xp = *yp = 450;
//             }
//             else if (satelliteIdentifier == 54 && channelNumber == 3 && *dx == 1179) { /* Meteosat 7, channel 3 */
//                 *dx = *dy = 1184;
//                 *xp = *yp = 635;
//             }
//             else if (satelliteIdentifier == 259 && channelNumber == 4 && *dx == 1185) { /* GOES-15 (West) channel 4 */
//                 *dx = *dy = 880;
//                 *xp = *yp = 450;
//             }
//             else if (satelliteIdentifier == 57 && *dx == 1732) { /* MSG (Meteosat second generation), non-HRV channels */
//                 *dx = *dy = 1811;
//                 *xp = *yp = 928;
//             }
//         }
//     }
// }

#define RAD2DEG 57.29577951308232087684 /* 180 over pi */
#define DEG2RAD 0.01745329251994329576  /* pi over 180 */

static int init(grib_iterator* iter, grib_handle* h, grib_arguments* args)
{
    /* REFERENCE:
    *  LRIT/HRIT Global Specification (CGMS 03, Issue 2.6, 12.08.1999)
    */
    int ret = GRIB_SUCCESS;
    double *lats, *lons; /* arrays of latitudes and longitudes */
    double latOfSubSatellitePointInDegrees, lonOfSubSatellitePointInDegrees;
    double orientationInDegrees, nrInRadiusOfEarth;
    double radius = 0, xpInGridLengths = 0, ypInGridLengths = 0;
    long nx, ny, earthIsOblate                              = 0;
    long alternativeRowScanning, iScansNegatively;
    long Xo, Yo, jScansPositively, jPointsAreConsecutive, i;

    double major = 0, minor = 0, r_eq, r_pol, height;
    double lap, lop, angular_size;
    double xp, yp, dx, dy, rx, ry, x, y;
    double cos_x, cos_y, sin_x, sin_y;
    double factor_1, factor_2, tmp1, Sd, Sn, Sxy, S1, S2, S3;
    int x0, y0, ix, iy;
    double *s_x, *c_x; /* arrays storing sin and cos values */
    size_t array_size = (iter->nv * sizeof(double));

    grib_iterator_space_view* self = (grib_iterator_space_view*)iter;

    const char* sradius                          = grib_arguments_get_name(h, args, self->carg++);
    const char* sEarthIsOblate                   = grib_arguments_get_name(h, args, self->carg++);
    const char* sMajorAxisInMetres               = grib_arguments_get_name(h, args, self->carg++);
    const char* sMinorAxisInMetres               = grib_arguments_get_name(h, args, self->carg++);
    const char* snx                              = grib_arguments_get_name(h, args, self->carg++);
    const char* sny                              = grib_arguments_get_name(h, args, self->carg++);
    const char* sLatOfSubSatellitePointInDegrees = grib_arguments_get_name(h, args, self->carg++);
    const char* sLonOfSubSatellitePointInDegrees = grib_arguments_get_name(h, args, self->carg++);
    const char* sDx                              = grib_arguments_get_name(h, args, self->carg++);
    const char* sDy                              = grib_arguments_get_name(h, args, self->carg++);
    const char* sXpInGridLengths                 = grib_arguments_get_name(h, args, self->carg++);
    const char* sYpInGridLengths                 = grib_arguments_get_name(h, args, self->carg++);
    const char* sOrientationInDegrees            = grib_arguments_get_name(h, args, self->carg++);
    const char* sNrInRadiusOfEarthScaled         = grib_arguments_get_name(h, args, self->carg++);
    const char* sXo                              = grib_arguments_get_name(h, args, self->carg++);
    const char* sYo                              = grib_arguments_get_name(h, args, self->carg++);

    const char* siScansNegatively       = grib_arguments_get_name(h, args, self->carg++);
    const char* sjScansPositively       = grib_arguments_get_name(h, args, self->carg++);
    const char* sjPointsAreConsecutive  = grib_arguments_get_name(h, args, self->carg++);
    const char* sAlternativeRowScanning = grib_arguments_get_name(h, args, self->carg++);

    if ((ret = grib_get_long_internal(h, snx, &nx)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, sny, &ny)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, sEarthIsOblate, &earthIsOblate)) != GRIB_SUCCESS)
        return ret;

    if (earthIsOblate) {
        if ((ret = grib_get_double_internal(h, sMajorAxisInMetres, &major)) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_get_double_internal(h, sMinorAxisInMetres, &minor)) != GRIB_SUCCESS)
            return ret;
    }
    else {
        if ((ret = grib_get_double_internal(h, sradius, &radius)) != GRIB_SUCCESS)
            return ret;
    }

    if (iter->nv != nx * ny) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Wrong number of points (%zu!=%ldx%ld)", ITER, iter->nv, nx, ny);
        return GRIB_WRONG_GRID;
    }
    if ((ret = grib_get_double_internal(h, sLatOfSubSatellitePointInDegrees, &latOfSubSatellitePointInDegrees)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, sLonOfSubSatellitePointInDegrees, &lonOfSubSatellitePointInDegrees)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, sDx, &dx)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, sDy, &dy)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, sXpInGridLengths, &xpInGridLengths)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, sYpInGridLengths, &ypInGridLengths)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, sOrientationInDegrees, &orientationInDegrees)) != GRIB_SUCCESS)
        return ret;

    /* Orthographic not supported. This happens when Nr (camera altitude) is missing */
    if (grib_is_missing(h, "Nr", &ret)) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Orthographic view (Nr missing) not supported", ITER);
        return GRIB_GEOCALCULUS_PROBLEM;
    }
    if ((ret = grib_get_double_internal(h, sNrInRadiusOfEarthScaled, &nrInRadiusOfEarth)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(h, sXo, &Xo)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, sYo, &Yo)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, sjPointsAreConsecutive, &jPointsAreConsecutive)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, sjScansPositively, &jScansPositively)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, siScansNegatively, &iScansNegatively)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, sAlternativeRowScanning, &alternativeRowScanning)) != GRIB_SUCCESS)
        return ret;

    if (earthIsOblate) {
        r_eq  = major; /* In km */
        r_pol = minor;
    }
    else {
        r_eq = r_pol = radius * 0.001; /*conv to km*/
    }

    if (nrInRadiusOfEarth == 0) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Key %s must be greater than zero", ITER, sNrInRadiusOfEarthScaled);
        return GRIB_GEOCALCULUS_PROBLEM;
    }

    angular_size = 2.0 * asin(1.0 / nrInRadiusOfEarth);
    height       = nrInRadiusOfEarth * r_eq;

    lap = latOfSubSatellitePointInDegrees;
    lop = lonOfSubSatellitePointInDegrees;
    if (lap != 0.0) {
        grib_context_log(h->context, GRIB_LOG_ERROR,
                         "%s: Key %s must be 0 (satellite must be located in the equator plane)",
                         ITER, sLatOfSubSatellitePointInDegrees);
        return GRIB_GEOCALCULUS_PROBLEM;
    }

    /*orient_angle = orientationInDegrees;*/
    /* if (orient_angle != 0.0) return GRIB_NOT_IMPLEMENTED; */

    xp = xpInGridLengths;
    yp = ypInGridLengths;
    x0 = Xo;
    y0 = Yo;

    /* adjustBadlyEncodedEcmwfGribs(h, &nx, &ny, &dx, &dy, &xp, &yp); */
    if (dx == 0 || dy == 0) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Keys %s and %s must be greater than zero", ITER, sDx, sDy);
        return GRIB_GEOCALCULUS_PROBLEM;
    }
    rx = angular_size / dx;
    ry = (r_pol / r_eq) * angular_size / dy;

    self->lats = (double*)grib_context_malloc(h->context, array_size);
    if (!self->lats) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, array_size);
        return GRIB_OUT_OF_MEMORY;
    }
    self->lons = (double*)grib_context_malloc(h->context, array_size);
    if (!self->lats) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, array_size);
        return GRIB_OUT_OF_MEMORY;
    }
    lats = self->lats;
    lons = self->lons;

    if (!iScansNegatively) {
        xp = xp - x0;
    }
    else {
        xp = (nx - 1) - (xp - x0);
    }
    if (jScansPositively) {
        yp = yp - y0;
    }
    else {
        yp = (ny - 1) - (yp - y0);
    }
    i        = 0;
    factor_2 = (r_eq / r_pol) * (r_eq / r_pol);
    factor_1 = height * height - r_eq * r_eq;

    /* Store array of sin and cosine values to avoid recalculation */
    s_x = (double*)grib_context_malloc(h->context, nx * sizeof(double));
    if (!s_x) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, nx * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    c_x = (double*)grib_context_malloc(h->context, nx * sizeof(double));
    if (!c_x) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, nx * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }

    for (ix = 0; ix < nx; ix++) {
        x       = (ix - xp) * rx;
        s_x[ix] = sin(x);
        c_x[ix] = sqrt(1.0 - s_x[ix] * s_x[ix]);
    }

    /*for (iy = 0; iy < ny; iy++) {*/
    for (iy = ny - 1; iy >= 0; --iy) {
        y     = (iy - yp) * ry;
        sin_y = sin(y);
        cos_y = sqrt(1.0 - sin_y * sin_y);

        tmp1 = (1 + (factor_2 - 1.0) * sin_y * sin_y);

        for (ix = 0; ix < nx; ix++, i++) {
            /*x = (ix - xp) * rx;*/
            /* Use sin/cos previously computed */
            sin_x = s_x[ix];
            cos_x = c_x[ix];

            Sd = height * cos_x * cos_y;
            Sd = Sd * Sd - tmp1 * factor_1;
            if (Sd <= 0.0) {           /* outside of view */
                lats[i] = lons[i] = 0; /* TODO: error? */
            }
            else {
                Sd      = sqrt(Sd);
                Sn      = (height * cos_x * cos_y - Sd) / tmp1;
                S1      = height - Sn * cos_x * cos_y;
                S2      = Sn * sin_x * cos_y;
                S3      = Sn * sin_y;
                Sxy     = sqrt(S1 * S1 + S2 * S2);
                lons[i] = atan(S2 / S1) * (RAD2DEG) + lop;
                lats[i] = atan(factor_2 * S3 / Sxy) * (RAD2DEG);
                /*fprintf(stderr, "lat=%g   lon=%g\n", lats[i], lons[i]);*/
            }
            while (lons[i] < 0)
                lons[i] += 360;
            while (lons[i] > 360)
                lons[i] -= 360;
        }
    }
    grib_context_free(h->context, s_x);
    grib_context_free(h->context, c_x);
    iter->e = -1;

    return ret;
}

static int destroy(grib_iterator* iter)
{
    grib_iterator_space_view* self = (grib_iterator_space_view*)iter;
    const grib_context* c          = iter->h->context;

    grib_context_free(c, self->lats);
    grib_context_free(c, self->lons);
    return GRIB_SUCCESS;
}
