/*
 * (C) Copyright 2005- ECMWF.
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
   MEMBERS     =   long Nj
   END_CLASS_DEF
*/

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "iterator.class" and rerun ./make_class.pl

*/


static void init_class(grib_iterator_class*);

static int init(grib_iterator* i, grib_handle*, grib_arguments*);
static int next(grib_iterator* i, double* lat, double* lon, double* val);
static int destroy(grib_iterator* i);


typedef struct grib_iterator_lambert_conformal
{
    grib_iterator it;
    /* Members defined in gen */
    long carg;
    const char* missingValue;
    /* Members defined in lambert_conformal */
    double* lats;
    double* lons;
    long Nj;
} grib_iterator_lambert_conformal;

extern grib_iterator_class* grib_iterator_class_gen;

static grib_iterator_class _grib_iterator_class_lambert_conformal = {
    &grib_iterator_class_gen,                /* super                     */
    "lambert_conformal",                     /* name                      */
    sizeof(grib_iterator_lambert_conformal), /* size of instance          */
    0,                                       /* inited */
    &init_class,                             /* init_class */
    &init,                                   /* constructor               */
    &destroy,                                /* destructor                */
    &next,                                   /* Next Value                */
    0,                                       /*  Previous Value           */
    0,                                       /* Reset the counter         */
    0,                                       /* has next values           */
};

grib_iterator_class* grib_iterator_class_lambert_conformal = &_grib_iterator_class_lambert_conformal;


static void init_class(grib_iterator_class* c)
{
    c->previous = (*(c->super))->previous;
    c->reset    = (*(c->super))->reset;
    c->has_next = (*(c->super))->has_next;
}
/* END_CLASS_IMP */

static int next(grib_iterator* i, double* lat, double* lon, double* val)
{
    grib_iterator_lambert_conformal* self = (grib_iterator_lambert_conformal*)i;

    if ((long)i->e >= (long)(i->nv - 1))
        return 0;
    i->e++;

    *lat = self->lats[i->e];
    *lon = self->lons[i->e];
    *val = i->data[i->e];

    return 1;
}

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* Whole pie */
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923 /* Half a pie */
#endif

#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962 /* Quarter of a pie */
#endif

#define RAD2DEG 57.29577951308232087684 /* 180 over pi */
#define DEG2RAD 0.01745329251994329576  /* pi over 180 */

/* Adjust longitude to range -180 to 180 */
static double adjust_lon(double x)
{
    if (x > M_PI)  x -= 2 * M_PI;
    if (x < -M_PI) x += 2 * M_PI;
    return x;
}

/* Function to compute the latitude angle, phi2, for the inverse */
double phi2z(
    double eccent, /* Spheroid eccentricity */
    double ts,     /* Constant value t */
    int* error)
{
    double eccnth, phi, con, dphi, sinpi;
    int i, MAX_ITER = 15;

    eccnth = 0.5 * eccent;
    phi = M_PI_2 - 2 * atan(ts);
    for (i = 0; i <= MAX_ITER; i++) {
        sinpi = sin(phi);
        con = eccent * sinpi;
        dphi = M_PI_2 - 2 * atan(ts *(pow(((1.0 - con)/(1.0 + con)),eccnth))) - phi;
        phi += dphi;
        if (fabs(dphi) <= .0000000001)
            return(phi);
    }
    /*Assert(!"Convergence error");*/
    *error = GRIB_INTERNAL_ERROR;
    return 0;
}

/* Compute the constant small m which is the radius of
   a parallel of latitude, phi, divided by the semimajor axis */
double msfnz(double eccent, double sinphi, double cosphi)
{
    const double con = eccent * sinphi;
    return ((cosphi / (sqrt(1.0 - con * con))));
}

/* Compute the constant small t for use in the forward
   computations */
double tsfnz(
    double eccent, /* Eccentricity of the spheroid */
    double phi,    /* Latitude phi */
    double sinphi) /* Sine of the latitude */
{
    double con = eccent * sinphi;
    double com = 0.5 * eccent;
    con        = pow(((1.0 - con) / (1.0 + con)), com);
    return (tan(0.5 * (M_PI_2 - phi)) / con);
}

static double calculate_eccentricity(double minor, double major)
{
    const double temp = minor / major;
    return sqrt(1.0 - temp * temp);
}

