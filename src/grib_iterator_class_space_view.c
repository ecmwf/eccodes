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


typedef struct grib_iterator_space_view{
  grib_iterator it;
/* Members defined in gen */
	long carg;
	const char* missingValue;
/* Members defined in space_view */
	double *lats;
	double *lons;
	long nam;
} grib_iterator_space_view;

extern grib_iterator_class* grib_iterator_class_gen;

static grib_iterator_class _grib_iterator_class_space_view = {
    &grib_iterator_class_gen,                    /* super                     */
    "space_view",                    /* name                      */
    sizeof(grib_iterator_space_view),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    &destroy,                  /* destructor                */
    &next,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_space_view = &_grib_iterator_class_space_view;


static void init_class(grib_iterator_class* c)
{
	c->previous	=	(*(c->super))->previous;
	c->reset	=	(*(c->super))->reset;
	c->has_next	=	(*(c->super))->has_next;
}
/* END_CLASS_IMP */

static int next(grib_iterator* i, double *lat, double *lon, double *val)
{
    grib_iterator_space_view* self = (grib_iterator_space_view*)i;

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

#if 0
static int init(grib_iterator* iter,grib_handle* h,grib_arguments* args)
{
    int ret = GRIB_SUCCESS;
    double *lats,*lons;
    double latOfSubSatellitePointInDegrees,lonOfSubSatellitePointInDegrees;
    double orientationInDegrees, nrInRadiusOfEarth;
    double radius=0,xpInGridLengths=0,ypInGridLengths=0;
    long nx, ny, earthIsOblate;
    long alternativeRowScanning,iScansNegatively;
    long Xo, Yo;
    long jScansPositively,jPointsAreConsecutive;
    long i;
    
    double major, minor, r_eq, r_pol, sat_height;
    double lap, lop, orient_angle, angular_size;
    double xp, yp, dx, dy, rx, ry, x, y;
    double cos_x, cos_y, sin_x, sin_y;
    double factor_1, factor_2, tmp1, Sd, Sn, Sxy, S1, S2, S3;
    int x0, y0, ix, iy;
    double *s_x, *c_x;
    double const1, const2, const3;
    
    grib_iterator_space_view* self = (grib_iterator_space_view*)iter;
    
    const char* sradius                 = grib_arguments_get_name(h,args,self->carg++);
    const char* sEarthIsOblate          = grib_arguments_get_name(h,args,self->carg++);
    const char* sMajorAxisInMetres   = grib_arguments_get_name(h,args,self->carg++);
    const char* sMinorAxisInMetres   = grib_arguments_get_name(h,args,self->carg++);
    const char* snx                     = grib_arguments_get_name(h,args,self->carg++);
    const char* sny                     = grib_arguments_get_name(h,args,self->carg++);
    const char* sLatOfSubSatellitePointInDegrees = grib_arguments_get_name(h,args,self->carg++);
    const char* sLonOfSubSatellitePointInDegrees = grib_arguments_get_name(h,args,self->carg++);
    const char* sDx                     = grib_arguments_get_name(h,args,self->carg++);
    const char* sDy                     = grib_arguments_get_name(h,args,self->carg++);
    const char* sXpInGridLengths        = grib_arguments_get_name(h,args,self->carg++);
    const char* sYpInGridLengths        = grib_arguments_get_name(h,args,self->carg++);
    const char* sOrientationInDegrees   = grib_arguments_get_name(h,args,self->carg++);
    const char* sNrInRadiusOfEarth      = grib_arguments_get_name(h,args,self->carg++);
    const char* sXo   = grib_arguments_get_name(h,args,self->carg++);
    const char* sYo   = grib_arguments_get_name(h,args,self->carg++);
    
    const char* siScansNegatively       = grib_arguments_get_name(h,args,self->carg++);
    const char* sjScansPositively       = grib_arguments_get_name(h,args,self->carg++);
    const char* sjPointsAreConsecutive  = grib_arguments_get_name(h,args,self->carg++);
    const char* sAlternativeRowScanning = grib_arguments_get_name(h,args,self->carg++);
    
    if((ret = grib_get_long_internal(h, snx,&nx)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sny,&ny)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sEarthIsOblate, &earthIsOblate)) != GRIB_SUCCESS)
        return ret;
    
    if (earthIsOblate) {
        if((ret = grib_get_double_internal(h, sMajorAxisInMetres, &major)) != GRIB_SUCCESS)
            return ret;
        if((ret = grib_get_double_internal(h, sMinorAxisInMetres, &minor)) != GRIB_SUCCESS)
            return ret;
    } else {
        if((ret = grib_get_double_internal(h, sradius, &radius)) != GRIB_SUCCESS)
            return ret;
    }
    
    if (iter->nv!=nx*ny) {
        grib_context_log(h->context,GRIB_LOG_ERROR, "Wrong number of points (%ld!=%ldx%ld)", iter->nv,nx,ny);
        return GRIB_WRONG_GRID;
    }
    if((ret = grib_get_double_internal(h, sLatOfSubSatellitePointInDegrees,&latOfSubSatellitePointInDegrees)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sLonOfSubSatellitePointInDegrees,&lonOfSubSatellitePointInDegrees)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sDx, &dx)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sDy, &dy)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sXpInGridLengths,&xpInGridLengths)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sYpInGridLengths,&ypInGridLengths)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sOrientationInDegrees,&orientationInDegrees)) != GRIB_SUCCESS)
        return ret;
    
    /* Orthographic not supported. This happens when Nr (camera altitude) is missing */
    if (grib_is_missing(h, sNrInRadiusOfEarth, &ret)) {
        grib_context_log(h->context,GRIB_LOG_ERROR, "Orthographic view (Nr missing) not supported");
        return GRIB_NOT_IMPLEMENTED;
    }
    if((ret = grib_get_double_internal(h, sNrInRadiusOfEarth,&nrInRadiusOfEarth)) != GRIB_SUCCESS)
        return ret;
    
    if((ret = grib_get_long_internal(h, sXo,&Xo)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sYo,&Yo)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sjPointsAreConsecutive,&jPointsAreConsecutive)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sjScansPositively,&jScansPositively)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, siScansNegatively,&iScansNegatively)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sAlternativeRowScanning,&alternativeRowScanning)) != GRIB_SUCCESS)
        return ret;
    
    if (earthIsOblate) {
        r_eq = major;  /* In km */
        r_pol = minor;
    } else {
        r_eq = r_pol = radius * 0.001;/*conv to km*/
    }

    double as = 2 * asin(1.0 / nrInRadiusOfEarth); /* angular_size */
    double cfac = dx / as;
    double lfac = dy / as;
    // use km, so scale by the earth radius
    double scale_factor = (Nr - 1) * majorAxis / 1000; // this sets the units of the projection x,y coords in km
    double scale_x = scale_factor; // LOOK fake neg need scan value
    double scale_y = -scale_factor; // LOOK fake neg need scan value
    
    double startx = scale_factor * (1 - Xp) / cfac;
    double incrx = scale_factor / cfac;
    boolean yscanPositive = GribUtils.scanModeYisPositive(scanMode);
    double starty, incry;
    if (yscanPositive) {
        starty = scale_factor * (Yp - getNy()) / lfac;
        incry = (scale_factor / lfac);
    } else {
        incry = -(scale_factor / lfac);
        starty = scale_factor * (Yp - getNy()) / lfac - incry * (getNy() - 1);
    }
    
    //MSGnavigation proj = new MSGnavigation(LaP, LoP, majorAxis, minorAxis, Nr * majorAxis, scale_x, scale_y);
    sat_height = nrInRadiusOfEarth * r_eq; /* km */
    const1 = major / minor;
    const1 *= const1;
    const2 = 1.0 - (minor * minor) / (major * major);
    const3 = sat_height * sat_height - major * major;
    
    
    
    
    
    
    ///////OLD
    angular_size = 2.0 * asin(1.0 / nrInRadiusOfEarth);
    sat_height = nrInRadiusOfEarth * r_eq;
    
    lap = latOfSubSatellitePointInDegrees;
    lop = lonOfSubSatellitePointInDegrees;
    /* apply default scaling factor */
    lap *= 1e-6;
    lop *= 1e-6;
    lap *= DEG2RAD;
    lop *= DEG2RAD;
    
    orient_angle = orientationInDegrees;
    /* apply default scaling factor */
    if (orient_angle != 0.0) return GRIB_NOT_IMPLEMENTED;
    
    xp = xpInGridLengths;
    yp = ypInGridLengths;
    x0 = Xo;
    y0 = Yo;
    
    rx = angular_size / dx;
    ry = (r_pol/r_eq) * angular_size / dy;
    
    /*dx = iScansNegatively == 0 ? dx : -dx;
     *    dy = jScansPositively == 1 ? dy : -dy;*/
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
    
    if (!iScansNegatively) {
        xp = xp - x0;
    } else {
        xp = (nx-1) - (xp - x0);
    }
    if (jScansPositively) {
        yp = yp - y0;
    }
    else {
        yp = (ny-1) - (yp - y0);
    }
    i = 0;
    factor_2 = (r_eq/r_pol)*(r_eq/r_pol);
    factor_1 = sat_height * sat_height - r_eq * r_eq;
    
    s_x = (double *) grib_context_malloc(h->context, nx*sizeof(double));
    if (!s_x) {
        grib_context_log(h->context,GRIB_LOG_ERROR, "unable to allocate %ld bytes",nx*sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    c_x = (double *) grib_context_malloc(h->context, nx*sizeof(double));
    if (!c_x) {
        grib_context_log(h->context,GRIB_LOG_ERROR, "unable to allocate %ld bytes",nx*sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    
    for (ix = 0; ix < nx; ix++) {
        x = (ix - xp) * rx;
        s_x[ix] = sin(x);
        c_x[ix] = sqrt(1.0 - s_x[ix]*s_x[ix]);
    }
    
    for (iy = 0; iy < ny; iy++) {
        y = (iy - yp) * ry;
        sin_y = sin(y);
        cos_y = sqrt(1.0 - sin_y*sin_y);
        
        tmp1 = (1 + (factor_2-1.0)*sin_y*sin_y);
        
        for (ix = 0; ix < nx; ix++, i++) {
            /* x = (ix - xp) * rx; */
            sin_x = s_x[ix];
            cos_x = c_x[ix];
            
            Sd = sat_height * cos_x * cos_y;
            Sd = Sd * Sd - tmp1*factor_1;
            if (Sd <= 0.0) {    // outside of view
                lats[i] = lons[i] = 0; /* TODO: error? */
            }
            else {
                Sd = sqrt(Sd);
                Sn = (sat_height*cos_x*cos_y - Sd) / tmp1;
                S1 = sat_height - Sn * cos_x * cos_y;
                S2 = Sn * sin_x * cos_y;
                S3 = Sn * sin_y;
                Sxy = sqrt(S1*S1 + S2*S2);
                lons[i] = atan(S2/S1)*(RAD2DEG) + lop;
                lats[i] = atan(factor_2*S3/Sxy)*(RAD2DEG);
                /*printf("lat=%g   lon=%g\n", lats[i], lons[i]);*/
            }
            /*while (lons[i]<0)   lons[i] += 360;
             *            while (lons[i]>360) lons[i] -= 360;*/
        }
    }
    grib_context_free(h->context, s_x);
    grib_context_free(h->context, c_x);
    iter->e = -1;
    
    return ret;
}
#endif
#if 1
static int init(grib_iterator* iter,grib_handle* h,grib_arguments* args)
{
    int ret=0;
    double *lats,*lons;
    double latOfSubSatellitePointInDegrees,lonOfSubSatellitePointInDegrees;
    double orientationInDegrees, nrInRadiusOfEarth;
    double radius=0,xpInGridLengths=0,ypInGridLengths=0;
    long nx, ny, earthIsOblate;
    long alternativeRowScanning,iScansNegatively;
    long Xo, Yo;
    long jScansPositively,jPointsAreConsecutive;
    long i;

    double major, minor, r_eq, r_pol, sat_height;
    double lap, lop, orient_angle, angular_size;
    double xp, yp, dx, dy, rx, ry, x, y;
    double cos_x, cos_y, sin_x, sin_y;
    double factor_1, factor_2, tmp1, Sd, Sn, Sxy, S1, S2, S3;
    int x0, y0, ix, iy;
    double *s_x, *c_x;
    
    grib_iterator_space_view* self = (grib_iterator_space_view*)iter;

    const char* sradius                 = grib_arguments_get_name(h,args,self->carg++);
    const char* sEarthIsOblate          = grib_arguments_get_name(h,args,self->carg++);
    const char* sMajorAxisInMetres   = grib_arguments_get_name(h,args,self->carg++);
    const char* sMinorAxisInMetres   = grib_arguments_get_name(h,args,self->carg++);
    const char* snx                     = grib_arguments_get_name(h,args,self->carg++);
    const char* sny                     = grib_arguments_get_name(h,args,self->carg++);
    const char* sLatOfSubSatellitePointInDegrees = grib_arguments_get_name(h,args,self->carg++);
    const char* sLonOfSubSatellitePointInDegrees = grib_arguments_get_name(h,args,self->carg++);
    const char* sDx                     = grib_arguments_get_name(h,args,self->carg++);
    const char* sDy                     = grib_arguments_get_name(h,args,self->carg++);
    const char* sXpInGridLengths        = grib_arguments_get_name(h,args,self->carg++);
    const char* sYpInGridLengths        = grib_arguments_get_name(h,args,self->carg++);
    const char* sOrientationInDegrees   = grib_arguments_get_name(h,args,self->carg++);
    const char* sNrInRadiusOfEarth      = grib_arguments_get_name(h,args,self->carg++);
    const char* sXo   = grib_arguments_get_name(h,args,self->carg++);
    const char* sYo   = grib_arguments_get_name(h,args,self->carg++);

    const char* siScansNegatively       = grib_arguments_get_name(h,args,self->carg++);
    const char* sjScansPositively       = grib_arguments_get_name(h,args,self->carg++);
    const char* sjPointsAreConsecutive  = grib_arguments_get_name(h,args,self->carg++);
    const char* sAlternativeRowScanning = grib_arguments_get_name(h,args,self->carg++);

    if((ret = grib_get_long_internal(h, snx,&nx)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sny,&ny)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sEarthIsOblate, &earthIsOblate)) != GRIB_SUCCESS)
        return ret;

    if (earthIsOblate) {
        if((ret = grib_get_double_internal(h, sMajorAxisInMetres, &major)) != GRIB_SUCCESS)
            return ret;
        if((ret = grib_get_double_internal(h, sMinorAxisInMetres, &minor)) != GRIB_SUCCESS)
            return ret;
    } else {
        if((ret = grib_get_double_internal(h, sradius, &radius)) != GRIB_SUCCESS)
            return ret;
    }
    
    if (iter->nv!=nx*ny) {
        grib_context_log(h->context,GRIB_LOG_ERROR, "Wrong number of points (%ld!=%ldx%ld)", iter->nv,nx,ny);
        return GRIB_WRONG_GRID;
    }
    if((ret = grib_get_double_internal(h, sLatOfSubSatellitePointInDegrees,&latOfSubSatellitePointInDegrees)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sLonOfSubSatellitePointInDegrees,&lonOfSubSatellitePointInDegrees)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sDx, &dx)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sDy, &dy)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sXpInGridLengths,&xpInGridLengths)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sYpInGridLengths,&ypInGridLengths)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_double_internal(h, sOrientationInDegrees,&orientationInDegrees)) != GRIB_SUCCESS)
        return ret;

    /* Orthographic not supported. This happens when Nr (camera altitude) is missing */
    if (grib_is_missing(h, sNrInRadiusOfEarth, &ret)) {
        grib_context_log(h->context,GRIB_LOG_ERROR, "Orthographic view (Nr missing) not supported");
        return GRIB_NOT_IMPLEMENTED;
    }
    if((ret = grib_get_double_internal(h, sNrInRadiusOfEarth,&nrInRadiusOfEarth)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(h, sXo,&Xo)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sYo,&Yo)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sjPointsAreConsecutive,&jPointsAreConsecutive)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sjScansPositively,&jScansPositively)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, siScansNegatively,&iScansNegatively)) != GRIB_SUCCESS)
        return ret;
    if((ret = grib_get_long_internal(h, sAlternativeRowScanning,&alternativeRowScanning)) != GRIB_SUCCESS)
        return ret;

    if (earthIsOblate) {
        r_eq = major;  /* In km */
        r_pol = minor;
    } else {
        r_eq = r_pol = radius * 0.001;/*conv to km*/
    }
    angular_size = 2.0 * asin(1.0 / nrInRadiusOfEarth);
    sat_height = nrInRadiusOfEarth * r_eq;

    lap = latOfSubSatellitePointInDegrees;
    lop = lonOfSubSatellitePointInDegrees;
    /* apply default scaling factor */
    lap *= 1e-6;
    lop *= 1e-6;
    lap *= DEG2RAD;
    lop *= DEG2RAD;

    orient_angle = orientationInDegrees;
    /* apply default scaling factor */
    if (orient_angle != 0.0) return GRIB_NOT_IMPLEMENTED;

    xp = xpInGridLengths;
    yp = ypInGridLengths;
    x0 = Xo;
    y0 = Yo;

    rx = angular_size / dx;
    ry = (r_pol/r_eq) * angular_size / dy;

    /*dx = iScansNegatively == 0 ? dx : -dx;
    dy = jScansPositively == 1 ? dy : -dy;*/
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

    if (!iScansNegatively) {
        xp = xp - x0;
    } else {
        xp = (nx-1) - (xp - x0);
    }
    if (jScansPositively) {
        yp = yp - y0;
    }
    else {
        yp = (ny-1) - (yp - y0);
    }
    i = 0;
    factor_2 = (r_eq/r_pol)*(r_eq/r_pol);
    factor_1 = sat_height * sat_height - r_eq * r_eq;

    s_x = (double *) grib_context_malloc(h->context, nx*sizeof(double));
    if (!s_x) {
        grib_context_log(h->context,GRIB_LOG_ERROR, "unable to allocate %ld bytes",nx*sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    c_x = (double *) grib_context_malloc(h->context, nx*sizeof(double));
    if (!c_x) {
        grib_context_log(h->context,GRIB_LOG_ERROR, "unable to allocate %ld bytes",nx*sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }

    for (ix = 0; ix < nx; ix++) {
        x = (ix - xp) * rx;
        s_x[ix] = sin(x);
        c_x[ix] = sqrt(1.0 - s_x[ix]*s_x[ix]);
    }

    for (iy = 0; iy < ny; iy++) {
        y = (iy - yp) * ry;
        sin_y = sin(y);
        cos_y = sqrt(1.0 - sin_y*sin_y);

        tmp1 = (1 + (factor_2-1.0)*sin_y*sin_y);
        
        for (ix = 0; ix < nx; ix++, i++) {
            /* x = (ix - xp) * rx; */
            sin_x = s_x[ix];
            cos_x = c_x[ix];
            
            Sd = sat_height * cos_x * cos_y;
            Sd = Sd * Sd - tmp1*factor_1;
            if (Sd <= 0.0) {    // outside of view
                lats[i] = lons[i] = 0; /* TODO: error? */
            }
            else {
                Sd = sqrt(Sd);
                Sn = (sat_height*cos_x*cos_y - Sd) / tmp1;
                S1 = sat_height - Sn * cos_x * cos_y;
                S2 = Sn * sin_x * cos_y;
                S3 = Sn * sin_y;
                Sxy = sqrt(S1*S1 + S2*S2);
                lons[i] = atan(S2/S1)*(RAD2DEG) + lop;
                lats[i] = atan(factor_2*S3/Sxy)*(RAD2DEG);
                /*printf("lat=%g   lon=%g\n", lats[i], lons[i]);*/
            }
            /*while (lons[i]<0)   lons[i] += 360;
            while (lons[i]>360) lons[i] -= 360;*/
        }
    }
    grib_context_free(h->context, s_x);
    grib_context_free(h->context, c_x);
    iter->e = -1;

    return ret;
}
#endif

static int destroy(grib_iterator* i)
{
    grib_iterator_space_view* self = (grib_iterator_space_view*)i;
    const grib_context *c = i->h->context;

    grib_context_free(c,self->lats);
    grib_context_free(c,self->lons);
    return 1;
}
