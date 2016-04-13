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
   SUPER      = grib_nearest_class_gen
   IMPLEMENTS = init;destroy;find
   MEMBERS    = double* lats
   MEMBERS    = int  lats_count
   MEMBERS    = double* lons
   MEMBERS    = double* distances
   MEMBERS    = int* k
   MEMBERS    = int* j
   MEMBERS    = const char* Nj
   MEMBERS    = const char* pl
   MEMBERS    = long global
   MEMBERS    = double lon_first
   MEMBERS    = double lon_last
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

typedef struct grib_nearest_reduced{
  grib_nearest nearest;
/* Members defined in gen */
	const char* values_key;
	const char* radius;
	int cargs;
/* Members defined in reduced */
	double* lats;
	int  lats_count;
	double* lons;
	double* distances;
	int* k;
	int* j;
	const char* Nj;
	const char* pl;
	long global;
	double lon_first;
	double lon_last;
} grib_nearest_reduced;

extern grib_nearest_class* grib_nearest_class_gen;

static grib_nearest_class _grib_nearest_class_reduced = {
    &grib_nearest_class_gen,                         /* super                     */
    "reduced",                         /* name                      */
    sizeof(grib_nearest_reduced),      /* size of instance          */
    0,                              /* inited */
    &init_class,                    /* init_class */
    &init,                          /* constructor               */
    &destroy,                       /* destructor                */
    &find,                          /* find nearest              */
};

grib_nearest_class* grib_nearest_class_reduced = &_grib_nearest_class_reduced;


static void init_class(grib_nearest_class* c)
{
}
/* END_CLASS_IMP */

static int init(grib_nearest* nearest,grib_handle* h,grib_arguments* args)
{
    grib_nearest_reduced* self = (grib_nearest_reduced*) nearest;
    self->Nj  = grib_arguments_get_name(h,args,self->cargs++);
    self->pl  = grib_arguments_get_name(h,args,self->cargs++);
    self->j=(int*)grib_context_malloc(h->context,2*sizeof(int));
    if (!self->j) return GRIB_OUT_OF_MEMORY;
    self->k=(int*)grib_context_malloc( nearest->context,4*sizeof(int));
    if (!self->k) return GRIB_OUT_OF_MEMORY;
    grib_get_long(h,"global",&self->global);
    if (!self->global)
    {
        int ret;
        /*TODO longitudeOfFirstGridPointInDegrees from the def file*/
        if ((ret=grib_get_double(h,"longitudeOfFirstGridPointInDegrees",&self->lon_first))!=GRIB_SUCCESS) {
            grib_context_log(h->context,GRIB_LOG_ERROR,
                    "grib_nearest_reduced.find(): unable to get longitudeOfFirstGridPointInDegrees %s\n",
                    grib_get_error_message(ret));
            return ret;
        }
        /*TODO longitudeOfLastGridPointInDegrees from the def file*/
        if ((ret=grib_get_double(h,"longitudeOfLastGridPointInDegrees",&self->lon_last))!=GRIB_SUCCESS) {
            grib_context_log(h->context,GRIB_LOG_ERROR,
                    "grib_nearest_reduced.find(): unable to get longitudeOfLastGridPointInDegrees %s\n",
                    grib_get_error_message(ret));
            return ret;
        }
    }

    return 0;
}

