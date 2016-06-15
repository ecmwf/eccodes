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


typedef struct grib_iterator_lambert_conformal{
  grib_iterator it;
/* Members defined in gen */
	long carg;
	const char* missingValue;
/* Members defined in lambert_conformal */
	double *lats;
	double *lons;
	long nam;
} grib_iterator_lambert_conformal;

extern grib_iterator_class* grib_iterator_class_gen;

static grib_iterator_class _grib_iterator_class_lambert_conformal = {
    &grib_iterator_class_gen,                    /* super                     */
    "lambert_conformal",                    /* name                      */
    sizeof(grib_iterator_lambert_conformal),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    &destroy,                  /* destructor                */
    &next,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_lambert_conformal = &_grib_iterator_class_lambert_conformal;


static void init_class(grib_iterator_class* c)
{
	c->previous	=	(*(c->super))->previous;
	c->reset	=	(*(c->super))->reset;
	c->has_next	=	(*(c->super))->has_next;
}
/* END_CLASS_IMP */

static int next(grib_iterator* i, double *lat, double *lon, double *val)
{
	grib_iterator_lambert_conformal* self = (grib_iterator_lambert_conformal*)i;

	if((long)i->e >= (long)(i->nv-1))
		return 0;
	i->e++;

	*lat = self->lats[i->e];
	*lon = self->lons[i->e];
	*val = i->data[i->e];

	return 1;
}

#ifndef M_PI
#define M_PI      3.14159265358979323846   /* Whole pie */
#endif

#ifndef M_PI_2
#define M_PI_2    1.57079632679489661923   /* Half a pie */
#endif

#ifndef M_PI_4
#define M_PI_4    0.78539816339744830962   /* Quarter of a pie */
#endif

#define RAD2DEG   57.29577951308232087684  /* 180 over pi */
#define DEG2RAD   0.01745329251994329576   /* pi over 180 */

static int init(grib_iterator* iter,grib_handle* h,grib_arguments* args)
{
    int i, j, ret=0;
    double *lats, *lons; /* the lat/lon arrays to be populated */
    long nx,ny,iScansNegatively,jScansPositively,jPointsAreConsecutive,alternativeRowScanning;
    double LoVInDegrees,LaDInDegrees,Latin1InDegrees,Latin2InDegrees,latFirstInDegrees,
    lonFirstInDegrees, Dx, Dy, radius=0;
    double latFirstInRadians, lonFirstInRadians, LoVInRadians, Latin1InRadians, Latin2InRadians,
    LaDInRadians, lonDiff, lonDeg, latDeg;
    double f, n, rho, rho0, angle, x0, y0, x, y, tmp, tmp2;

    grib_iterator_lambert_conformal* self = (grib_iterator_lambert_conformal*)iter;

    const char* sradius                  = grib_arguments_get_name(h,args,self->carg++);
    const char* snx                      = grib_arguments_get_name(h,args,self->carg++);
    const char* sny                      = grib_arguments_get_name(h,args,self->carg++);
    const char* sLoVInDegrees            = grib_arguments_get_name(h,args,self->carg++);
    const char* sLaDInDegrees            = grib_arguments_get_name(h,args,self->carg++);
    const char* sLatin1InDegrees         = grib_arguments_get_name(h,args,self->carg++);
    const char* sLatin2InDegrees         = grib_arguments_get_name(h,args,self->carg++);
    const char* slatFirstInDegrees       = grib_arguments_get_name(h,args,self->carg++);
    const char* slonFirstInDegrees       = grib_arguments_get_name(h,args,self->carg++);
    /* Dx and Dy are in Metres */
    const char* sDx                      = grib_arguments_get_name(h,args,self->carg++);
    const char* sDy                      = grib_arguments_get_name(h,args,self->carg++);
    const char* siScansNegatively        = grib_arguments_get_name(h,args,self->carg++);
    const char* sjScansPositively        = grib_arguments_get_name(h,args,self->carg++);
    const char* sjPointsAreConsecutive   = grib_arguments_get_name(h,args,self->carg++);
    const char* salternativeRowScanning  = grib_arguments_get_name(h,args,self->carg++);

    if((ret = grib_get_long_internal(h, snx,&nx)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sny,&ny)) != GRIB_SUCCESS)
        return ret;

    if (iter->nv!=nx*ny) {
        grib_context_log(h->context,GRIB_LOG_ERROR,"Wrong number of points (%ld!=%ldx%ld)",iter->nv,nx,ny);
        return GRIB_WRONG_GRID;
    }
    if((ret = grib_get_double_internal(h, sradius,&radius)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sLoVInDegrees, &LoVInDegrees))!=GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sLaDInDegrees, &LaDInDegrees))!=GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sLatin1InDegrees, &Latin1InDegrees))!=GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sLatin2InDegrees, &Latin2InDegrees))!=GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, slatFirstInDegrees,&latFirstInDegrees)) !=GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, slonFirstInDegrees,&lonFirstInDegrees)) !=GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sDx,&Dx)) !=GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sDy,&Dy)) !=GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sjPointsAreConsecutive,&jPointsAreConsecutive)) !=GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sjScansPositively,&jScansPositively)) !=GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, siScansNegatively,&iScansNegatively)) !=GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, salternativeRowScanning,&alternativeRowScanning)) !=GRIB_SUCCESS)
        return ret;

    /* See Wolfram MathWorld: http://mathworld.wolfram.com/LambertConformalConicProjection.html */
    latFirstInRadians = latFirstInDegrees * DEG2RAD;
    lonFirstInRadians = lonFirstInDegrees * DEG2RAD;
    Latin1InRadians   = Latin1InDegrees * DEG2RAD;
    Latin2InRadians   = Latin2InDegrees * DEG2RAD;
    LaDInRadians      = LaDInDegrees * DEG2RAD;
    LoVInRadians      = LoVInDegrees * DEG2RAD;

    if ( fabs(Latin1InRadians - Latin2InRadians) < 1E-09 ) {
        n = sin(Latin1InRadians);
    }
    else {
        n = log(cos(Latin1InRadians)/cos(Latin2InRadians)) /
                log(tan(M_PI_4 + Latin2InRadians/2.0) / tan(M_PI_4 + Latin1InRadians/2.0));
    }

    f = (cos(Latin1InRadians) * pow(tan(M_PI_4 + Latin1InRadians/2.0), n)) / n;
    rho = radius * f * pow(tan(M_PI_4 + latFirstInRadians/2.0), -n);
    rho0 = radius * f * pow(tan(M_PI_4 + LaDInRadians/2.0), -n);
    if ( n < 0 ) /* adjustment for southern hemisphere */
        rho0 = -rho0;
    lonDiff = lonFirstInRadians - LoVInRadians;

    /* Adjust longitude to range -180 to 180 */
    if (lonDiff > M_PI)  lonDiff -= 2*M_PI;
    if (lonDiff < -M_PI) lonDiff += 2*M_PI;
    angle = n * lonDiff;
    x0 = rho * sin(angle);
    y0 = rho0 - rho * cos(angle);
    Dx = iScansNegatively == 0 ? Dx : -Dx;
    /* GRIB-405: Don't change sign of Dy. Latitudes ALWAYS increase from latitudeOfFirstGridPoint */
    /*Dy = jScansPositively == 1 ? Dy : -Dy;*/

    /* No support (yet) for jPointsAreConsecutive */
    if (jPointsAreConsecutive) {
        grib_context_log(h->context,GRIB_LOG_ERROR,"No support for: 'Adjacent points in j (y) direction being consecutive'");
        Assert(0);
    }

    /* Allocate latitude and longitude arrays */
    self->lats = (double*)grib_context_malloc(h->context,iter->nv*sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context,GRIB_LOG_ERROR,	"unable to allocate %ld bytes",iter->nv*sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    self->lons = (double*)grib_context_malloc(h->context,iter->nv*sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context,GRIB_LOG_ERROR,	"unable to allocate %ld bytes",iter->nv*sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    lats=self->lats;
    lons=self->lons;

    /* Populate our arrays */
    for (j = 0; j < ny; j++) {
        y = y0 + j*Dy;
        if ( n < 0 ) {  /* adjustment for southern hemisphere */
            y = -y;
        }
        tmp = rho0 - y;
        tmp2 = tmp*tmp;
        for (i = 0; i < nx; i++) {
            int index =i+j*nx;
            x = x0 + i*Dx;
            if ( n < 0 ) {  /* adjustment for southern hemisphere */
                x = -x;
            }

            angle = atan(x / tmp);
            rho = sqrt(x*x + tmp2);
            if (n <= 0) rho = -rho;
            lonDeg = LoVInDegrees + (angle/n) * RAD2DEG;
            latDeg = (2.0 * atan(pow(radius * f/rho, 1.0/n)) - M_PI_2) * RAD2DEG;
            while ( lonDeg >= 360.0) lonDeg -= 360.0;
            while ( lonDeg < 0.0)    lonDeg += 360.0;
            lons[index] = lonDeg;
            lats[index] = latDeg;
        }
    }

    iter->e = -1;

    return ret;
}

static int destroy(grib_iterator* i)
{
    grib_iterator_lambert_conformal* self = (grib_iterator_lambert_conformal*)i;
    const grib_context *c = i->h->context;

    grib_context_free(c,self->lats);
    grib_context_free(c,self->lons);
    return 1;
}
