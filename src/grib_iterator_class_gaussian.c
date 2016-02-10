/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
/**************************************
 *  Enrico Fucile
 **************************************/


#include "grib_api_internal.h"
#include <math.h>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = iterator
   SUPER      = grib_iterator_class_regular
   IMPLEMENTS = init
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


typedef struct grib_iterator_gaussian{
  grib_iterator it;
/* Members defined in gen */
	long carg;
	const char* missingValue;
/* Members defined in regular */
	double   *las;
	double   *los;
	long      nap;
	long      nam;
	long iScansNegatively;
	long isRotated;
	double angleOfRotation;
	double southPoleLat;
	double southPoleLon;
/* Members defined in gaussian */
} grib_iterator_gaussian;

extern grib_iterator_class* grib_iterator_class_regular;

static grib_iterator_class _grib_iterator_class_gaussian = {
    &grib_iterator_class_regular,                    /* super                     */
    "gaussian",                    /* name                      */
    sizeof(grib_iterator_gaussian),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    0,                  /* destructor                */
    0,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_gaussian = &_grib_iterator_class_gaussian;


static void init_class(grib_iterator_class* c)
{
	c->next	=	(*(c->super))->next;
	c->previous	=	(*(c->super))->previous;
	c->reset	=	(*(c->super))->reset;
	c->has_next	=	(*(c->super))->has_next;
}
/* END_CLASS_IMP */

static void binary_search(double xx[], const unsigned long n, double x, unsigned long *j);

static int init(grib_iterator* i,grib_handle* h,grib_arguments *args){
    grib_iterator_gaussian* self = (grib_iterator_gaussian*)i;

    double *lats;
    double laf; /* latitude of first point in degrees */
    double lal; /* latitude of last point in degrees */
    long trunc; /* number of parallels between a pole and the equator */
    long lai;
    long jScansPositively=0;
    int size;
    double start;
    unsigned long istart=0;
    int ret = GRIB_SUCCESS;

    const char* latofirst   = grib_arguments_get_name(h,args,self->carg++);
    const char* latoflast   = grib_arguments_get_name(h,args,self->carg++);
    const char* numtrunc    = grib_arguments_get_name(h,args,self->carg++);
    const char* s_jScansPositively    = grib_arguments_get_name(h,args,self->carg++);

    if((ret = grib_get_double_internal(h,latofirst,   &laf))) return ret;
    if((ret = grib_get_double_internal(h,latoflast,   &lal))) return ret;
    if((ret = grib_get_long_internal(h,numtrunc,&trunc))) return ret;
    if((ret = grib_get_long_internal(h,s_jScansPositively,&jScansPositively)))
        return ret;

    start=laf;

    size=trunc*2;

    lats = (double*)grib_context_malloc(h->context,size*sizeof(double));

    ret = grib_get_gaussian_latitudes(trunc, lats);

    if(ret != GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR,"error %d calculating gaussian points",ret);
        return ret;
    }
    /*
  for(loi=(trunc*2)-1;loi>=0;loi--)
    if(fabs(lats[loi] - lal) < glatPrecision) break;
  for(j=(trunc*2)-1;j>0;j--) {
    if(fabs(lats[j] - laf) < glatPrecision) break;
  }
     */

    binary_search(lats,size-1,start,&istart);
    Assert(istart < size);

    if (jScansPositively) {
        for(lai=0;lai<self->nam;lai++) {
            self->las[lai] = lats[istart--];
            /*if (istart<0) istart=size-1;  this condition is always FALSE -- 'istart' is unsigned long */
        }
    } else {
        for(lai=0;lai<self->nam;lai++) {
            self->las[lai] = lats[istart++];
            if (istart>size-1) istart=0;
        }
    }

    grib_context_free(h->context,lats);

    return ret;
}

static void binary_search(double xx[], const unsigned long n, double x, unsigned long *j)
{
    /*This routine works only on descending ordered arrays*/
#define EPSILON 1e-3

    unsigned long ju,jm,jl;
    jl=0;
    ju=n;
    while (ju-jl > 1) {
        jm=(ju+jl) >> 1;
        if (fabs(x-xx[jm]) < EPSILON) {
            /* found something close enough. We're done */
            *j=jm;
            return;
        }
        if (x < xx[jm]) jl=jm;
        else ju=jm;
    }
    *j=jl;
}
