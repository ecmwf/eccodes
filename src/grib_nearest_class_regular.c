/*
 * Copyright 2005-2018 ECMWF.
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
   SUPER      = grib_nearest_class_gen
   IMPLEMENTS = init
   IMPLEMENTS = find;destroy
   MEMBERS    = double* lats
   MEMBERS    = int  lats_count
   MEMBERS    = double* lons
   MEMBERS    = int  lons_count
   MEMBERS    = double* distances
   MEMBERS    = int* k
   MEMBERS    = int* i
   MEMBERS    = int* j
   MEMBERS    = const char* Ni
   MEMBERS    = const char* Nj
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

typedef struct grib_nearest_regular{
  grib_nearest nearest;
/* Members defined in gen */
	const char* values_key;
	const char* radius;
	int cargs;
/* Members defined in regular */
	double* lats;
	int  lats_count;
	double* lons;
	int  lons_count;
	double* distances;
	int* k;
	int* i;
	int* j;
	const char* Ni;
	const char* Nj;
} grib_nearest_regular;

extern grib_nearest_class* grib_nearest_class_gen;

static grib_nearest_class _grib_nearest_class_regular = {
    &grib_nearest_class_gen,                         /* super                     */
    "regular",                         /* name                      */
    sizeof(grib_nearest_regular),      /* size of instance          */
    0,                              /* inited */
    &init_class,                    /* init_class */
    &init,                          /* constructor               */
    &destroy,                       /* destructor                */
    &find,                          /* find nearest              */
};

grib_nearest_class* grib_nearest_class_regular = &_grib_nearest_class_regular;


static void init_class(grib_nearest_class* c)
{
}
/* END_CLASS_IMP */


static int init(grib_nearest* nearest,grib_handle* h,grib_arguments* args)
{
    grib_nearest_regular* self = (grib_nearest_regular*) nearest;
    self->Ni  = grib_arguments_get_name(h,args,self->cargs++);
    self->Nj  = grib_arguments_get_name(h,args,self->cargs++);
    self->i=(int*)grib_context_malloc(h->context,2*sizeof(int));
    self->j=(int*)grib_context_malloc(h->context,2*sizeof(int));
    return 0;
}

#if 0
static int find(grib_nearest* nearest, grib_handle* h,
        double inlat, double inlon,unsigned long flags,
        double* outlats,double* outlons,
        double *values,double *distances,int* indexes, size_t *len) {
    grib_nearest_regular* self = (grib_nearest_regular*) nearest;
    int ret=0,kk=0,ii=0,jj=0;
    size_t nvalues=0;

    long iradius;
    double radius;

    if( (ret =  grib_get_long(h,self->radius,&iradius))!= GRIB_SUCCESS)
        return ret;
    radius=((double)iradius)/1000.0;

    if (!nearest->h || (flags & GRIB_NEAREST_SAME_DATA)==0 || nearest->h!=h) {
        grib_iterator* iter=NULL;
        double lat=0,lon=0;

        if( (ret =  grib_get_size(h,self->values_key,&nvalues))!= GRIB_SUCCESS)
            return ret;
        nearest->values_count = nvalues;
        if (nearest->values) grib_context_free(nearest->context,nearest->values);
        nearest->values = grib_context_malloc(h->context,nvalues*sizeof(double));
        if (!nearest->values) return GRIB_OUT_OF_MEMORY;

        ret=grib_get_double_array_internal( h,self->values_key,
                nearest->values,&(nearest->values_count));
        if (ret!=GRIB_SUCCESS) grib_context_log(nearest->context,GRIB_LOG_ERROR,
                "nearest: unable to get values array");

        if (!nearest->h || (flags & GRIB_NEAREST_SAME_GRID)==0) {
            double dummy=0;
            double olat=1.e10, olon=1.e10;
            int ilat=0,ilon=0;
            long n=0;

            if( (ret =  grib_get_long(h,self->Ni,&n))!= GRIB_SUCCESS)
                return ret;
            self->lons_count=n;

            if( (ret =  grib_get_long(h,self->Nj,&n))!= GRIB_SUCCESS)
                return ret;
            self->lats_count=n;

            if (self->lats) grib_context_free(nearest->context,self->lats);
            self->lats=grib_context_malloc( nearest->context,
                    self->lats_count* sizeof(double));
            if (!self->lats) return GRIB_OUT_OF_MEMORY;

            if (self->lons) grib_context_free(nearest->context,self->lons);
            self->lons=grib_context_malloc( nearest->context,
                    self->lons_count*sizeof(double));
            if (!self->lons) return GRIB_OUT_OF_MEMORY;

            iter=grib_iterator_new(h,0,&ret);
            if (ret) {
                grib_context_log(nearest->context,GRIB_LOG_ERROR,"unable to create iterator");
                return ret;
            }
            while(grib_iterator_next(iter,&lat,&lon,&dummy)) {
                if (olat != lat) {
                    self->lats[ilat++]=lat;
                    olat=lat;
                }
                if (ilon<self->lons_count && olon != lon) {
                    self->lons[ilon++]=lon;
                    olon=lon;
                }
            }
            grib_iterator_delete(iter);

        }
        nearest->h=h;

    }

    if (!self->distances || (flags & GRIB_NEAREST_SAME_POINT)==0
            || (flags & GRIB_NEAREST_SAME_GRID)==0) {

        grib_binary_search(self->lats,self->lats_count-1,inlat,
                &(self->j[0]),&(self->j[1]));
        grib_binary_search(self->lons,self->lons_count-1,inlon,
                &(self->i[0]),&(self->i[1]));
        if (!self->distances)
            self->distances=(double*)grib_context_malloc( nearest->context,4*sizeof(double));
        if (!self->k)
            self->k=(int*)grib_context_malloc( nearest->context,4*sizeof(int));
        kk=0;
        for (ii=0;ii<2;ii++) {
            for (jj=0;jj<2;jj++) {
                self->k[kk]=self->i[ii]+self->lons_count*self->j[jj]-1;
                self->distances[kk]=grib_nearest_distance(radius,inlon,inlat,
                        self->lons[self->i[ii]],self->lats[self->j[jj]]);
                kk++;
            }
        }
    }

    kk=0;
    for (ii=0;ii<2;ii++) {
        for (jj=0;jj<2;jj++) {
            distances[kk]=self->distances[kk];
            outlats[kk]=self->lats[self->j[jj]];
            outlons[kk]=self->lons[self->i[ii]];
            values[kk]=nearest->values[self->k[kk]];
            indexes[kk]=self->k[kk];
            kk++;
        }
    }

    return GRIB_SUCCESS;
}
#else
static int is_rotated_grid(grib_handle* h)
{
    long is_rotated = 0;
    int err = grib_get_long(h, "is_rotated_grid", &is_rotated);
    if (!err && is_rotated ) return 1;
    return 0;
}