static int init_sphere(grib_handle* h,
                       grib_iterator_lambert_conformal* self,
                       size_t nv, long nx, long ny,
                       double LoVInDegrees,
                       double Dx, double Dy, double radius,
                       double latFirstInRadians, double lonFirstInRadians,
                       double LoVInRadians, double Latin1InRadians, double Latin2InRadians,
                       double LaDInRadians,
                       long iScansNegatively, long jScansPositively, long jPointsAreConsecutive)
{
    int i, j;
    double f, n, rho, rho0, angle, x0, y0, x, y, tmp, tmp2;
    double latDeg, lonDeg, lonDiff;

    if (fabs(Latin1InRadians - Latin2InRadians) < 1E-09) {
        n = sin(Latin1InRadians);
    }
    else {
        n = log(cos(Latin1InRadians) / cos(Latin2InRadians)) /
            log(tan(M_PI_4 + Latin2InRadians / 2.0) / tan(M_PI_4 + Latin1InRadians / 2.0));
    }

    f    = (cos(Latin1InRadians) * pow(tan(M_PI_4 + Latin1InRadians / 2.0), n)) / n;
    rho  = radius * f * pow(tan(M_PI_4 + latFirstInRadians / 2.0), -n);
    rho0 = radius * f * pow(tan(M_PI_4 + LaDInRadians / 2.0), -n);
    if (n < 0) /* adjustment for southern hemisphere */
        rho0 = -rho0;
    lonDiff = lonFirstInRadians - LoVInRadians;

    /* Adjust longitude to range -180 to 180 */
    if (lonDiff > M_PI)
        lonDiff -= 2 * M_PI;
    if (lonDiff < -M_PI)
        lonDiff += 2 * M_PI;
    angle = n * lonDiff;
    x0    = rho * sin(angle);
    y0    = rho0 - rho * cos(angle);
    /*Dx = iScansNegatively == 0 ? Dx : -Dx;*/
    /* GRIB-405: Don't change sign of Dy. Latitudes ALWAYS increase from latitudeOfFirstGridPoint */
    /*Dy = jScansPositively == 1 ? Dy : -Dy;*/

    /* Allocate latitude and longitude arrays */
    self->lats = (double*)grib_context_malloc(h->context, nv * sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Unable to allocate %ld bytes", nv * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    self->lons = (double*)grib_context_malloc(h->context, nv * sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Unable to allocate %ld bytes", nv * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }

    /* Populate our arrays */
    for (j = 0; j < ny; j++) {
        y = y0 + j * Dy;
        if (n < 0) { /* adjustment for southern hemisphere */
            y = -y;
        }
        tmp  = rho0 - y;
        tmp2 = tmp * tmp;
        for (i = 0; i < nx; i++) {
            int index = i + j * nx;
            x         = x0 + i * Dx;
            if (n < 0) { /* adjustment for southern hemisphere */
                x = -x;
            }

            angle = atan2(x, tmp); /* See ECC-524 */
            rho   = sqrt(x * x + tmp2);
            if (n <= 0)
                rho = -rho;
            lonDeg = LoVInDegrees + (angle / n) * RAD2DEG;
            latDeg = (2.0 * atan(pow(radius * f / rho, 1.0 / n)) - M_PI_2) * RAD2DEG;
            while (lonDeg >= 360.0)
                lonDeg -= 360.0;
            while (lonDeg < 0.0)
                lonDeg += 360.0;
            self->lons[index] = lonDeg;
            self->lats[index] = latDeg;
            /*printf("DBK: llat[%d] = %g \t llon[%d] = %g\n", index,lats[index], index,lons[index]);*/
        }
    }

    return GRIB_SUCCESS;
}

#define EPSILON 1.0e-10

