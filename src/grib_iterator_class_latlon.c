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
   CLASS      = iterator
   SUPER      = grib_iterator_class_regular
   IMPLEMENTS = init;next
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


typedef struct grib_iterator_latlon{
  grib_iterator it;
/* Members defined in gen */
	long carg;
	const char* missingValue;
/* Members defined in regular */
	double   *las;
	double   *los;
	long      nap;
	long      nam;
	long iScansNegatively;
	long isRotated;
	double angleOfRotation;
	double southPoleLat;
	double southPoleLon;
	long jPointsAreConsecutive;
/* Members defined in latlon */
} grib_iterator_latlon;

extern grib_iterator_class* grib_iterator_class_regular;

static grib_iterator_class _grib_iterator_class_latlon = {
    &grib_iterator_class_regular,                    /* super                     */
    "latlon",                    /* name                      */
    sizeof(grib_iterator_latlon),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    0,                  /* destructor                */
    &next,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_latlon = &_grib_iterator_class_latlon;


static void init_class(grib_iterator_class* c)
{
	c->previous	=	(*(c->super))->previous;
	c->reset	=	(*(c->super))->reset;
	c->has_next	=	(*(c->super))->has_next;
}
/* END_CLASS_IMP */

#define RAD2DEG   57.29577951308232087684  /* 180 over pi */
#define DEG2RAD    0.01745329251994329576  /* pi over 180 */

void unrotate(grib_handle* h,
        const double inlat, const double inlon,
        const double angleOfRot, const double southPoleLat, const double southPoleLon,
        double* outlat, double* outlon)
{
    /* Algorithm taken from ecKit */
    const double lon_x = inlon;
    const double lat_y = inlat;
    /* First convert the data point from spherical lat lon to (x',y',z') */
    double latr = lat_y * DEG2RAD;
    double lonr = lon_x * DEG2RAD;
    double xd = cos(lonr)*cos(latr);
    double yd = sin(lonr)*cos(latr);
    double zd = sin(latr);

    double t = -(90.0 + southPoleLat);
    double o = -southPoleLon;

    double sin_t = sin(DEG2RAD * t);
    double cos_t = cos(DEG2RAD * t);
    double sin_o = sin(DEG2RAD * o);
    double cos_o = cos(DEG2RAD * o);

    double x = cos_t*cos_o*xd + sin_o*yd + sin_t*cos_o*zd;
    double y = -cos_t*sin_o*xd + cos_o*yd - sin_t*sin_o*zd;
    double z = -sin_t*xd + cos_t*zd;

    double ret_lat=0, ret_lon=0;

    /* Then convert back to 'normal' (lat,lon)
     * Uses arcsin, to convert back to degrees, put in range -1 to 1 in case of slight rounding error
     * avoid error on calculating e.g. asin(1.00000001) */
    if (z > 1.0)  z = 1.0;
    if (z < -1.0) z = -1.0;

    ret_lat = asin(z) * RAD2DEG;
    ret_lon = atan2(y, x) * RAD2DEG;

    /* Still get a very small rounding error, round to 6 decimal places */
    /* TODO: roundf implementation missing on MSVC! */
#ifndef ECCODES_ON_WINDOWS
    ret_lat = roundf( ret_lat * 1000000.0 )/1000000.0;
    ret_lon = roundf( ret_lon * 1000000.0 )/1000000.0;
#endif

    ret_lon -= angleOfRot;

    /* Make sure ret_lon is in range*/
    /*
    while (ret_lon < lonmin_) ret_lon += 360.0;
    while (ret_lon >= lonmax_) ret_lon -= 360.0;
     */
    *outlat = ret_lat;
    *outlon = ret_lon;
}

static int next(grib_iterator* iter, double *lat, double *lon, double *val)
{
    /* GRIB-238: Support rotated lat/lon grids */
    double ret_lat, ret_lon, ret_val;
    grib_iterator_latlon* self = (grib_iterator_latlon*)iter;

    if((long)iter->e >= (long)(iter->nv-1))  return 0;

    iter->e++;

    /* Assumptions:
     *   All rows scan in the same direction (alternativeRowScanning==0)
     */
    if (!self->jPointsAreConsecutive) {
        /* Adjacent points in i (x) direction are consecutive */
        ret_lat = self->las[(long)floor(iter->e/self->nap)];
        ret_lon = self->los[(long)iter->e%self->nap];
        ret_val = iter->data[iter->e];
    } else {
        /* Adjacent points in j (y) direction is consecutive */
        ret_lon = self->los[(long)iter->e/self->nam];
        ret_lat = self->las[(long)floor(iter->e%self->nam)];
        ret_val = iter->data[iter->e];
    }

    if (self->isRotated)
    {
        double new_lat = 0, new_lon = 0;
        unrotate(iter->h, ret_lat, ret_lon,
                self->angleOfRotation, self->southPoleLat, self->southPoleLon,
                &new_lat, &new_lon);
        ret_lat = new_lat;
        ret_lon = new_lon;
    }

    *lat = ret_lat;
    *lon = ret_lon;
    *val = ret_val;
    return 1;
}

static int init(grib_iterator* iter, grib_handle* h,grib_arguments* args)
{
    grib_iterator_latlon* self = (grib_iterator_latlon*)iter;
    int err = 0;
    double jdir;
    double lat1;
    long jScansPositively;
    long lai;

    const char* s_lat1      = grib_arguments_get_name(h,args,self->carg++);
    const char* s_jdir      = grib_arguments_get_name(h,args,self->carg++);
    const char* s_jScansPos = grib_arguments_get_name(h,args,self->carg++);
    const char* s_jPtsConsec = grib_arguments_get_name(h,args,self->carg++);
    self->angleOfRotation = 0;
    self->isRotated = 0;
    self->southPoleLat = 0;
    self->southPoleLon = 0;

    if ((err = grib_get_long(h, "is_rotated_grid", &self->isRotated))) return err;
    if (self->isRotated) {
        if ((err = grib_get_double_internal(h,"angleOfRotation",                  &self->angleOfRotation))) return err;
        if ((err = grib_get_double_internal(h,"latitudeOfSouthernPoleInDegrees",  &self->southPoleLat))) return err;
        if ((err = grib_get_double_internal(h,"longitudeOfSouthernPoleInDegrees", &self->southPoleLon))) return err;
    }

    if((err = grib_get_double_internal(h, s_lat1,  &lat1))) return err;
    if((err = grib_get_double_internal(h, s_jdir,  &jdir))) return err;
    if((err = grib_get_long_internal(h, s_jScansPos,  &jScansPositively))) return err;
    if((err = grib_get_long_internal(h, s_jPtsConsec, &self->jPointsAreConsecutive))) return err;

    if (jScansPositively) jdir=-jdir;

    for( lai = 0; lai <  self->nam; lai++ )  {
        self->las[lai] = lat1;
        lat1 -= jdir ;
    }

    iter->e = -1;
    return err;
}
