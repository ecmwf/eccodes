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

typedef struct grib_nearest_lambert_conformal{
  grib_nearest nearest;
/* Members defined in gen */
	const char* values_key;
	const char* radius;
	int cargs;
/* Members defined in lambert_conformal */
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
} grib_nearest_lambert_conformal;

extern grib_nearest_class* grib_nearest_class_gen;

static grib_nearest_class _grib_nearest_class_lambert_conformal = {
    &grib_nearest_class_gen,                         /* super                     */
    "lambert_conformal",                         /* name                      */
    sizeof(grib_nearest_lambert_conformal),      /* size of instance          */
    0,                              /* inited */
    &init_class,                    /* init_class */
    &init,                          /* constructor               */
    &destroy,                       /* destructor                */
    &find,                          /* find nearest              */
};

grib_nearest_class* grib_nearest_class_lambert_conformal = &_grib_nearest_class_lambert_conformal;


static void init_class(grib_nearest_class* c)
{
}
/* END_CLASS_IMP */

static int init(grib_nearest* nearest,grib_handle* h,grib_arguments* args)
{
    grib_nearest_lambert_conformal* self = (grib_nearest_lambert_conformal*) nearest;
    self->Ni  = grib_arguments_get_name(h,args,self->cargs++);
    self->Nj  = grib_arguments_get_name(h,args,self->cargs++);
    self->i=(int*)grib_context_malloc(h->context,2*sizeof(int));
    self->j=(int*)grib_context_malloc(h->context,2*sizeof(int));
    return 0;
}

static int compare_doubles( const void* a, const void* b, int ascending )
{
    /* ascending is a boolean: 0 or 1 */
    double* arg1 = (double*) a;
    double* arg2 = (double*) b;
    if (ascending) {
        if( *arg1 < *arg2 ) return -1;		/*Smaller values come before larger ones*/
    } else {
        if( *arg1 > *arg2 ) return -1;		/*Larger values come before smaller ones*/
    }
    if( *arg1 == *arg2 ) return 0;
    else return 1;
}

static int compare_doubles_ascending( const void* a, const void* b )
{
    return compare_doubles(a,b,1);
}

typedef struct PointStore {
    double m_lat;
    double m_lon;
    double m_dist;
    double m_value;
    int    m_index;
} PointStore ;

/* Comparison function to sort points by distance */
static int compare_points(const void* a, const void* b)
{
    PointStore *pA = (PointStore*)a;
    PointStore *pB = (PointStore*)b;

    if (pA->m_dist < pB->m_dist) return -1;
    if (pA->m_dist > pB->m_dist) return 1;
    return 0;
}

