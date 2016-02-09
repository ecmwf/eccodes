/*
 * Copyright 2005-2016 ECMWF.
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
   CLASS      = nearest
   IMPLEMENTS = destroy
   IMPLEMENTS = find
   IMPLEMENTS = init
   MEMBERS  = const char* values_key
   MEMBERS = const char* radius
   MEMBERS  = int cargs
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "nearest.class" and rerun ./make_class.pl

*/


static void init_class              (grib_nearest_class*);

static int init               (grib_nearest* nearest,grib_handle* h,grib_arguments* args);
static int find(grib_nearest* nearest, grib_handle* h,double inlat, double inlon, unsigned long flags, double* outlats,double* outlons, double *values,double *distances, int *indexes,size_t *len);
static int destroy            (grib_nearest* nearest);

typedef struct grib_nearest_gen{
  grib_nearest nearest;
/* Members defined in gen */
	const char* values_key;
	const char* radius;
	int cargs;
} grib_nearest_gen;


static grib_nearest_class _grib_nearest_class_gen = {
    0,                         /* super                     */
    "gen",                         /* name                      */
    sizeof(grib_nearest_gen),      /* size of instance          */
    0,                              /* inited */
    &init_class,                    /* init_class */
    &init,                          /* constructor               */
    &destroy,                       /* destructor                */
    &find,                          /* find nearest              */
};

grib_nearest_class* grib_nearest_class_gen = &_grib_nearest_class_gen;


static void init_class(grib_nearest_class* c)
{
}
/* END_CLASS_IMP */

static int init(grib_nearest* nearest,grib_handle* h,grib_arguments* args)
{
    grib_nearest_gen* self = (grib_nearest_gen*) nearest;
    int ret = GRIB_SUCCESS;

    self->cargs = 1;

    self->values_key  = grib_arguments_get_name(h,args,self->cargs++);
    self->radius  = grib_arguments_get_name(h,args,self->cargs++);
    nearest->values=NULL;

    nearest->context=h->context;

    return ret;
}

static int destroy(grib_nearest* nearest)
{
    if (nearest->values) grib_context_free(nearest->context,nearest->values);
    grib_context_free(nearest->context,nearest);
    return GRIB_SUCCESS;
}

static int find(grib_nearest* nearest, grib_handle* h,
        double inlat, double inlon,unsigned long flags,
        double* outlats,double* outlons, double *values,
        double *distances,int *indexes, size_t *len)
{
    return GRIB_NOT_IMPLEMENTED;
}
