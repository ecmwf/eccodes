/*
 * Copyright 2005-2019 ECMWF.
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

/* Search for 'x' in the array 'xx' (the index of last element being 'n') and return index in 'j' */
static void binary_search(double xx[], const unsigned long n, double x, long *j)
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

/* Use legacy way to compute the iterator latitude/longitude values */
static int iterate_reduced_gaussian_subarea_legacy(grib_iterator* iter, grib_handle* h,
        double lat_first, double lon_first,
        double lat_last, double lon_last,
        double* lats, long* pl, size_t plsize)
{
    int err = 0;
    int l = 0;
    size_t j = 0;
    long row_count=0;
    double d=0;
    long ilon_first, ilon_last, i;
    grib_iterator_gaussian_reduced* self = (grib_iterator_gaussian_reduced*)iter;
    get_reduced_row_proc get_reduced_row = &grib_get_reduced_row;
    get_reduced_row = &grib_get_reduced_row_legacy; /* legacy algorithm */

    if (h->context->debug) {
        const size_t np = count_subarea_points(h, get_reduced_row, pl, plsize, lon_first, lon_last);
        fprintf(stderr,"ECCODES DEBUG grib_iterator_class_gaussian_reduced: Legacy sub-area num points=%ld\n", (long)np);
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
                size_t np = count_subarea_points(h, get_reduced_row, pl, plsize, lon_first, lon_last);
                grib_context_log(h->context,GRIB_LOG_ERROR,
                                 "Reduced Gaussian iterator (sub-area legacy). Num points=%ld, size(values)=%ld", np, iter->nv);
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

/* ECC-747 */
static int iterate_reduced_gaussian_subarea_algorithm2(grib_iterator* iter, grib_handle* h,
        double lat_first, double lon_first,
        double lat_last, double lon_last,
        double* lats, long* pl, size_t plsize, size_t numlats)
{
    int err = 0;
    long l = 0;
    size_t j = 0;
    long row_count=0, i=0;
    double olon_first, olon_last;
    grib_iterator_gaussian_reduced* self = (grib_iterator_gaussian_reduced*)iter;
    get_reduced_row_proc get_reduced_row = &grib_get_reduced_row;

    if (h->context->debug) {
        const size_t np = count_subarea_points(h, get_reduced_row, pl, plsize, lon_first, lon_last);
        fprintf(stderr, "ECCODES DEBUG grib_iterator_class_gaussian_reduced: sub-area num points=%ld\n", (long)np);
    }

    /* Find starting latitude */
    binary_search(lats, numlats-1, lat_first, &l);
    Assert(l < numlats);

#if 0
    for(il=0; il<numlats; ++il) {
        const double diff = fabs(lat_first-lats[il]);
        if (diff < min_d) {
            min_d = diff;
            l = il; /* index of the latitude */
        }
    }
#endif

    iter->e=0;
    for (j=0;j<plsize;j++) {
        const double delta = 360.0/pl[j];
        row_count=0;
        grib_get_reduced_row_p(pl[j],lon_first,lon_last, &row_count,&olon_first,&olon_last);
        for(i=0; i<row_count; ++i) {
            double lon2 = olon_first + i * delta;
            if(iter->e >= iter->nv){
                /* Only print error message on the second pass */
                size_t np = count_subarea_points(h, get_reduced_row, pl, plsize, lon_first, lon_last);
                grib_context_log(h->context,GRIB_LOG_ERROR,
                                 "Reduced Gaussian iterator (sub-area). Num points=%ld, size(values)=%ld", np, iter->nv);
                return GRIB_WRONG_GRID;
            }
            self->los[iter->e]=lon2;
            DebugAssert(j+l < numlats);
            self->las[iter->e]=lats[j+l];
            iter->e++;
        }
    }

    if (iter->e != iter->nv) {
        /* Fewer counted points in the sub-area than the number of data values */
        const size_t legacy_count = count_subarea_points(h, grib_get_reduced_row_legacy, pl, plsize, lon_first, lon_last);
        if (iter->nv == legacy_count) {
            /* Legacy (produced by PRODGEN/LIBEMOS) */
            return iterate_reduced_gaussian_subarea_legacy(iter, h, lat_first, lon_first, lat_last, lon_last, lats, pl, plsize);
        } else {
            /* TODO: A gap exists! Not all values can be mapped. Inconsistent grid or error in calculating num. points! */
        }
    }
    return err;
}

static int iterate_reduced_gaussian_subarea_wrapper(grib_iterator* iter, grib_handle* h,
        double lat_first, double lon_first,
        double lat_last, double lon_last,
        double* lats, long* pl, size_t plsize, size_t numlats)
{
    return iterate_reduced_gaussian_subarea_algorithm2(iter, h, lat_first, lon_first, lat_last, lon_last, lats, pl, plsize, numlats);

#if 0
    /* Try legacy approach, if that fails try the next algorithm */
    int err = iterate_reduced_gaussian_subarea(iter, h, lat_first, lon_first, lat_last, lon_last, lats, pl, plsize, 0);
    if (err == GRIB_WRONG_GRID) {
        /* ECC-445: First attempt failed. Try again with a different algorithm */
        err = iterate_reduced_gaussian_subarea_algorithm2(iter, h, lat_first, lon_first, lat_last, lon_last, lats, pl, plsize);
    }
    return err;
#endif
}

static int init(grib_iterator* iter, grib_handle* h, grib_arguments* args)
{
    int ret=GRIB_SUCCESS, j, is_global=0;
    double lat_first=0,lon_first=0,lat_last=0,lon_last=0;
    double angular_precision = 1.0/1000000.0;
    double* lats;
    size_t plsize=0;
    size_t numlats=0;
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

    numlats = order*2;
    lats=(double*)grib_context_malloc(h->context,sizeof(double)*numlats);
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
        ret = iterate_reduced_gaussian_subarea_wrapper(iter, h, lat_first, lon_first, lat_last, lon_last, lats, pl, plsize, numlats);
    } else {
        /*global*/
        iter->e=0;
        if (h->context->debug) {
            const size_t np = count_global_points(pl, plsize);
            fprintf(stderr, "ECCODES DEBUG grib_iterator_class_gaussian_reduced: global num points=%ld\n", (long)np);
        }

        for (j=0;j<plsize;j++) {
            row_count=pl[j];
            for (i=0;i<row_count;i++) {

                if(iter->e >= iter->nv){
                    /*grib_context_log(h->context,GRIB_LOG_ERROR, "Failed to initialise reduced Gaussian iterator (global)");*/
                    /*return GRIB_WRONG_GRID;*/
                    /*Try now as NON-global*/
                    ret = iterate_reduced_gaussian_subarea_wrapper(iter, h, lat_first, lon_first, lat_last, lon_last, lats, pl, plsize, numlats);
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