static int find(grib_nearest* nearest, grib_handle* h,
        double inlat, double inlon,unsigned long flags,
        double* outlats,double* outlons,
        double *values,double *distances,int* indexes, size_t *len)
{
    grib_nearest_lambert_conformal* self = (grib_nearest_lambert_conformal*) nearest;
    int ret=0, i=0;
    size_t nvalues=0;
    long iradius;
    double radius;
    grib_iterator* iter=NULL;
    double lat=0,lon=0;

    /* array of candidates for nearest neighbours */
    PointStore* neighbours = NULL;

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

    neighbours = (PointStore*)grib_context_malloc( nearest->context, nvalues*sizeof(PointStore) );
    for(i=0; i<nvalues; ++i) {
        neighbours[i].m_dist = 1e10; /* set all distances to large number to begin with */
        neighbours[i].m_lat=0;
        neighbours[i].m_lon=0;
        neighbours[i].m_value=0;
        neighbours[i].m_index=0;
    }

    /* GRIB_NEAREST_SAME_GRID not yet implemented */
    {
        double the_value = 0;
        double min_dist = 1e10;
        size_t the_index = 0;
        int ilat=0, ilon=0;
        int idx_upper=0, idx_lower=0;
        double lat1=0, lat2=0; /* inlat will be between these */
        double dist=0;
        const double LAT_DELTA = 10.0; /* in degrees */

        if (grib_is_missing(h,self->Ni,&ret)) {
            grib_context_log(h->context, GRIB_LOG_DEBUG,"Key '%s' is missing", self->Ni);
            return ret ? ret : GRIB_GEOCALCULUS_PROBLEM;
        }

        if (grib_is_missing(h,self->Nj,&ret)) {
            grib_context_log(h->context, GRIB_LOG_DEBUG,"Key '%s' is missing", self->Nj);
            return ret ? ret : GRIB_GEOCALCULUS_PROBLEM;
        }

        self->lons_count=nvalues;  /* Maybe overestimate but safe */
        self->lats_count=nvalues;

        if (self->lats) grib_context_free(nearest->context,self->lats);
        self->lats=(double*)grib_context_malloc( nearest->context, nvalues* sizeof(double));
        if (!self->lats) return GRIB_OUT_OF_MEMORY;

        if (self->lons) grib_context_free(nearest->context,self->lons);
        self->lons=(double*)grib_context_malloc( nearest->context, nvalues*sizeof(double));
        if (!self->lons) return GRIB_OUT_OF_MEMORY;

        iter=grib_iterator_new(h,0,&ret);
        if (ret) return ret;
        /* First pass: collect all latitudes and longitudes */
        while(grib_iterator_next(iter,&lat,&lon,&the_value))
        {
            ++the_index;
            Assert(ilat < self->lats_count);
            Assert(ilon < self->lons_count);
            self->lats[ilat++]=lat;
            self->lons[ilon++]=lon;
        }

        /* See between which 2 latitudes our point lies */
        qsort(self->lats, nvalues, sizeof(double), &compare_doubles_ascending);
        grib_binary_search(self->lats, self->lats_count-1, inlat, &idx_upper, &idx_lower);
        lat2 = self->lats[idx_upper];
        lat1 = self->lats[idx_lower];
        Assert(lat1<=lat2);

        /* Second pass: Iterate again and collect candidate neighbours */
        grib_iterator_reset(iter);
        the_index=0;
        i = 0;
        while(grib_iterator_next(iter,&lat,&lon,&the_value))
        {
            if (lat > lat2+LAT_DELTA || lat < lat1-LAT_DELTA) {
                /* Ignore latitudes too far from our point */
            }
            else {
                dist = grib_nearest_distance(radius, inlon, inlat, lon, lat);
                if (dist < min_dist) min_dist = dist;
                /*printf("Candidate: lat=%.5f lon=%.5f dist=%f Idx=%ld Val=%f\n",lat,lon,dist,the_index,the_value);*/
                /* store this candidate point */
                neighbours[i].m_dist  = dist;
                neighbours[i].m_index = the_index;
                neighbours[i].m_lat   = lat;
                neighbours[i].m_lon   = lon;
                neighbours[i].m_value = the_value;
                i++;
            }
            ++the_index;
        }
        /* Sort the candidate neighbours in ascending order of distance */
        /* The first 4 entries will now be the closest 4 neighbours */
        qsort(neighbours, nvalues, sizeof(PointStore), &compare_points);

        grib_iterator_delete(iter);
    }
    nearest->h=h;

    /* Sanity check for sorting */
#ifdef DEBUG
    for(i=0; i<nvalues-1; ++i) {
        Assert( neighbours[i].m_dist <= neighbours[i+1].m_dist);
    }
#endif

    /* GRIB_NEAREST_SAME_XXX not yet implemented */
    if (!self->distances) {
        self->distances=(double*)grib_context_malloc( nearest->context,4*sizeof(double));
    }
    self->distances[0] = neighbours[0].m_dist;
    self->distances[1] = neighbours[1].m_dist;
    self->distances[2] = neighbours[2].m_dist;
    self->distances[3] = neighbours[3].m_dist;

    for(i=0; i <4; ++i)
    {
        distances[i] = neighbours[i].m_dist;
        outlats[i]   = neighbours[i].m_lat;
        outlons[i]   = neighbours[i].m_lon;
        indexes[i]   = neighbours[i].m_index;
        values[i]    = neighbours[i].m_value;
        /*printf("(%f,%f)  i=%d  d=%f  v=%f\n",outlats[i],outlons[i],indexes[i],distances[i],values[i]);*/
    }

    free(neighbours);
    return GRIB_SUCCESS;
}

static int destroy(grib_nearest* nearest)
{
    grib_nearest_lambert_conformal* self = (grib_nearest_lambert_conformal*) nearest;
    if (self->lats) grib_context_free(nearest->context,self->lats);
    if (self->lons) grib_context_free(nearest->context,self->lons);
    if (self->i) grib_context_free(nearest->context,self->i);
    if (self->j) grib_context_free(nearest->context,self->j);
    if (self->k) grib_context_free(nearest->context,self->k);
    if (self->distances) grib_context_free(nearest->context,self->distances);
    return GRIB_SUCCESS;
}