#if 1
static int find(grib_nearest* nearest, grib_handle* h,
        double inlat, double inlon,unsigned long flags,
        double* outlats,double* outlons, double *values,
        double *distances,int *indexes, size_t *len) {
    grib_nearest_reduced* self = (grib_nearest_reduced*) nearest;
    int ret=0,kk=0,ii=0,jj=0;
    long* pla=NULL;
    long* pl=NULL;
    size_t nvalues=0;
    grib_iterator* iter=NULL;
    double lat=0,lon=0;
    long iradius;
    double radius;
    int ilat=0,ilon=0;

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
        double olat=1.e10;
        long n=0;

        ilat=0,ilon=0;
        if (grib_is_missing(h,self->Nj,&ret)) {
            grib_context_log(h->context, GRIB_LOG_DEBUG,"Key '%s' is missing", self->Nj);
            return ret ? ret : GRIB_GEOCALCULUS_PROBLEM;
        }

        if( (ret =  grib_get_long(h,self->Nj,&n))!= GRIB_SUCCESS)
            return ret;
        self->lats_count=n;

        if (self->lats) grib_context_free(nearest->context,self->lats);
        self->lats=(double*)grib_context_malloc( nearest->context,
                self->lats_count* sizeof(double));
        if (!self->lats) return GRIB_OUT_OF_MEMORY;

        if (self->lons) grib_context_free(nearest->context,self->lons);
        self->lons=(double*)grib_context_malloc( nearest->context,
                nearest->values_count*sizeof(double));
        if (!self->lons) return GRIB_OUT_OF_MEMORY;

        iter=grib_iterator_new(h,0,&ret);
        if (ret != GRIB_SUCCESS) {
            grib_context_log(h->context,GRIB_LOG_ERROR,"Unable to create lat/lon iterator");
            return ret;
        }
        while(grib_iterator_next(iter,&lat,&lon,&dummy)) {
            if (olat!=lat) {
                self->lats[ilat++]=lat;
                olat=lat;
            }
            self->lons[ilon++]=lon;
        }
        self->lats_count=ilat;
        grib_iterator_delete(iter);

    }
    nearest->h=h;

    if (!self->distances || (flags & GRIB_NEAREST_SAME_POINT)==0
            || (flags & GRIB_NEAREST_SAME_GRID)==0) {
        double* lons=NULL;
        int nlon=0;
        size_t plsize=0;
        long nplm1=0;
        int nearest_lons_found=0;
        long row_count,ilon_first,ilon_last;

        if (self->global) {
            while (inlon<0) inlon+=360;
            while (inlon>360) inlon-=360;
        }

        ilat=self->lats_count;
        if (self->lats[ilat-1] > self->lats[0]) {
            if (inlat < self->lats[0] || inlat > self->lats[ilat-1])
                return GRIB_OUT_OF_AREA;
        } else {
            if (inlat > self->lats[0] || inlat < self->lats[ilat-1])
                return GRIB_OUT_OF_AREA;
        }

        if (!self->distances)
            self->distances=(double*)grib_context_malloc( nearest->context,4*sizeof(double));
        if (!self->distances) return GRIB_OUT_OF_MEMORY;

        grib_binary_search(self->lats,ilat-1,inlat,
                &(self->j[0]),&(self->j[1]));

        plsize=self->lats_count;
        if( (ret=grib_get_size(h,self->pl,&plsize))!= GRIB_SUCCESS)
            return ret;
        pla=(long*)grib_context_malloc(h->context,plsize*sizeof(long));
        if (!pla) return GRIB_OUT_OF_MEMORY;
        if( (ret=grib_get_long_array(h,self->pl,pla,&plsize))!= GRIB_SUCCESS)
            return ret;

        pl=pla;
        while ((*pl)==0) {pl++;}

        nlon=0;
        if (self->global) {
            for (jj=0;jj<self->j[0];jj++) nlon+=pl[jj];
            nplm1=pl[self->j[0]]-1;
        } else {
            nlon=0;
            for (jj=0;jj<self->j[0];jj++) {
                row_count=0;ilon_first=0;ilon_last=0;
                grib_get_reduced_row(pl[jj],self->lon_first,self->lon_last,&row_count,&ilon_first,&ilon_last);
                nlon+=row_count;
            }
            row_count=0;ilon_first=0;ilon_last=0;
            grib_get_reduced_row(pl[self->j[0]],self->lon_first,self->lon_last,&row_count,&ilon_first,&ilon_last);
            nplm1=row_count-1;
        }
        lons=self->lons+nlon;

        nearest_lons_found=0;
        if (lons[nplm1]>lons[0]) {
            if (inlon< lons[0] || inlon > lons[nplm1]) {
                if (lons[nplm1]-lons[0]-360 <=
                        lons[nplm1]-lons[nplm1-1]) {
                    self->k[0]=0;
                    self->k[1]=nplm1;
                    nearest_lons_found=1;
                } else return GRIB_OUT_OF_AREA;
            }
        } else {
            if (inlon >lons[0] || inlon< lons[nplm1]) {
                if (lons[0]-lons[nplm1]-360 <=
                        lons[0]-lons[1]) {
                    self->k[0]=0;
                    self->k[1]=nplm1;
                    nearest_lons_found=1;
                } else return GRIB_OUT_OF_AREA;
            }
        }

        if (!nearest_lons_found) {
            if (!self->global) {
                row_count=0;ilon_first=0;ilon_last=0;
                grib_get_reduced_row(pl[self->j[0]],self->lon_first,self->lon_last,&row_count,&ilon_first,&ilon_last);
            } else {
                row_count=pl[self->j[0]];
            }

            grib_binary_search(lons,row_count-1,inlon,
                    &(self->k[0]),&(self->k[1]));
        }
        self->k[0]+=nlon;
        self->k[1]+=nlon;

        nlon=0;
        if (self->global) {
            for (jj=0;jj<self->j[1];jj++) nlon+=pl[jj];
            nplm1=pl[self->j[1]]-1;
        } else {
            for (jj=0;jj<self->j[1];jj++) {
                row_count=0;ilon_first=0;ilon_last=0;
                grib_get_reduced_row(pl[jj],self->lon_first,self->lon_last,&row_count,&ilon_first,&ilon_last);
                nlon+=row_count;
            }
            row_count=0;ilon_first=0;ilon_last=0;
            grib_get_reduced_row(pl[self->j[1]],self->lon_first,self->lon_last,&nplm1,&ilon_first,&ilon_last);
            nplm1--;
        }
        lons=self->lons+nlon;

        nearest_lons_found=0;
        if (lons[nplm1]>lons[0]) {
            if (inlon<lons[0] || inlon>lons[nplm1]) {
                if (lons[nplm1]-lons[0]-360 <=
                        lons[nplm1]-lons[nplm1-1]) {
                    self->k[2]=0;
                    self->k[3]=nplm1;
                    nearest_lons_found=1;
                } else return GRIB_OUT_OF_AREA;
            }
        } else {
            if (inlon>lons[0] || inlon<lons[nplm1]) {
                if (lons[0]-lons[nplm1]-360 <=
                        lons[0]-lons[1]) {
                    self->k[2]=0;
                    self->k[3]=nplm1;
                    nearest_lons_found=1;
                } else return GRIB_OUT_OF_AREA;
            }
        }

        if (!nearest_lons_found) {
            if (!self->global) {
                row_count=0;ilon_first=0;ilon_last=0;
                grib_get_reduced_row(pl[self->j[1]],self->lon_first,self->lon_last,&row_count,&ilon_first,&ilon_last);
            } else {
                row_count=pl[self->j[1]];
            }

            grib_binary_search(lons,row_count-1,inlon,
                    &(self->k[2]),&(self->k[3]));
        }

        self->k[2]+=nlon;
        self->k[3]+=nlon;

        kk=0;
        for (jj=0;jj<2;jj++) {
            for (ii=0;ii<2;ii++) {
                self->distances[kk]=grib_nearest_distance(radius,inlon,inlat,
                        self->lons[self->k[kk]],self->lats[self->j[jj]]);
                kk++;
            }
        }

        grib_context_free(h->context,pla);
    }

    kk=0;
    for (jj=0;jj<2;jj++) {
        for (ii=0;ii<2;ii++) {
            distances[kk]=self->distances[kk];
            outlats[kk]=self->lats[self->j[jj]];
            outlons[kk]=self->lons[self->k[kk]];
            grib_get_double_element_internal(h,self->values_key,self->k[kk],&(values[kk]));
            indexes[kk]=self->k[kk];
            kk++;
        }
    }

    return GRIB_SUCCESS;
}
#else
static int find(grib_nearest* nearest, grib_handle* h,
        double inlat, double inlon,unsigned long flags,
        double* outlats,double* outlons, double *values,
        double *distances,double *distances,int *indexes,size_t *len) {
    grib_nearest_reduced* self = (grib_nearest_reduced*) nearest;
    int ret=0,kk=0,ii=0,jj=0;
    double* pl=NULL;
    int ilat;
    size_t nvalues=0;
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
            double olat=1.e10;
            ilat=0,ilon=0;
            long n=0;

            if (grib_is_missing(h,self->Nj,&ret)) {
                grib_context_log(h->context, GRIB_LOG_DEBUG,"Key '%s' is missing", self->Nj);
                return ret ? ret : GRIB_GEOCALCULUS_PROBLEM;
            }

            if( (ret =  grib_get_long(h,self->Nj,&n))!= GRIB_SUCCESS)
                return ret;
            self->lats_count=n;

            if (self->lats) grib_context_free(nearest->context,self->lats);
            self->lats=grib_context_malloc( nearest->context,
                    self->lats_count* sizeof(double));
            if (!self->lats) return GRIB_OUT_OF_MEMORY;

            if (self->lons) grib_context_free(nearest->context,self->lons);
            self->lons=grib_context_malloc( nearest->context,
                    nearest->values_count*sizeof(double));
            if (!self->lons) return GRIB_OUT_OF_MEMORY;

            iter=grib_iterator_new(h,0,&ret);
            while(grib_iterator_next(iter,&lat,&lon,&dummy)) {
                if (olat!=lat) {self->lats[ilat++]=lat;olat=lat;}
                self->lons[ilon++]=lon;
            }
            grib_iterator_delete(iter);
        }
        nearest->h=h;

    }

    if (!self->distances || (flags & GRIB_NEAREST_SAME_POINT)==0
            || (flags & GRIB_NEAREST_SAME_GRID)==0) {
        double* lons=NULL;
        int nlon=0;
        int plsize=0;

        self->distances=(double*)grib_context_malloc( nearest->context,4*sizeof(double));
        if (!self->distances) return GRIB_OUT_OF_MEMORY;

        grib_binary_search(self->lats,self->lats_count-1,inlat,
                &(self->j[0]),&(self->j[1]));

        plsize=self->lats_count;
        pl=(double*)grib_context_malloc(h->context,plsize*sizeof(double));
        if (!pl) return GRIB_OUT_OF_MEMORY;
        if( (ret=grib_get_double_array(h,"pl",pl,&plsize))!= GRIB_SUCCESS)
            return ret;

        nlon=0;
        for (jj=0;jj<self->j[0];jj++) {
            nlon+=pl[jj];
        }
        lons=self->lons+nlon;
        grib_binary_search(lons,pl[self->j[0]]-1,inlon,
                &(self->k[0]),&(self->k[1]));
        self->k[0]+=nlon;
        self->k[1]+=nlon;

        nlon=0;
        for (jj=0;jj<self->j[1];jj++) {
            nlon+=pl[jj];
        }
        lons=self->lons+nlon;
        grib_binary_search(lons,pl[self->j[1]]-1,inlon,
                &(self->k[2]),&(self->k[3]));
        self->k[2]+=nlon;
        self->k[3]+=nlon;

        kk=0;
        for (ii=0;ii<2;ii++) {
            for (jj=0;jj<2;jj++) {
                self->distances[kk]=grib_nearest_distance(radius,inlon,inlat,
                        self->lons[self->k[kk]],self->lats[self->j[jj]]);
                kk++;
            }
        }

        grib_context_free(h->context,pl);
    }

    kk=0;
    for (ii=0;ii<2;ii++) {
        for (jj=0;jj<2;jj++) {
            distances[kk]=self->distances[kk];
            outlats[kk]=self->lats[self->j[jj]];
            outlons[kk]=self->lons[self->k[kk]];
            values[kk]=nearest->values[self->k[kk]];
            indexes[kk]=self->k[kk];
            kk++;
        }
    }

    return GRIB_SUCCESS;
}
#endif

static int destroy(grib_nearest* nearest) {
    grib_nearest_reduced* self = (grib_nearest_reduced*) nearest;
    if (self->lats) grib_context_free(nearest->context,self->lats);
    if (self->lons) grib_context_free(nearest->context,self->lons);
    if (self->j) grib_context_free(nearest->context,self->j);
    if (self->k) grib_context_free(nearest->context,self->k);
    if (self->distances) grib_context_free(nearest->context,self->distances);

    return GRIB_SUCCESS;
}