/* Oblate spheroid */
static int init_oblate(grib_handle* h,
                       grib_iterator_lambert_conformal* self,
                       size_t nv, long nx, long ny,
                       double LoVInDegrees,
                       double Dx, double Dy,
                       double earthMinorAxisInMetres, double earthMajorAxisInMetres,
                       double latFirstInRadians, double lonFirstInRadians,
                       double LoVInRadians, double Latin1InRadians, double Latin2InRadians,
                       double LaDInRadians)
{
    int i, j, err = 0;
    double x0, y0, x, y, latDeg, lonDeg, sinphi, ts, rh1, theta;

    double ns;     /* ratio of angle between meridian */
    double f0;     /* flattening of ellipsoid */
    double rh;     /* height above ellipsoid  */
    double sin_po; /* sin value */
    double cos_po; /* cos value */
    double con;    /* temporary variable */
    double ms1;    /* small m 1 */
    double ms2;    /* small m 2 */
    double ts0;    /* small t 0 */
    double ts1;    /* small t 1 */
    double ts2;    /* small t 2 */

    double e = calculate_eccentricity(earthMinorAxisInMetres, earthMajorAxisInMetres);

    sin_po = sin(Latin1InRadians);
    cos_po = cos(Latin1InRadians);
    con    = sin_po;
    ms1    = msfnz(e, sin_po, cos_po);
    ts1    = tsfnz(e, Latin2InRadians, sin_po);
    sin_po = sin(Latin2InRadians);
    cos_po = cos(Latin2InRadians);
    ms2    = msfnz(e, sin_po, cos_po);
    ts2    = tsfnz(e, Latin2InRadians, sin_po);
    sin_po = sin(LaDInRadians);
    ts0    = tsfnz(e, LaDInRadians, sin_po);

    if (fabs(Latin1InRadians - Latin2InRadians) > EPSILON) {
        ns = log(ms1 / ms2) / log(ts1 / ts2);
    } else {
        ns = con;
    }
    f0 = ms1 / (ns * pow(ts1, ns));
    rh = earthMajorAxisInMetres * f0 * pow(ts0, ns);

    /* lat,lon to x,y */
    con = fabs(fabs(latFirstInRadians) - M_PI_2);
    if (con > EPSILON) {
        sinphi = sin(latFirstInRadians);
        ts     = tsfnz(e, latFirstInRadians, sinphi);
        rh1    = earthMajorAxisInMetres * f0 * pow(ts, ns);
    } else {
        con = latFirstInRadians * ns;
        if (con <= 0) {
            grib_context_log(h->context, GRIB_LOG_ERROR, "Point cannot be projected");
            return GRIB_GEOCALCULUS_PROBLEM;
        }
        rh1 = 0;
    }
    theta = ns * adjust_lon(lonFirstInRadians - LoVInRadians);
    x0    = rh1 * sin(theta);
    y0    = rh - rh1 * cos(theta);

    /* Allocate latitude and longitude arrays */
    self->lats = (double*)grib_context_malloc(h->context, nv * sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Unable to allocate %ld bytes", nv * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    self->lons = (double*)grib_context_malloc(h->context, nv * sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Unable to allocate %ld bytes", nv * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }

    /* Populate our arrays */
    for (j = 0; j < ny; j++) {
        y = y0 + j * Dy;
        for (i = 0; i < nx; i++) {
            const int index = i + j * nx;
            x = x0 + i * Dx;
            /* from x,y to lat,lon */
            y = rh - y;
            if (ns > 0) {
                rh1 = sqrt(x * x + y * y);
                con = 1.0;
            } else {
                rh1 = -sqrt(x * x + y * y);
                con = -1.0;
            }
            theta = 0.0;
            if (rh1 != 0)
                theta = atan2((con * x), (con * y));
            if ((rh1 != 0) || (ns > 0.0)) {
                con  = 1.0 / ns;
                ts   = pow((rh1 / (earthMajorAxisInMetres * f0)), con);
                latDeg = phi2z(e, ts, &err);
                if (err) {
                    grib_context_log(h->context, GRIB_LOG_ERROR, "Failed to compute the latitude angle, phi2, for the inverse");
                    return err;
                }
            } else {
                latDeg = -M_PI_2;
            }
            lonDeg = adjust_lon(theta / ns + LoVInRadians);
            while (lonDeg >= 360.0) lonDeg -= 360.0;
            while (lonDeg < 0.0)    lonDeg += 360.0;
            self->lons[index] = lonDeg;
            self->lats[index] = latDeg;
            /*printf("DBK: llat[%d] = %g \t llon[%d] = %g\n", index,lats[index], index,lons[index]);*/
        }
    }
    return GRIB_SUCCESS;
}

