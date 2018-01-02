/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/**
* Author: Enrico Fucile
* date: 31/10/2007
*
*/

#include "grib_api_internal.h"

int grib_nearest_find(
        grib_nearest *nearest, grib_handle* h,
        double inlat, double inlon,
        unsigned long flags,
        double* outlats,double* outlons,
        double* values, double* distances, int* indexes, size_t *len)
{
    grib_nearest_class *c = NULL;
    Assert(nearest);
    c = nearest->cclass;
    Assert( flags <= (GRIB_NEAREST_SAME_GRID|GRIB_NEAREST_SAME_DATA|GRIB_NEAREST_SAME_POINT) );

    while(c)
    {
        grib_nearest_class *s = c->super ? *(c->super) : NULL;
        if(c->find) {
            int ret=c->find(nearest,h,inlat,inlon,flags,outlats,outlons,values,distances,indexes,len);
            if (ret!=GRIB_SUCCESS) {
                if (inlon>0) inlon-=360;
                else inlon+=360;
                ret=c->find(nearest,h,inlat,inlon,flags,outlats,outlons,values,distances,indexes,len);
            }
            return ret;
        }
        c = s;
    }
    Assert(0);
    return 0;
}

/* For this one, ALL init are called */
static int init_nearest(grib_nearest_class* c,grib_nearest* i, grib_handle *h, grib_arguments* args)
{
    if(c) {
        int ret = GRIB_SUCCESS;
        grib_nearest_class *s = c->super ? *(c->super) : NULL;
        if(!c->inited)
        {
            if(c->init_class) c->init_class(c);
            c->inited = 1;
        }
        if(s) ret = init_nearest(s,i,h,args);

        if(ret != GRIB_SUCCESS) return ret;

        if(c->init) return c->init(i,h, args);
    }
    return GRIB_INTERNAL_ERROR;
}

int grib_nearest_init(grib_nearest* i, grib_handle *h, grib_arguments* args)
{
    return init_nearest(i->cclass,i,h,args);
}

/* For this one, ALL destroy are called */

int grib_nearest_delete(grib_nearest *i)
{
    grib_nearest_class *c = NULL;
    Assert(i);
    c = i->cclass;
    while(c)
    {
        grib_nearest_class *s = c->super ? *(c->super) : NULL;
        if(c->destroy) c->destroy(i);
        c = s;
    }
    return 0;
}

void grib_binary_search(double xx[], const unsigned long n, double x,
        int *ju,int *jl)
{
    size_t jm=0;
    int ascending=0;
    *jl=0;
    *ju=n;
    ascending=(xx[n] >= xx[0]);
    while (*ju-*jl > 1) {
        jm=(*ju+*jl) >> 1;
        if ((x >= xx[jm]) == ascending) *jl=jm;
        else *ju=jm;
    }
}

#define RADIAN(x) ((x) * acos(0.0) / 90.0)

double grib_nearest_distance(double radius,double lon1, double lat1, double lon2, double lat2)
{
    double rlat1=RADIAN(lat1);
    double rlat2=RADIAN(lat2);
    double rlon1=lon1;
    double rlon2=lon2;
    double a;

    if (lat1 == lat2 && lon1 == lon2) {
        return 0.0; /* the two points are identical */
    }

    if (rlon1 >=360) rlon1-=360.0;
    rlon1=RADIAN(rlon1);
    if (rlon2 >=360) rlon2-=360.0;
    rlon2=RADIAN(rlon2);

    a = sin(rlat1)*sin(rlat2)+ cos(rlat1)*cos(rlat2)*cos(rlon2-rlon1);

    if (a > 1 || a < -1 ) a=(int)a;

    return radius*acos(a);
}

int grib_nearest_find_multiple(grib_handle* h,int is_lsm,
        double* inlats,double* inlons,long npoints,
        double* outlats,double* outlons,
        double* values,double* distances, int* indexes)
{
    grib_nearest* nearest=0;
    double* pdistances=distances;
    double* poutlats=outlats;
    double* poutlons=outlons;
    double* pvalues=values;
    int* pindexes=indexes;
    int idx=0,ii=0;
    double max,min;
    double qdistances[4]={0,};
    double qoutlats[4]={0,};
    double qoutlons[4]={0,};
    double qvalues[4]={0,};
    int qindexes[4]={0,};
    int ret=0;
    long i=0;
    size_t len=4;
    int flags=GRIB_NEAREST_SAME_GRID | GRIB_NEAREST_SAME_DATA;

    nearest=grib_nearest_new(h,&ret);
    if (ret!=GRIB_SUCCESS) return ret;

    if (is_lsm) {
        int noland=1;
        for (i=0;i<npoints;i++) {
            ret=grib_nearest_find(nearest,h,inlats[i],inlons[i],flags,qoutlats,qoutlons,
                    qvalues,qdistances,qindexes,&len);
            max=qdistances[0];
            for (ii=0;ii<4;ii++) {
                if (max<qdistances[ii]) {max=qdistances[ii];idx=ii;}
                if (qvalues[ii] >= 0.5) noland=0;
            }
            min=max;
            for (ii=0;ii<4;ii++) {
                if ((min >= qdistances[ii]) && ( noland || (qvalues[ii] >= 0.5)) ) {
                    min = qdistances[ii];
                    idx=ii;
                }
            }
            *poutlats=qoutlats[idx];poutlats++;
            *poutlons=qoutlons[idx];poutlons++;
            *pvalues=qvalues[idx];pvalues++;
            *pdistances=qdistances[idx];pdistances++;
            *pindexes=qindexes[idx];pindexes++;

        }
    } else {
        for (i=0;i<npoints;i++) {
            ret=grib_nearest_find(nearest,h,inlats[i],inlons[i],flags,qoutlats,qoutlons,
                    qvalues,qdistances,qindexes,&len);
            min=qdistances[0];
            for (ii=0;ii<4;ii++) {
                if ((min >= qdistances[ii])) {
                    min = qdistances[ii];
                    idx=ii;
                }
            }
            *poutlats=qoutlats[idx];poutlats++;
            *poutlons=qoutlons[idx];poutlons++;
            *pvalues=qvalues[idx];pvalues++;
            *pdistances=qdistances[idx];pdistances++;
            *pindexes=qindexes[idx];pindexes++;
        }
    }

    grib_nearest_delete(nearest);

    return ret;
}