static int find(grib_nearest* nearest, grib_handle* h,
        double inlat, double inlon,unsigned long flags,
        double* outlats,double* outlons,
        double *values,double *distances,int* indexes, size_t *len)
{
    grib_nearest_regular* self = (grib_nearest_regular*) nearest;
    int ret=0,kk=0,ii=0,jj=0;
    size_t nvalues=0;
    long iradius;
    double radius;

    grib_iterator* iter=NULL;
    double lat=0,lon=0;

    while (inlon<0) inlon+=360;
    while (inlon>360) inlon-=360;

    if( (ret =  grib_get_size(h,self->values_key,&nvalues))!= GRIB_SUCCESS)
        return ret;
    nearest->values_count = nvalues;

    if (grib_is_missing(h,self->radius,&ret)) {
        grib_context_log(h->context, GRIB_LOG_DEBUG,"Key '%s' is missing", self->radius);
        return ret ? ret : GRIB_GEOCALCULUS_PROBLEM;
    }

    if( (ret =  grib_get_long(h,self->radius,&iradius))!= GRIB_SUCCESS)
        return ret;
    radius=((double)iradius)/1000.0;

    if (!nearest->h || (flags & GRIB_NEAREST_SAME_GRID)==0) {
        double dummy=0;
        double olat=1.e10, olon=1.e10;
        int ilat=0,ilon=0;
        long n=0;

        if (grib_is_missing(h,self->Ni,&ret)) {
            grib_context_log(h->context, GRIB_LOG_DEBUG,"Key '%s' is missing", self->Ni);
            return ret ? ret : GRIB_GEOCALCULUS_PROBLEM;
        }

        if (grib_is_missing(h,self->Nj,&ret)) {
            grib_context_log(h->context, GRIB_LOG_DEBUG,"Key '%s' is missing", self->Nj);
            return ret ? ret : GRIB_GEOCALCULUS_PROBLEM;
        }

        /* Support for rotated grids not yet implemented */
        if (is_rotated_grid(h)) {
            grib_context_log(h->context,GRIB_LOG_ERROR,
                    "Nearest neighbour functionality is not supported for rotated grids.");
            return GRIB_NOT_IMPLEMENTED;
        }

        if ((ret =  grib_get_long(h,self->Ni,&n))!= GRIB_SUCCESS)
            return ret;
        self->lons_count=n;

        if ((ret =  grib_get_long(h,self->Nj,&n))!= GRIB_SUCCESS)
            return ret;
        self->lats_count=n;

        if (self->lats) grib_context_free(nearest->context,self->lats);
        self->lats=(double*)grib_context_malloc( nearest->context,
                self->lats_count* sizeof(double));
        if (!self->lats) return GRIB_OUT_OF_MEMORY;

        if (self->lons) grib_context_free(nearest->context,self->lons);
        self->lons=(double*)grib_context_malloc( nearest->context,
                self->lons_count*sizeof(double));
        if (!self->lons) return GRIB_OUT_OF_MEMORY;

        iter=grib_iterator_new(h,0,&ret);
        if (ret != GRIB_SUCCESS) {
            grib_context_log(h->context,GRIB_LOG_ERROR,"Unable to create lat/lon iterator");
            return ret;
        }
        while(grib_iterator_next(iter,&lat,&lon,&dummy)) {
            if (olat != lat) {
                Assert( ilat < self->lats_count );
                self->lats[ilat++]=lat;
                olat=lat;
            }
            if (ilon<self->lons_count && olon != lon) {
                self->lons[ilon++]=lon ;
                olon=lon;
            }
        }
        grib_iterator_delete(iter);

    }
    nearest->h=h;

    if (!self->distances || (flags & GRIB_NEAREST_SAME_POINT)==0
            || (flags & GRIB_NEAREST_SAME_GRID)==0) {
        int nearest_lons_found=0;

        if (self->lats[self->lats_count-1] > self->lats[0]) {
            if (inlat<self->lats[0] || inlat>self->lats[self->lats_count-1])
                return GRIB_OUT_OF_AREA;
        } else {
            if (inlat > self->lats[0] || inlat < self->lats[self->lats_count-1])
                return GRIB_OUT_OF_AREA;
        }

        if (self->lons[self->lons_count-1] > self->lons[0]) {
            if (inlon<self->lons[0] || inlon>self->lons[self->lons_count-1]) {
                /* try to scale*/
                if (inlon>0) inlon-=360;
                else inlon+=360;

                if (inlon<self->lons[0] || inlon>self->lons[self->lons_count-1]) {
                    if (	self->lons[0]+360-self->lons[self->lons_count-1]<=
                            self->lons[1]-self->lons[0]) {
                        /*it's a global field in longitude*/
                        self->i[0]=0;
                        self->i[1]=self->lons_count-1;
                        nearest_lons_found=1;
                    } else
                        return GRIB_OUT_OF_AREA;
                }
            }
        } else {
            if (inlon>self->lons[0] || inlon<self->lons[self->lons_count-1]) {
                /* try to scale*/
                if (inlon>0) inlon-=360;
                else inlon+=360;
                if (self->lons[0]-self->lons[self->lons_count-1]-360 <=
                        self->lons[0]-self->lons[1]) {
                    /*it's a global field in longitude*/
                    self->i[0]=0;
                    self->i[1]=self->lons_count-1;
                    nearest_lons_found=1;
                } else if (inlon>self->lons[0] || inlon<self->lons[self->lons_count-1])
                    return GRIB_OUT_OF_AREA;
            }
        }

        grib_binary_search(self->lats,self->lats_count-1,inlat,
                &(self->j[0]),&(self->j[1]));

        if (!nearest_lons_found)
            grib_binary_search(self->lons,self->lons_count-1,inlon,
                    &(self->i[0]),&(self->i[1]));

        if (!self->distances)
            self->distances=(double*)grib_context_malloc( nearest->context,4*sizeof(double));
        if (!self->k)
            self->k=(int*)grib_context_malloc( nearest->context,4*sizeof(int));
        kk=0;
        for (jj=0;jj<2;jj++) {
            for (ii=0;ii<2;ii++) {
                self->k[kk]=self->i[ii]+self->lons_count*self->j[jj];
                self->distances[kk]=grib_nearest_distance(radius,inlon,inlat,
                        self->lons[self->i[ii]],self->lats[self->j[jj]]);
                kk++;
            }
        }
    }

    kk=0;

    /*
     * Brute force algorithm:
     * First unpack all the values into an array. Then when we need the 4 points
     * we just index into this array so no need to call grib_get_double_element_internal
     *
     *   if (nearest->values) grib_context_free(nearest->context,nearest->values);
     *   nearest->values = grib_context_malloc(h->context,nvalues*sizeof(double));
     *   if (!nearest->values) return GRIB_OUT_OF_MEMORY;
     *   ret = grib_get_double_array(h, self->values_key, nearest->values ,&nvalues);
     *   if (ret) return ret;
     */

    for (jj=0;jj<2;jj++) {
        for (ii=0;ii<2;ii++) {
            distances[kk]=self->distances[kk];
            outlats[kk]=self->lats[self->j[jj]];
            outlons[kk]=self->lons[self->i[ii]];
            grib_get_double_element_internal(h,self->values_key,self->k[kk],&(values[kk]));
            /* Using the brute force approach described above */
            /* Assert(self->k[kk] < nvalues); */
            /* values[kk]=nearest->values[self->k[kk]]; */

            indexes[kk]=self->k[kk];
            kk++;
        }
    }

    return GRIB_SUCCESS;
}
#endif

static int destroy(grib_nearest* nearest) {
    grib_nearest_regular* self = (grib_nearest_regular*) nearest;
    if (self->lats) grib_context_free(nearest->context,self->lats);
    if (self->lons) grib_context_free(nearest->context,self->lons);
    if (self->i) grib_context_free(nearest->context,self->i);
    if (self->j) grib_context_free(nearest->context,self->j);
    if (self->k) grib_context_free(nearest->context,self->k);
    if (self->distances) grib_context_free(nearest->context,self->distances);
    return GRIB_SUCCESS;
}