static int init(grib_iterator* iter, grib_handle* h, grib_arguments* args)
{
    int err = 0, is_oblate = 0;
    long nx, ny, iScansNegatively, jScansPositively, jPointsAreConsecutive, alternativeRowScanning;
    double LoVInDegrees, LaDInDegrees, Latin1InDegrees, Latin2InDegrees, latFirstInDegrees,
        lonFirstInDegrees, Dx, Dy, radius = 0;
    double latFirstInRadians, lonFirstInRadians, LoVInRadians, Latin1InRadians, Latin2InRadians,
        LaDInRadians;
    double earthMajorAxisInMetres, earthMinorAxisInMetres;

    grib_iterator_lambert_conformal* self = (grib_iterator_lambert_conformal*)iter;

    const char* sradius            = grib_arguments_get_name(h, args, self->carg++);
    const char* snx                = grib_arguments_get_name(h, args, self->carg++);
    const char* sny                = grib_arguments_get_name(h, args, self->carg++);
    const char* sLoVInDegrees      = grib_arguments_get_name(h, args, self->carg++);
    const char* sLaDInDegrees      = grib_arguments_get_name(h, args, self->carg++);
    const char* sLatin1InDegrees   = grib_arguments_get_name(h, args, self->carg++);
    const char* sLatin2InDegrees   = grib_arguments_get_name(h, args, self->carg++);
    const char* slatFirstInDegrees = grib_arguments_get_name(h, args, self->carg++);
    const char* slonFirstInDegrees = grib_arguments_get_name(h, args, self->carg++);
    /* Dx and Dy are in Metres */
    const char* sDx                     = grib_arguments_get_name(h, args, self->carg++);
    const char* sDy                     = grib_arguments_get_name(h, args, self->carg++);
    const char* siScansNegatively       = grib_arguments_get_name(h, args, self->carg++);
    const char* sjScansPositively       = grib_arguments_get_name(h, args, self->carg++);
    const char* sjPointsAreConsecutive  = grib_arguments_get_name(h, args, self->carg++);
    const char* salternativeRowScanning = grib_arguments_get_name(h, args, self->carg++);

    if ((err = grib_get_long_internal(h, snx, &nx)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, sny, &ny)) != GRIB_SUCCESS)
        return err;

    is_oblate = grib_is_earth_oblate(h);

    if (is_oblate) {
        if ((err = grib_get_double_internal(h, "earthMajorAxisInMetres", &earthMajorAxisInMetres)) != GRIB_SUCCESS) return err;
        if ((err = grib_get_double_internal(h, "earthMinorAxisInMetres", &earthMinorAxisInMetres)) != GRIB_SUCCESS) return err;
    }

    if (iter->nv != nx * ny) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Wrong number of points (%ld!=%ldx%ld)", iter->nv, nx, ny);
        return GRIB_WRONG_GRID;
    }
    if ((err = grib_get_double_internal(h, sradius, &radius)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, sLoVInDegrees, &LoVInDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, sLaDInDegrees, &LaDInDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, sLatin1InDegrees, &Latin1InDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, sLatin2InDegrees, &Latin2InDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, slatFirstInDegrees, &latFirstInDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, slonFirstInDegrees, &lonFirstInDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, sDx, &Dx)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, sDy, &Dy)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, sjPointsAreConsecutive, &jPointsAreConsecutive)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, sjScansPositively, &jScansPositively)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, siScansNegatively, &iScansNegatively)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, salternativeRowScanning, &alternativeRowScanning)) != GRIB_SUCCESS)
        return err;

    /*
     * See Wolfram MathWorld: http://mathworld.wolfram.com/LambertConformalConicProjection.html
     */
    latFirstInRadians = latFirstInDegrees * DEG2RAD;
    lonFirstInRadians = lonFirstInDegrees * DEG2RAD;
    Latin1InRadians   = Latin1InDegrees * DEG2RAD;
    Latin2InRadians   = Latin2InDegrees * DEG2RAD;
    LaDInRadians      = LaDInDegrees * DEG2RAD;
    LoVInRadians      = LoVInDegrees * DEG2RAD;

    if (is_oblate) {
        err = init_oblate(h, self, iter->nv, nx, ny,
                          LoVInDegrees,
                          Dx, Dy, earthMinorAxisInMetres, earthMajorAxisInMetres,
                          latFirstInRadians, lonFirstInRadians,
                          LoVInRadians, Latin1InRadians, Latin2InRadians,
                          LaDInRadians);
    }
    else {
        err = init_sphere(h, self, iter->nv, nx, ny,
                          LoVInDegrees,
                          Dx, Dy, radius,
                          latFirstInRadians, lonFirstInRadians,
                          LoVInRadians, Latin1InRadians, Latin2InRadians, LaDInRadians,
                          iScansNegatively, jScansPositively, jPointsAreConsecutive);
    }
    if (err) return err;

    iter->e = -1;

    /* Apply the scanning mode flags which may require data array to be transformed */
    err = transform_iterator_data(h, iter->data,
                                  iScansNegatively, jScansPositively, jPointsAreConsecutive, alternativeRowScanning,
                                  iter->nv, nx, ny);
    return err;
}

static int destroy(grib_iterator* i)
{
    grib_iterator_lambert_conformal* self = (grib_iterator_lambert_conformal*)i;
    const grib_context* c                 = i->h->context;

    grib_context_free(c, self->lats);
    grib_context_free(c, self->lons);
    return 1;
}
