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
   MEMBERS     =   long Nsides
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


typedef struct grib_iterator_healpix{
  grib_iterator it;
    /* Members defined in gen */
    int carg;
    const char* missingValue;
    /* Members defined in healpix */
    double *lats;
    double *lons;
    long Nsides;
} grib_iterator_healpix;

extern grib_iterator_class* grib_iterator_class_gen;

static grib_iterator_class _grib_iterator_class_healpix = {
    &grib_iterator_class_gen,                    /* super                     */
    "healpix",                    /* name                      */
    sizeof(grib_iterator_healpix),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    &destroy,                  /* destructor                */
    &next,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_healpix = &_grib_iterator_class_healpix;


static void init_class(grib_iterator_class* c)
{
    c->previous    =    (*(c->super))->previous;
    c->reset    =    (*(c->super))->reset;
    c->has_next    =    (*(c->super))->has_next;
}
/* END_CLASS_IMP */

#define ITER "HEALPix Geoiterator"
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

// Adjust longitude (in radians) to range -180 to 180
// static double adjust_lon_radians(double lon)
// {
//     if (lon > M_PI)  lon -= 2 * M_PI;
//     if (lon < -M_PI) lon += 2 * M_PI;
//     return lon;
// }

static int init(grib_iterator* iter, grib_handle* h, grib_arguments* args)
{
    int err = 0, is_oblate = 0;
    long nside = 0;
    char ordering[32] = {0,};
    size_t slen = sizeof(ordering);

    grib_iterator_healpix* self = (grib_iterator_healpix*)iter;

    const char* snside = grib_arguments_get_name(h, args, self->carg++);
    const char* sorder = grib_arguments_get_name(h, args, self->carg++);

    if ((err = grib_get_long_internal(h, snside, &nside)) != GRIB_SUCCESS) return err;
    if ((err = grib_get_string_internal(h, sorder, ordering, &slen)) != GRIB_SUCCESS)
        return err;

    if (!STR_EQUAL(ordering, "ring")) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Only ring ordering is supported", ITER);
        return GRIB_WRONG_GRID;
    }

    is_oblate = grib_is_earth_oblate(h);
    Assert(!is_oblate);

    if (iter->nv != 12 * nside * nside) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Wrong number of points (%ld!=12x%ldx%ld)",
                ITER, iter->nv, nside, nside);
        return GRIB_WRONG_GRID;
    }

    {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Not implemented! Be patient :)", ITER);
        return GRIB_INTERNAL_ERROR;
    }

//     latFirstInRadians = latFirstInDegrees * DEG2RAD;
//     lonFirstInRadians = lonFirstInDegrees * DEG2RAD;
//     Latin1InRadians   = Latin1InDegrees * DEG2RAD;
//     Latin2InRadians   = Latin2InDegrees * DEG2RAD;
//     LaDInRadians      = LaDInDegrees * DEG2RAD;
//     LoVInRadians      = LoVInDegrees * DEG2RAD;
// 
//     err = init_sphere(h, self, iter->nv, nx, ny,
//                           LoVInDegrees,
//                           Dx, Dy, radius,
//                           latFirstInRadians, lonFirstInRadians,
//                           LoVInRadians, Latin1InRadians, Latin2InRadians, LaDInRadians);
//     if (err) return err;

    iter->e = -1;

    return err;
}

static int next(grib_iterator* iter, double* lat, double* lon, double* val)
{
    grib_iterator_healpix* self = (grib_iterator_healpix*)iter;

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

static int destroy(grib_iterator* i)
{
    grib_iterator_healpix* self = (grib_iterator_healpix*)i;
    const grib_context* c       = i->h->context;

    grib_context_free(c, self->lats);
    grib_context_free(c, self->lons);
    return GRIB_SUCCESS;
}
