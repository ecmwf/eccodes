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


typedef struct grib_iterator_lambert_azimuthal_equal_area{
  grib_iterator it;
/* Members defined in gen */
	long carg;
	const char* missingValue;
/* Members defined in lambert_azimuthal_equal_area */
	double *lats;
	double *lons;
	long nam;
} grib_iterator_lambert_azimuthal_equal_area;

extern grib_iterator_class* grib_iterator_class_gen;

static grib_iterator_class _grib_iterator_class_lambert_azimuthal_equal_area = {
    &grib_iterator_class_gen,                    /* super                     */
    "lambert_azimuthal_equal_area",                    /* name                      */
    sizeof(grib_iterator_lambert_azimuthal_equal_area),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    &destroy,                  /* destructor                */
    &next,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_lambert_azimuthal_equal_area = &_grib_iterator_class_lambert_azimuthal_equal_area;


static void init_class(grib_iterator_class* c)
{
	c->previous	=	(*(c->super))->previous;
	c->reset	=	(*(c->super))->reset;
	c->has_next	=	(*(c->super))->has_next;
}
/* END_CLASS_IMP */

static int next(grib_iterator* i, double *lat, double *lon, double *val)
{

    grib_iterator_lambert_azimuthal_equal_area* self = (grib_iterator_lambert_azimuthal_equal_area*)i;

    if((long)i->e >= (long)(i->nv-1))
        return 0;
    i->e++;

    *lat = self->lats[i->e];
    *lon = self->lons[i->e];
    *val = i->data[i->e];

    return 1;
}

static int init(grib_iterator* iter,grib_handle* h,grib_arguments* args)
{
    int ret=0;
    double *lats,*lons;
    double lonFirstInDegrees,latFirstInDegrees,lonFirst,latFirst,radius=0;
    long nx,ny,standardParallel,centralLongitude;
    double phi1,lambda0,xFirst,yFirst,x,y,Dx,Dy;
    double kp,sinphi1,cosphi1;
    long alternativeRowScanning,iScansNegatively;
    long jScansPositively,jPointsAreConsecutive;
    double sinphi,cosphi,cosdlambda,sindlambda;
    double cosc,sinc;
    long i,j;

    grib_iterator_lambert_azimuthal_equal_area* self =
            (grib_iterator_lambert_azimuthal_equal_area*)iter;

    const char* sradius           = grib_arguments_get_name(h,args,self->carg++);
    const char* snx               = grib_arguments_get_name(h,args,self->carg++);
    const char* sny               = grib_arguments_get_name(h,args,self->carg++);
    const char* slatFirstInDegrees = grib_arguments_get_name(h,args,self->carg++);
    const char* slonFirstInDegrees = grib_arguments_get_name(h,args,self->carg++);
    const char* sstandardParallel = grib_arguments_get_name(h,args,self->carg++);
    const char* scentralLongitude = grib_arguments_get_name(h,args,self->carg++);
    const char* sDx = grib_arguments_get_name(h,args,self->carg++);
    const char* sDy = grib_arguments_get_name(h,args,self->carg++);
    const char* siScansNegatively = grib_arguments_get_name(h,args,self->carg++);
    const char* sjScansPositively = grib_arguments_get_name(h,args,self->carg++);
    const char* sjPointsAreConsecutive = grib_arguments_get_name(h,args,self->carg++);
    const char* salternativeRowScanning = grib_arguments_get_name(h,args,self->carg++);
    double c,rho;
    double epsilon=1.0e-20;
    double d2r=acos(0.0)/90.0;

    if((ret = grib_get_double_internal(h, sradius,&radius)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, snx,&nx)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sny,&ny)) != GRIB_SUCCESS)
        return ret;

    if (iter->nv!=nx*ny) {
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "Wrong number of points (%ld!=%ldx%ld)",
                iter->nv,nx,ny);
        return GRIB_WRONG_GRID;
    }
    if((ret = grib_get_double_internal(h, slatFirstInDegrees,&latFirstInDegrees))
            != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, slonFirstInDegrees,&lonFirstInDegrees))
            != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sstandardParallel,&standardParallel))
            != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, scentralLongitude,&centralLongitude))
            != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sDx,&Dx)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sDy,&Dy)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h,
            sjPointsAreConsecutive,&jPointsAreConsecutive))
            != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sjScansPositively,&jScansPositively))
            != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, siScansNegatively,&iScansNegatively))
            != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h,
            salternativeRowScanning,&alternativeRowScanning))
            != GRIB_SUCCESS)
        return ret;

    lambda0=d2r*centralLongitude/1000000;
    phi1=d2r*standardParallel/1000000;
    latFirst=latFirstInDegrees*d2r;
    lonFirst=lonFirstInDegrees*d2r;

    cosphi1=cos(phi1);
    sinphi1=sin(phi1);

    Dx = iScansNegatively == 0 ? Dx/1000 : -Dx/1000;
    Dy = jScansPositively == 1 ? Dy/1000 : -Dy/1000;
    self->lats = (double*)grib_context_malloc(h->context,iter->nv*sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "unable to allocate %ld bytes",iter->nv*sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    self->lons = (double*)grib_context_malloc(h->context,iter->nv*sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "unable to allocate %ld bytes",iter->nv*sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    lats=self->lats;
    lons=self->lons;

    /* compute xFirst,yFirst in metres */
    sinphi=sin(latFirst);
    cosphi=cos(latFirst);
    cosdlambda=cos(lonFirst-lambda0);
    sindlambda=sin(lonFirst-lambda0);
    kp=radius*sqrt(2.0/(1+sinphi1*sinphi+cosphi1*cosphi*cosdlambda));
    xFirst=kp*cosphi*sindlambda;
    yFirst=kp*(cosphi1*sinphi-sinphi1*cosphi*cosdlambda);

    if (jPointsAreConsecutive) {

        x=xFirst;
        for (i=0;i<nx;i++) {
            double xsq = x*x;
            y=yFirst;
            for (j=0;j<ny;j++) {
                rho=sqrt(xsq+y*y);
                if (rho>epsilon) {
                    c=2*asin(rho/(2.0*radius));
                    cosc=cos(c);
                    sinc=sin(c);
                    *lats=asin(cosc*sinphi1+y*sinc*cosphi1/rho)/d2r;
                    *lons=(lambda0+atan2(x*sinc,rho*cosphi1*cosc-y*sinphi1*sinc))/d2r;
                } else {
                    *lats=phi1/d2r;
                    *lons=lambda0/d2r;
                }
                if (*lons<0) *lons+=360;
                lons++;
                lats++;

                y+=Dy;
            }
            x+=Dx;
        }

    } else {

        y=yFirst;
        for (j=0;j<ny;j++) {
            double ysq = y*y;
            x=xFirst;
            for (i=0;i<nx;i++) {
                rho=sqrt(x*x+ysq);
                if (rho>epsilon) {
                    c=2*asin(rho/(2.0*radius));
                    cosc=cos(c);
                    sinc=sin(c);
                    *lats=asin(cosc*sinphi1+y*sinc*cosphi1/rho)/d2r;
                    *lons=(lambda0+atan2(x*sinc,rho*cosphi1*cosc-y*sinphi1*sinc))/d2r;
                } else {
                    *lats=phi1/d2r;
                    *lons=lambda0/d2r;
                }
                if (*lons<0) *lons+=360;
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
    grib_iterator_lambert_azimuthal_equal_area* self = (grib_iterator_lambert_azimuthal_equal_area*)i;
    const grib_context *c = i->h->context;

    grib_context_free(c,self->lats);
    grib_context_free(c,self->lons);
    return 1;
}
