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
#include <math.h>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = iterator
   SUPER      = grib_iterator_class_gen
   IMPLEMENTS = previous;next
   IMPLEMENTS = init;destroy
   MEMBERS    =  double   *las
   MEMBERS    =  double   *los
   MEMBERS    =  long      nap
   MEMBERS    =  long      nam
   MEMBERS    =  long iScansNegatively
   MEMBERS    = long isRotated
   MEMBERS    = double angleOfRotation
   MEMBERS    = double southPoleLat
   MEMBERS    = double southPoleLon
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
	long      nap;
	long      nam;
	long iScansNegatively;
	long isRotated;
	double angleOfRotation;
	double southPoleLat;
	double southPoleLon;
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
	c->reset	=	(*(c->super))->reset;
	c->has_next	=	(*(c->super))->has_next;
}
/* END_CLASS_IMP */


static int next(grib_iterator* i, double *lat, double *lon, double *val){
    grib_iterator_regular* self = (grib_iterator_regular*)i;

    if((long)i->e >= (long)(i->nv-1))  return 0;

    i->e++;

    *lat = self->las[(long)floor(i->e/self->nap)];
    *lon = self->los[(long)i->e%self->nap];
    *val = i->data[i->e];

    return 1;
}

static int previous(grib_iterator* i, double *lat, double *lon, double *val){
    grib_iterator_regular* self = (grib_iterator_regular*)i;

    if(i->e < 0)      return 0;
    *lat = self->las[(long)floor(i->e/self->nap)];
    *lon = self->los[i->e%self->nap];
    *val = i->data[i->e];
    i->e--;

    return 1;
}

static int destroy(grib_iterator* i){
    grib_iterator_regular* self = (grib_iterator_regular*)i;
    const grib_context *c = i->h->context;
    grib_context_free(c,self->las);
    grib_context_free(c,self->los);
    return GRIB_SUCCESS;
}

static int init(grib_iterator* i,grib_handle* h,grib_arguments* args)
{
    grib_iterator_regular* self = (grib_iterator_regular*)i;
    int ret = GRIB_SUCCESS;

    long nap; /* Number of points along a parallel = Ni */
    long nam; /* Number of points along a meridian = Nj */
    double idir, lof,lol;
    long loi;

    const char* longoffirst = grib_arguments_get_name(h,args,self->carg++);
    const char* idirec      = grib_arguments_get_name(h,args,self->carg++);
    const char* nalpar      = grib_arguments_get_name(h,args,self->carg++);
    const char* nalmer      = grib_arguments_get_name(h,args,self->carg++);
    const char* iScansNegatively  = grib_arguments_get_name(h,args,self->carg++);

    if((ret = grib_get_double_internal(h,longoffirst,   &lof))) return ret;
    if((ret = grib_get_double_internal(h,"longitudeOfLastGridPointInDegrees", &lol))) return ret;
    if((ret = grib_get_double_internal(h,idirec,        &idir))) return ret;
    if((ret = grib_get_long_internal(h,nalpar,          &nap))) return ret;
    if((ret = grib_get_long_internal(h,nalmer,          &nam))) return ret;
    if((ret = grib_get_long_internal(h,iScansNegatively,&self->iScansNegatively)))
        return ret;

    /* GRIB-801: Careful of case with a single point! nap==1 */
    if (nap > 1) {
        /* Note: If first and last longitudes are equal I assume you wanna go round the globe */
        if (self->iScansNegatively) {
            if (lof > lol){
                idir=(lof-lol)/(nap-1);
            }
            else {
                idir=(lof+360.0-lol)/(nap-1);
            }
        }
        else {
            if (lol > lof){
                idir=(lol-lof)/(nap-1);
            }
            else {
                idir=(lol+360.0-lof)/(nap-1);
            }
        }
    }
    if (self->iScansNegatively) {
        idir=-idir;
    } else {
        const double epsilon = 1e-6;
        if (lof+(nap-2)*idir>360) lof-=360;
        else if ( (lof+(nap-1)*idir)-360 > epsilon ){
            /*See GRIB-396*/
            idir=360.0/(float)nap;
        }
    }

    self->nap = nap;
    self->nam = nam;

    self->las = (double*)grib_context_malloc(h->context,nam*sizeof(double));
    self->los = (double*)grib_context_malloc(h->context,nap*sizeof(double));

    for( loi = 0; loi < nap; loi++ )  {
        self->los[loi] = lof;
        lof += idir ;
    }

    return ret;
}
