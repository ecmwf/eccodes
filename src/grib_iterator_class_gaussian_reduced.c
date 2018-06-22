/*
 * Copyright 2005-2018 ECMWF.
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
   SUPER      = grib_iterator_class_gen
   IMPLEMENTS = destroy
   IMPLEMENTS = init;next
   MEMBERS     =   double *las
   MEMBERS     =   double *los
   MEMBERS     =   long nam
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


typedef struct grib_iterator_gaussian_reduced{
  grib_iterator it;
/* Members defined in gen */
	long carg;
	const char* missingValue;
/* Members defined in gaussian_reduced */
	double *las;
	double *los;
	long nam;
} grib_iterator_gaussian_reduced;

extern grib_iterator_class* grib_iterator_class_gen;

static grib_iterator_class _grib_iterator_class_gaussian_reduced = {
    &grib_iterator_class_gen,                    /* super                     */
    "gaussian_reduced",                    /* name                      */
    sizeof(grib_iterator_gaussian_reduced),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    &destroy,                  /* destructor                */
    &next,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_gaussian_reduced = &_grib_iterator_class_gaussian_reduced;


static void init_class(grib_iterator_class* c)
{
	c->previous	=	(*(c->super))->previous;
	c->reset	=	(*(c->super))->reset;
	c->has_next	=	(*(c->super))->has_next;
}
/* END_CLASS_IMP */

static int next(grib_iterator* i, double *lat, double *lon, double *val)
{
    grib_iterator_gaussian_reduced* self = (grib_iterator_gaussian_reduced*)i;

    if((long)i->e >= (long)(i->nv-1))
        return 0;
    i->e++;

    *lat = self->las[i->e];
    *lon = self->los[i->e];
    *val = i->data[i->e];

    return 1;
}

typedef void (*get_reduced_row_proc)(long pl, double lon_first, double lon_last, long* npoints, long* ilon_first, long* ilon_last);

/* For a reduced Gaussian grid which is GLOBAL, the number of points is the sum of the 'pl' array */
/* i.e. the total number of points on all latitudes */
static size_t count_global_points(long* pl, size_t plsize)
{
    return sum_of_pl_array(pl, plsize);
}
static size_t count_subarea_points(grib_handle* h, get_reduced_row_proc get_reduced_row,
                                   long* pl, size_t plsize, double lon_first, double lon_last)
{
    size_t j = 0, result = 0;
    long row_count = 0;
    long ilon_first=0,ilon_last=0;/*unused*/
    long Nj = 0;
    grib_get_long_internal(h, "Nj", &Nj);
    for (j=0;j<Nj;j++) {
        row_count = 0;
        get_reduced_row(pl[j],lon_first,lon_last,&row_count,&ilon_first,&ilon_last);
        result += row_count;
    }
    return result;
}

/* ECC-445: Try to compute the iterator latitude/longitude values. If algorithm2 is set, try a different point count */
static int iterate_reduced_gaussian_subarea(grib_iterator* iter, grib_handle* h,
        double lat_first, double lon_first,
        double lat_last, double lon_last,
        double* lats, long* pl, size_t plsize, int algorithm2)
{
    int err = 0;
    int l = 0;
    size_t j = 0;
    long row_count=0;
    double d=0;
    long ilon_first, ilon_last, i;
    grib_iterator_gaussian_reduced* self = (grib_iterator_gaussian_reduced*)iter;
    get_reduced_row_proc get_reduced_row = &grib_get_reduced_row;
    if (algorithm2) {
        get_reduced_row = &grib_get_reduced_row2; /* switch to 2nd algorithm */
    }

    if (h->context->debug) {
        const size_t np = count_subarea_points(h, get_reduced_row, pl, plsize, lon_first, lon_last);
        printf("ECCODES DEBUG grib_iterator_class_gaussian_reduced: sub-area num points=%ld\n", np);
    }

    /*find starting latitude */
    d = fabs(lats[0] - lats[1]);
    while (fabs(lat_first-lats[l]) > d ) {l++;}

    iter->e=0;
    for (j=0;j<plsize;j++) {
        long k = 0;
        row_count=0;
        get_reduced_row(pl[j],lon_first,lon_last, &row_count,&ilon_first,&ilon_last);
        /*printf("iterate_reduced_gaussian_subarea %ld %g %g count=%ld, (i1=%ld, i2=%ld)\n",pl[j],lon_first,lon_last, row_count, ilon_first,ilon_last);*/
        if (ilon_first>ilon_last) ilon_first-=pl[j];
        for (i=ilon_first;i<=ilon_last;i++) {

            if(iter->e >= iter->nv){
                if (algorithm2) {
                    /* Only print error message on the second pass */
                    size_t np = count_subarea_points(h, get_reduced_row, pl, plsize, lon_first, lon_last);
                    grib_context_log(h->context,GRIB_LOG_ERROR,
                                     "Reduced Gaussian iterator (sub-area). Num points=%ld, size(values)=%ld", np, iter->nv);
                }
                return GRIB_WRONG_GRID;
            }

            self->los[iter->e]=((i)*360.0)/pl[j];
            self->las[iter->e]=lats[j+l];
            iter->e++;
            k++;
            if (k >= row_count) {
                /* Ensure we exit the loop and only process 'row_count' points */
                break;
            }
        }
    }
    return err;
}

static int iterate_reduced_gaussian_subarea_wrapper(grib_iterator* iter, grib_handle* h,
        double lat_first, double lon_first,
        double lat_last, double lon_last,
        double* lats, long* pl, size_t plsize)
{
    int err = 0;
    if (expandedBoundingBox(h)) {
        return iterate_reduced_gaussian_subarea(iter, h, lat_first, lon_first, lat_last, lon_last, lats, pl, plsize, 1);
    }

    /* Try legacy approach, if that fails try the next algorithm */
    err = iterate_reduced_gaussian_subarea(iter, h, lat_first, lon_first, lat_last, lon_last, lats, pl, plsize, 0);
    if (err == GRIB_WRONG_GRID) {
        /* ECC-445: First attempt failed. Try again with a different algorithm */
        err = iterate_reduced_gaussian_subarea(iter, h, lat_first, lon_first, lat_last, lon_last, lats, pl, plsize, 1);
    }
    return err;
}

static int init(grib_iterator* iter, grib_handle* h, grib_arguments* args)
{
    int ret=GRIB_SUCCESS, j, is_global=0;
    double lat_first=0,lon_first=0,lat_last=0,lon_last=0;
    double angular_precision = 1.0/1000000.0;
    double* lats;
    size_t plsize=0;
    long* pl;
    long max_pl=0;
    long nj=0,order=0,i;
    long row_count=0;
    long editionNumber = 0;
    grib_context* c=h->context;
    grib_iterator_gaussian_reduced* self = (grib_iterator_gaussian_reduced*)iter;
    const char* slat_first   = grib_arguments_get_name(h,args,self->carg++);
    const char* slon_first   = grib_arguments_get_name(h,args,self->carg++);
    const char* slat_last    = grib_arguments_get_name(h,args,self->carg++);
    const char* slon_last    = grib_arguments_get_name(h,args,self->carg++);
    const char* sorder       = grib_arguments_get_name(h,args,self->carg++);
    const char* spl          = grib_arguments_get_name(h,args,self->carg++);
    const char* snj          = grib_arguments_get_name(h,args,self->carg++);

    if((ret = grib_get_double_internal(h, slat_first,&lat_first)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, slon_first,&lon_first)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, slat_last,&lat_last)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, slon_last,&lon_last)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(h, sorder,&order)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, snj,&nj)) != GRIB_SUCCESS)
        return ret;

    if (grib_get_long(h, "editionNumber", &editionNumber)==GRIB_SUCCESS) {
        if (editionNumber == 1) angular_precision = 1.0/1000;
    }

    lats=(double*)grib_context_malloc(h->context,sizeof(double)*order*2);
    if (!lats) return GRIB_OUT_OF_MEMORY;
    if((ret = grib_get_gaussian_latitudes(order, lats)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_size(h,spl,&plsize)) != GRIB_SUCCESS)
        return ret;

    Assert(plsize);
    pl=(long*)grib_context_malloc(c,sizeof(long)*plsize);
    if (!pl) return GRIB_OUT_OF_MEMORY;

    grib_get_long_array_internal(h,spl,pl, &plsize);

    self->las = (double*)grib_context_malloc(h->context,iter->nv*sizeof(double));
    if (!self->las) return GRIB_OUT_OF_MEMORY;
    self->los = (double*)grib_context_malloc(h->context,iter->nv*sizeof(double));
    if (!self->los) return GRIB_OUT_OF_MEMORY;

    while (lon_last<0) lon_last+=360;
    while (lon_first<0) lon_first+=360;

    /* Find the maximum element of "pl" array, do not assume it's 4*N! */
    /* This could be an Octahedral Gaussian Grid */
    max_pl = pl[0];
    for (j=1; j<plsize; j++) {
        if (pl[j] > max_pl) max_pl = pl[j];
    }

    is_global = is_gaussian_global(lat_first, lat_last, lon_first, lon_last, max_pl, lats, angular_precision);
    if ( !is_global ) {
        /*sub area*/
        ret = iterate_reduced_gaussian_subarea_wrapper(iter, h, lat_first, lon_first, lat_last, lon_last, lats, pl, plsize);
    } else {
        /*global*/
        iter->e=0;
        if (h->context->debug) {
            const size_t np = count_global_points(pl, plsize);
            printf("ECCODES DEBUG grib_iterator_class_gaussian_reduced: global num points=%ld\n", np);
        }

        for (j=0;j<plsize;j++) {
            row_count=pl[j];
            for (i=0;i<row_count;i++) {

                if(iter->e >= iter->nv){
                    /*grib_context_log(h->context,GRIB_LOG_ERROR, "Failed to initialise reduced Gaussian iterator (global)");*/
                    /*return GRIB_WRONG_GRID;*/
                    /*Try now as NON-global*/
                    ret = iterate_reduced_gaussian_subarea_wrapper(iter, h, lat_first, lon_first, lat_last, lon_last, lats, pl, plsize);
                    if (ret !=GRIB_SUCCESS) grib_context_log(h->context,GRIB_LOG_ERROR, "Failed to initialise reduced Gaussian iterator (global)");
                    goto finalise;
                }

                self->los[iter->e]=(i*360.0)/row_count;
                self->las[iter->e]=lats[j];
                iter->e++;
            }
        }
    }

finalise:
    iter->e = -1;
    grib_context_free(h->context,lats);
    grib_context_free(h->context,pl);

    return ret;
}

static int destroy(grib_iterator* i)
{
    grib_iterator_gaussian_reduced* self = (grib_iterator_gaussian_reduced*)i;
    const grib_context *c = i->h->context;

    grib_context_free(c,self->las);
    grib_context_free(c,self->los);
    return 1;
}
