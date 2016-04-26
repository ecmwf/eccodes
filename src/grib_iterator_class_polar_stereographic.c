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

#define RAD2DEG   57.29577951308232087684  /* 180 over pi */
#define DEG2RAD   0.01745329251994329576   /* pi over 180 */

static int init(grib_iterator* iter,grib_handle* h,grib_arguments* args)
{
    int ret=0;
    double *lats,*lons;
    double lonFirstInDegrees,latFirstInDegrees,lonFirst,latFirst,radius=0;
    long nx,ny,standardParallel,centralLongitude;
    double lambda0,xFirst,yFirst,x,y,Dx,Dy;
    double k,sinphi1,cosphi1;
    long alternativeRowScanning,iScansNegatively;
    long jScansPositively,jPointsAreConsecutive, southPoleOnPlane;
    double sinphi,cosphi,cosdlambda,sindlambda;
    double cosc,sinc;
    long i,j;

    grib_iterator_polar_stereographic* self = (grib_iterator_polar_stereographic*)iter;

    const char* sradius                 = grib_arguments_get_name(h,args,self->carg++);
    const char* snx                     = grib_arguments_get_name(h,args,self->carg++);
    const char* sny                     = grib_arguments_get_name(h,args,self->carg++);
    const char* slatFirstInDegrees      = grib_arguments_get_name(h,args,self->carg++);
    const char* slonFirstInDegrees      = grib_arguments_get_name(h,args,self->carg++);
    const char* ssouthPoleOnPlane       = grib_arguments_get_name(h,args,self->carg++);
    const char* scentralLongitude       = grib_arguments_get_name(h,args,self->carg++);
    const char* sDx                     = grib_arguments_get_name(h,args,self->carg++);
    const char* sDy                     = grib_arguments_get_name(h,args,self->carg++);
    const char* siScansNegatively       = grib_arguments_get_name(h,args,self->carg++);
    const char* sjScansPositively       = grib_arguments_get_name(h,args,self->carg++);
    const char* sjPointsAreConsecutive  = grib_arguments_get_name(h,args,self->carg++);
    const char* salternativeRowScanning = grib_arguments_get_name(h,args,self->carg++);
    double c,rho;
    sinphi1 = cosphi1 = 0.0;

    if((ret = grib_get_double_internal(h, sradius,&radius)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, snx,&nx)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sny,&ny)) != GRIB_SUCCESS)
        return ret;

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
    if((ret = grib_get_long_internal(h, scentralLongitude,&centralLongitude)) != GRIB_SUCCESS)
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

    standardParallel = (southPoleOnPlane == 1) ? -90 : +90;
    sinphi1 = sin(standardParallel*DEG2RAD);
    cosphi1 = cos(standardParallel*DEG2RAD);
    lambda0 = centralLongitude*DEG2RAD;
    latFirst= latFirstInDegrees*DEG2RAD;
    lonFirst= lonFirstInDegrees*DEG2RAD;

    Dx = iScansNegatively == 0 ? Dx : -Dx;
    Dy = jScansPositively == 1 ? Dy : -Dy;
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

    /* compute xFirst,yFirst in metres */
    sinphi=sin(latFirst);
    cosphi=cos(latFirst);
    cosdlambda=cos(lonFirst-lambda0);
    sindlambda=sin(lonFirst-lambda0);

    k = 2.0 * radius / ( 1 + sinphi1*sinphi + cosphi1*cosphi*cosdlambda );
    xFirst = k * cosphi * sindlambda;
    yFirst = k * (cosphi1*sinphi - sinphi1*cosphi*cosdlambda);

    /*kp=radius*2.0*tan(pi4-phi/2);
    xFirst=kp*cosphi*sindlambda;
    yFirst=-kp*cosphi*cosdlambda;*/

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
