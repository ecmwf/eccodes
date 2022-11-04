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
#include <math.h>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = iterator
   SUPER      = grib_iterator_class_gen
   IMPLEMENTS = previous;next
   IMPLEMENTS = init;destroy
   MEMBERS    = double   *las
   MEMBERS    = double   *los
   MEMBERS    = long      Ni
   MEMBERS    = long      Nj
   MEMBERS    = long iScansNegatively
   MEMBERS    = long isRotated
   MEMBERS    = double angleOfRotation
   MEMBERS    = double southPoleLat
   MEMBERS    = double southPoleLon
   MEMBERS    = long jPointsAreConsecutive
   MEMBERS    = long disableUnrotate
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
static int previous           (grib_iterator* ei, double *lat, double *lon, double *val);
static int destroy            (grib_iterator* i);


typedef struct grib_iterator_regular{
  grib_iterator it;
    /* Members defined in gen */
    long carg;
    const char* missingValue;
    /* Members defined in regular */
    double   *las;
    double   *los;
    long      Ni;
    long      Nj;
    long iScansNegatively;
    long isRotated;
    double angleOfRotation;
    double southPoleLat;
    double southPoleLon;
    long jPointsAreConsecutive;
    long disableUnrotate;
} grib_iterator_regular;

extern grib_iterator_class* grib_iterator_class_gen;

static grib_iterator_class _grib_iterator_class_regular = {
    &grib_iterator_class_gen,                    /* super                     */
    "regular",                    /* name                      */
    sizeof(grib_iterator_regular),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    &destroy,                  /* destructor                */
    &next,                     /* Next Value                */
    &previous,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_regular = &_grib_iterator_class_regular;


static void init_class(grib_iterator_class* c)
{
    c->reset    =    (*(c->super))->reset;
    c->has_next    =    (*(c->super))->has_next;
}
/* END_CLASS_IMP */


static int next(grib_iterator* i, double* lat, double* lon, double* val)
{
    grib_iterator_regular* self = (grib_iterator_regular*)i;

    if ((long)i->e >= (long)(i->nv - 1))
        return 0;

    i->e++;

    *lat = self->las[(long)floor(i->e / self->Ni)];
    *lon = self->los[(long)i->e % self->Ni];
    *val = i->data[i->e];

    return 1;
}

static int previous(grib_iterator* i, double* lat, double* lon, double* val)
{
    grib_iterator_regular* self = (grib_iterator_regular*)i;

    if (i->e < 0)
        return 0;
    *lat = self->las[(long)floor(i->e / self->Ni)];
    *lon = self->los[i->e % self->Ni];
    *val = i->data[i->e];
    i->e--;

    return 1;
}

static int destroy(grib_iterator* i)
{
    grib_iterator_regular* self = (grib_iterator_regular*)i;
    const grib_context* c       = i->h->context;
    grib_context_free(c, self->las);
    grib_context_free(c, self->los);
    return GRIB_SUCCESS;
}

static int init(grib_iterator* i, grib_handle* h, grib_arguments* args)
{
    grib_iterator_regular* self = (grib_iterator_regular*)i;
    int ret                     = GRIB_SUCCESS;

    long Ni; /* Number of points along a parallel = Nx */
    long Nj; /* Number of points along a meridian = Ny */
    double idir, lon1, lon2;
    long loi;

    const char* s_lon1      = grib_arguments_get_name(h, args, self->carg++);
    const char* s_idir      = grib_arguments_get_name(h, args, self->carg++);
    const char* s_Ni        = grib_arguments_get_name(h, args, self->carg++);
    const char* s_Nj        = grib_arguments_get_name(h, args, self->carg++);
    const char* s_iScansNeg = grib_arguments_get_name(h, args, self->carg++);

    if ((ret = grib_get_double_internal(h, s_lon1, &lon1)))
        return ret;
    if ((ret = grib_get_double_internal(h, "longitudeOfLastGridPointInDegrees", &lon2)))
        return ret;
    if ((ret = grib_get_double_internal(h, s_idir, &idir)))
        return ret;
    if ((ret = grib_get_long_internal(h, s_Ni, &Ni)))
        return ret;
    if (grib_is_missing(h, s_Ni, &ret) && ret == GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Key %s cannot be 'missing' for a regular grid!", s_Ni);
        return GRIB_WRONG_GRID;
    }

    if ((ret = grib_get_long_internal(h, s_Nj, &Nj)))
        return ret;
    if (grib_is_missing(h, s_Nj, &ret) && ret == GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Key %s cannot be 'missing' for a regular grid!", s_Nj);
        return GRIB_WRONG_GRID;
    }

    if ((ret = grib_get_long_internal(h, s_iScansNeg, &self->iScansNegatively)))
        return ret;

    /* GRIB-801: Careful of case with a single point! Ni==1 */
    if (Ni > 1) {
        /* Note: If first and last longitudes are equal I assume you wanna go round the globe */
        if (self->iScansNegatively) {
            if (lon1 > lon2) {
                idir = (lon1 - lon2) / (Ni - 1);
            }
            else {
                idir = (lon1 + 360.0 - lon2) / (Ni - 1);
            }
        }
        else {
            if (lon2 > lon1) {
                idir = (lon2 - lon1) / (Ni - 1);
            }
            else {
                idir = (lon2 + 360.0 - lon1) / (Ni - 1);
            }
        }
    }
    if (self->iScansNegatively) {
        idir = -idir;
    }
    else {
        if (lon1 + (Ni - 2) * idir > 360)
            lon1 -= 360;
        /*See ECC-704, GRIB-396*/
        /*else if ( (lon1+(Ni-1)*idir)-360 > epsilon ){
            idir=360.0/(float)Ni;
        }*/
    }

    self->Ni = Ni;
    self->Nj = Nj;

    self->las = (double*)grib_context_malloc(h->context, Nj * sizeof(double));
    self->los = (double*)grib_context_malloc(h->context, Ni * sizeof(double));

    for (loi = 0; loi < Ni; loi++) {
        self->los[loi] = lon1;
        lon1 += idir;
    }
    /* ECC-1406: Due to rounding, errors can accumulate.
     * So we ensure the last longitude is longitudeOfLastGridPointInDegrees
    */
    self->los[Ni-1] = lon2;

    return ret;
}
