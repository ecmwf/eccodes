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
#include <math.h>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = iterator
   SUPER      = grib_iterator_class_gen
   IMPLEMENTS = destroy
   IMPLEMENTS = init;next
   MEMBERS     =   double *lats
   MEMBERS     =   double *lons
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


typedef struct grib_iterator_polar_stereographic{
  grib_iterator it;
/* Members defined in gen */
	long carg;
	const char* missingValue;
/* Members defined in polar_stereographic */
	double *lats;
	double *lons;
	long nam;
} grib_iterator_polar_stereographic;

extern grib_iterator_class* grib_iterator_class_gen;

static grib_iterator_class _grib_iterator_class_polar_stereographic = {
    &grib_iterator_class_gen,                    /* super                     */
    "polar_stereographic",                    /* name                      */
    sizeof(grib_iterator_polar_stereographic),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    &destroy,                  /* destructor                */
    &next,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_polar_stereographic = &_grib_iterator_class_polar_stereographic;


static void init_class(grib_iterator_class* c)
{
	c->previous	=	(*(c->super))->previous;
	c->reset	=	(*(c->super))->reset;
	c->has_next	=	(*(c->super))->has_next;
}
/* END_CLASS_IMP */

static int next(grib_iterator* i, double *lat, double *lon, double *val)
{
    grib_iterator_polar_stereographic* self = (grib_iterator_polar_stereographic*)i;

    if((long)i->e >= (long)(i->nv-1))
        return 0;
    i->e++;

    *lat = self->lats[i->e];
    *lon = self->lons[i->e];
    *val = i->data[i->e];

    return 1;
}

/* Data struct for Forward and Inverse Projections */
typedef struct proj_data_t {
    double centre_lon;  /* central longitude */
    double centre_lat;  /* central latitude */
    double sign;        /* sign variable */
    double ind;         /* flag variable */
    double mcs;         /* small m */
    double tcs;         /* small t */
    double false_northing; /* y offset in meters */
    double false_easting;  /* x offset in meters */
} proj_data_t;

#define RAD2DEG    57.29577951308232087684  /* 180 over pi */
#define DEG2RAD    0.01745329251994329576   /* pi over 180 */
#define PI_OVER_2  1.5707963267948966       /* half pi */
#define EPSILON    1.0e-10

static int init(grib_iterator* iter,grib_handle* h,grib_arguments* args)
{
    int ret=0;
    double *lats, *lons; /* arrays for latitudes and longitudes */
    double lonFirstInDegrees, latFirstInDegrees, radius;
    double x, y, Dx, Dy;
    long nx, ny, centralLongitudeInDegrees, centralLatitudeInDegrees;
    long alternativeRowScanning, iScansNegatively, i, j;
    long jScansPositively, jPointsAreConsecutive, southPoleOnPlane;
    double centralLongitude, centralLatitude; /* in radians */
    double con1;         /* temporary angle */
    double ts;           /* value of small t */
    double height;       /* height above ellipsoid */
    double x0, y0, lonFirst, latFirst;
    proj_data_t fwd_proj_data = {0,};
    proj_data_t inv_proj_data = {0,};

    grib_iterator_polar_stereographic* self = (grib_iterator_polar_stereographic*)iter;

    const char* sradius                 = grib_arguments_get_name(h,args,self->carg++);
    const char* snx                     = grib_arguments_get_name(h,args,self->carg++);
    const char* sny                     = grib_arguments_get_name(h,args,self->carg++);
    const char* slatFirstInDegrees      = grib_arguments_get_name(h,args,self->carg++);
    const char* slonFirstInDegrees      = grib_arguments_get_name(h,args,self->carg++);
    const char* ssouthPoleOnPlane       = grib_arguments_get_name(h,args,self->carg++);
    const char* scentralLongitude       = grib_arguments_get_name(h,args,self->carg++);
    const char* scentralLatitude        = grib_arguments_get_name(h,args,self->carg++);
    const char* sDx                     = grib_arguments_get_name(h,args,self->carg++);
    const char* sDy                     = grib_arguments_get_name(h,args,self->carg++);
    const char* siScansNegatively       = grib_arguments_get_name(h,args,self->carg++);
    const char* sjScansPositively       = grib_arguments_get_name(h,args,self->carg++);
    const char* sjPointsAreConsecutive  = grib_arguments_get_name(h,args,self->carg++);
    const char* salternativeRowScanning = grib_arguments_get_name(h,args,self->carg++);

    if((ret = grib_get_double_internal(h, sradius,&radius)) != GRIB_SUCCESS) return ret;
    if((ret = grib_get_long_internal(h, snx,&nx)) != GRIB_SUCCESS) return ret;
    if((ret = grib_get_long_internal(h, sny,&ny)) != GRIB_SUCCESS) return ret;

    if (iter->nv!=nx*ny) {
        grib_context_log(h->context,GRIB_LOG_ERROR, "Wrong number of points (%ld!=%ldx%ld)", iter->nv,nx,ny);
        return GRIB_WRONG_GRID;
    }
    if((ret = grib_get_double_internal(h, slatFirstInDegrees,&latFirstInDegrees)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, slonFirstInDegrees,&lonFirstInDegrees)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, ssouthPoleOnPlane,&southPoleOnPlane)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, scentralLongitude,&centralLongitudeInDegrees)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, scentralLatitude,&centralLatitudeInDegrees)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sDx,&Dx)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sDy,&Dy)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sjPointsAreConsecutive,&jPointsAreConsecutive)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sjScansPositively,&jScansPositively)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, siScansNegatively,&iScansNegatively)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, salternativeRowScanning,&alternativeRowScanning)) != GRIB_SUCCESS)
        return ret;

    centralLongitude = centralLongitudeInDegrees * DEG2RAD;
    centralLatitude = centralLatitudeInDegrees * DEG2RAD;
    lonFirst        = lonFirstInDegrees * DEG2RAD;
    latFirst        = latFirstInDegrees*DEG2RAD;

    /* Forward projection initialisation */
    fwd_proj_data.false_northing = 0;
    fwd_proj_data.false_easting = 0;
    fwd_proj_data.centre_lon = centralLongitude;
    fwd_proj_data.centre_lat = centralLatitude;
    if (centralLatitude < 0) fwd_proj_data.sign = -1.0;
    else                     fwd_proj_data.sign = +1.0;
    fwd_proj_data.ind = 0;
    if (fabs(fabs(centralLatitude) - PI_OVER_2) > EPSILON) {
        /* central latitude different from 90 i.e. not north/south polar */
        fwd_proj_data.ind = 1;
        con1 = fwd_proj_data.sign * centralLatitude;
        fwd_proj_data.mcs = cos(con1);
        fwd_proj_data.tcs = tan(0.5 * (PI_OVER_2 - con1));
    }

    /* Forward projection from initial lat,lon to initial x,y */
    con1 = fwd_proj_data.sign * (lonFirst - fwd_proj_data.centre_lon);
    ts = tan(0.5 * (PI_OVER_2 - fwd_proj_data.sign * latFirst));
    if (fwd_proj_data.ind)
        height = radius * fwd_proj_data.mcs * ts / fwd_proj_data.tcs;
    else
        height = 2.0 * radius * ts;
    x0 = fwd_proj_data.sign * height * sin(con1) + fwd_proj_data.false_easting;
    y0 = -fwd_proj_data.sign * height * cos(con1) + fwd_proj_data.false_northing;
    
    x0 = -x0;
    y0 = -y0;

    /* Inverse projection initialisation */
    inv_proj_data.false_easting = x0;
    inv_proj_data.false_northing= y0;
    inv_proj_data.centre_lon = centralLongitude;
    inv_proj_data.centre_lat = centralLatitude;
    if (centralLatitude < 0) inv_proj_data.sign = -1.0;
    else           inv_proj_data.sign = +1.0;
    inv_proj_data.ind = 0;
    if (fabs(fabs(centralLatitude) - PI_OVER_2) > EPSILON) {
        inv_proj_data.ind = 1;
        con1 = inv_proj_data.sign * inv_proj_data.centre_lat;
        inv_proj_data.mcs = cos(con1);
        inv_proj_data.tcs = tan(0.5 * (PI_OVER_2 - con1));
    }
    self->lats = (double*)grib_context_malloc(h->context,iter->nv*sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context,GRIB_LOG_ERROR, "unable to allocate %ld bytes",iter->nv*sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    self->lons = (double*)grib_context_malloc(h->context,iter->nv*sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context,GRIB_LOG_ERROR, "unable to allocate %ld bytes",iter->nv*sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    lats=self->lats;
    lons=self->lons;
    Dx = iScansNegatively == 0 ? Dx : -Dx;
    Dy = jScansPositively == 1 ? Dy : -Dy;

    y = 0;
    for (j=0;j<ny;j++) {
        x = 0;
        for (i=0;i<nx;i++) {
            /* Inverse projection from x,y to lat,lon */
            /* int index =i+j*nx; */
            double _x = (x - inv_proj_data.false_easting) * inv_proj_data.sign;
            double _y = (y - inv_proj_data.false_northing) * inv_proj_data.sign;
            double rh = sqrt(_x * _x + _y * _y);
            if (inv_proj_data.ind)
                ts = rh * inv_proj_data.tcs/(radius * inv_proj_data.mcs);
            else
                ts = rh / (radius * 2.0);
            *lats = inv_proj_data.sign * (PI_OVER_2 - 2 * atan(ts));
            if (rh == 0) {
                *lons = inv_proj_data.sign * inv_proj_data.centre_lon;
            } else {
                double temp = atan2(_x, -_y);
                *lons = inv_proj_data.sign * temp + inv_proj_data.centre_lon;
            }
            *lats = *lats * RAD2DEG;
            *lons = *lons * RAD2DEG;
            while (*lons<0)   *lons += 360;
            while (*lons>360) *lons -= 360;
            lons++;
            lats++;

            x += Dx;
        }
        y += Dy;
    }
#if 0
    /*standardParallel = (southPoleOnPlane == 1) ? -90 : +90;*/
    if (jPointsAreConsecutive)
    {
        x=xFirst;
        for (i=0;i<nx;i++) {
            y=yFirst;
            for (j=0;j<ny;j++) {
                rho=sqrt(x*x+y*y);
                if (rho == 0) {
                    /* indeterminate case */
                    *lats = standardParallel;
                    *lons = centralLongitude;
                }
                else {
                    c=2*atan2(rho,(2.0*radius));
                    cosc=cos(c);
                    sinc=sin(c);
                    *lats = asin( cosc*sinphi1 + y*sinc*cosphi1/rho ) * RAD2DEG;
                    *lons = (lambda0+atan2(x*sinc, rho*cosphi1*cosc - y*sinphi1*sinc)) * RAD2DEG;
                }
                while (*lons<0)   *lons += 360;
                while (*lons>360) *lons -= 360;
                lons++;
                lats++;

                y+=Dy;
            }
            x+=Dx;
        }
    }
    else
    {
        y=yFirst;
        for (j=0;j<ny;j++) {
            x=xFirst;
            for (i=0;i<nx;i++) {
                /* int index =i+j*nx; */
                rho=sqrt(x*x+y*y);
                if (rho == 0) {
                    /* indeterminate case */
                    *lats = standardParallel;
                    *lons = centralLongitude;
                }
                else {
                    c=2*atan2(rho,(2.0*radius));
                    cosc=cos(c);
                    sinc=sin(c);
                    *lats = asin( cosc*sinphi1 + y*sinc*cosphi1/rho ) * RAD2DEG;
                    *lons = (lambda0+atan2(x*sinc, rho*cosphi1*cosc - y*sinphi1*sinc)) * RAD2DEG;
                }
                while (*lons<0)   *lons += 360;
                while (*lons>360) *lons -= 360;
                lons++;
                lats++;

                x+=Dx;
            }
            y+=Dy;
        }
    }
#endif
    iter->e = -1;

    return ret;
}

static int destroy(grib_iterator* i)
{
    grib_iterator_polar_stereographic* self = (grib_iterator_polar_stereographic*)i;
    const grib_context *c = i->h->context;

    grib_context_free(c,self->lats);
    grib_context_free(c,self->lons);
    return 1;
}
