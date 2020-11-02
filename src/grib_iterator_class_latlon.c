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

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = iterator
   SUPER      = grib_iterator_class_regular
   IMPLEMENTS = init;next
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "iterator.class" and rerun ./make_class.pl

*/


static void init_class(grib_iterator_class*);

static int init(grib_iterator* i, grib_handle*, grib_arguments*);
static int next(grib_iterator* i, double* lat, double* lon, double* val);


typedef struct grib_iterator_latlon
{
    grib_iterator it;
    /* Members defined in gen */
    long carg;
    const char* missingValue;
    /* Members defined in regular */
    double* las;
    double* los;
    long Ni;
    long Nj;
    long iScansNegatively;
    long isRotated;
    double angleOfRotation;
    double southPoleLat;
    double southPoleLon;
    long jPointsAreConsecutive;
    long disableUnrotate;
    /* Members defined in latlon */
} grib_iterator_latlon;

extern grib_iterator_class* grib_iterator_class_regular;

static grib_iterator_class _grib_iterator_class_latlon = {
    &grib_iterator_class_regular, /* super                     */
    "latlon",                     /* name                      */
    sizeof(grib_iterator_latlon), /* size of instance          */
    0,                            /* inited */
    &init_class,                  /* init_class */
    &init,                        /* constructor               */
    0,                            /* destructor                */
    &next,                        /* Next Value                */
    0,                            /*  Previous Value           */
    0,                            /* Reset the counter         */
    0,                            /* has next values           */
};

grib_iterator_class* grib_iterator_class_latlon = &_grib_iterator_class_latlon;


static void init_class(grib_iterator_class* c)
{
    c->previous = (*(c->super))->previous;
    c->reset    = (*(c->super))->reset;
    c->has_next = (*(c->super))->has_next;
}
/* END_CLASS_IMP */

static int next(grib_iterator* iter, double* lat, double* lon, double* val)
{
    /* GRIB-238: Support rotated lat/lon grids */
    double ret_lat, ret_lon, ret_val;
    grib_iterator_latlon* self = (grib_iterator_latlon*)iter;

    if ((long)iter->e >= (long)(iter->nv - 1))
        return 0;

    iter->e++;

    /* Assumptions:
     *   All rows scan in the same direction (alternativeRowScanning==0)
     */
    if (!self->jPointsAreConsecutive) {
        /* Adjacent points in i (x) direction are consecutive */
        ret_lat = self->las[(long)floor(iter->e / self->Ni)];
        ret_lon = self->los[(long)iter->e % self->Ni];
        ret_val = iter->data[iter->e];
    }
    else {
        /* Adjacent points in j (y) direction is consecutive */
        ret_lon = self->los[(long)iter->e / self->Nj];
        ret_lat = self->las[(long)floor(iter->e % self->Nj)];
        ret_val = iter->data[iter->e];
    }

    /* See ECC-808: Some users want to disable the unrotate */
    if (self->isRotated && !self->disableUnrotate) {
        double new_lat = 0, new_lon = 0;
        unrotate(ret_lat, ret_lon,
                 self->angleOfRotation, self->southPoleLat, self->southPoleLon,
                 &new_lat, &new_lon);
        ret_lat = new_lat;
        ret_lon = new_lon;
    }

    *lat = ret_lat;
    *lon = ret_lon;
    *val = ret_val;
    return 1;
}

static int init(grib_iterator* iter, grib_handle* h, grib_arguments* args)
{
    grib_iterator_latlon* self = (grib_iterator_latlon*)iter;
    int err                    = 0;
    double jdir;
    double lat1;
    long jScansPositively;
    long lai;

    const char* s_lat1       = grib_arguments_get_name(h, args, self->carg++);
    const char* s_jdir       = grib_arguments_get_name(h, args, self->carg++);
    const char* s_jScansPos  = grib_arguments_get_name(h, args, self->carg++);
    const char* s_jPtsConsec = grib_arguments_get_name(h, args, self->carg++);
    const char* s_isRotatedGrid  = grib_arguments_get_name(h, args, self->carg++);
    const char* s_angleOfRotation= grib_arguments_get_name(h, args, self->carg++);
    const char* s_latSouthernPole= grib_arguments_get_name(h, args, self->carg++);
    const char* s_lonSouthernPole= grib_arguments_get_name(h, args, self->carg++);

    self->angleOfRotation  = 0;
    self->isRotated        = 0;
    self->southPoleLat     = 0;
    self->southPoleLon     = 0;
    self->disableUnrotate  = 0; /* unrotate enabled by default */

    if ((err = grib_get_long(h, s_isRotatedGrid, &self->isRotated)))
        return err;
    if (self->isRotated) {
        if ((err = grib_get_double_internal(h, s_angleOfRotation, &self->angleOfRotation)))
            return err;
        if ((err = grib_get_double_internal(h, s_latSouthernPole, &self->southPoleLat)))
            return err;
        if ((err = grib_get_double_internal(h, s_lonSouthernPole, &self->southPoleLon)))
            return err;
    }

    if ((err = grib_get_double_internal(h, s_lat1, &lat1)))
        return err;
    if ((err = grib_get_double_internal(h, s_jdir, &jdir)))
        return err;
    if ((err = grib_get_long_internal(h, s_jScansPos, &jScansPositively)))
        return err;
    if ((err = grib_get_long_internal(h, s_jPtsConsec, &self->jPointsAreConsecutive)))
        return err;
    if ((err = grib_get_long(h, "iteratorDisableUnrotate", &self->disableUnrotate)))
        return err;

    /* ECC-984: If jDirectionIncrement is missing, then we cannot use it (See jDirectionIncrementGiven) */
    /* So try to compute the increment */
    if (grib_is_missing(h, s_jdir, &err) && err == GRIB_SUCCESS) {
        double lat2;
        if ((err = grib_get_double_internal(h, "latitudeLastInDegrees", &lat2)) == GRIB_SUCCESS) {
            const long Nj = self->Nj;
            Assert(Nj > 1);
            if (lat1 > lat2) {
                jdir = (lat1 - lat2) / (Nj - 1);
            }
            else {
                jdir = (lat2 - lat1) / (Nj - 1);
            }
            grib_context_log(h->context, GRIB_LOG_INFO,
                             "%s is missing (See jDirectionIncrementGiven). Using value of %.6f obtained from La1, La2 and Nj", s_jdir, jdir);
        }
    }
    if (jScansPositively)
        jdir = -jdir;

    for (lai = 0; lai < self->Nj; lai++) {
        self->las[lai] = lat1;
        lat1 -= jdir;
    }

    iter->e = -1;
    return err;
}
