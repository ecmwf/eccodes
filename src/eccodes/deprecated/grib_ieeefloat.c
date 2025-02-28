/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *   Enrico Fucile  - 06.01.2009                                           *
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"

#if GRIB_PTHREADS
static pthread_once_t once   = PTHREAD_ONCE_INIT;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void init()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}
#elif GRIB_OMP_THREADS
static int once = 0;
static omp_nest_lock_t mutex;

static void init()
{
    GRIB_OMP_CRITICAL(lock_grib_ieeefloat_c)
    {
        if (once == 0) {
            omp_init_nest_lock(&mutex);
            once = 1;
        }
    }
}
#endif

#if 0

#else
/* old code to be deleted */

double grib_ieeefloat_error(double x)
{
    return 0;
}

double grib_long_to_ieee(unsigned long x)
{
    unsigned long s = x & 0x80000000;
    unsigned long c = (x & 0x7f800000) >> 23;
    unsigned long m;
    double val;
    long e;

    if (x == 0)
        return 0;
    Assert(c != 255);

    if (c == 0) {
        m = x & 0x007fffff;
        e = -126 - 23;
    }
    else {
        m = (x & 0x007fffff) | (1 << 23);
        e = c - 127 - 23;
    }

    val = m;

    while (e < 0) {
        val /= 2.0;
        e++;
    }
    while (e > 0) {
        val *= 2.0;
        e--;
    }

    if (s)
        val = -val;

    return val;
}

int grib_nearest_smaller_ieee_float(double a, double* x)
{
    double e = grib_long_to_ieee(grib_ieee_to_long(a));
    double b = a;

    /* printf("----> a=%g e=%g e-a=%g\n",a,e,e-a); */

    if (e > b) {
        unsigned long ub = grib_ieee_to_long(b);
        unsigned long ue;
        while (e > b) {
            /* printf("a=%g e=%g e-a=%g\n",a,e,e-a); */
            a -= (e - a);
            e = grib_long_to_ieee(grib_ieee_to_long(a));
        }
        ue = grib_ieee_to_long(e);
        Assert((ue - ub) == 1);
    }

    Assert(b >= e);
    *x = e;
    return GRIB_SUCCESS;
}

unsigned long grib_ieee_to_long(double x)
{
    /* double y = x; */
    unsigned long s = 0;
    unsigned long m;
    long p          = 0;
    unsigned long e = 0;

    if (x == 0)
        return 0;

    if (x < 0) {
        s = 1;
        x = -x;
    }
    while (x < 2) {
        x *= 2;
        p--;
    }

    while (x >= 2) {
        x /= 2;
        p++;
    }

    if (p > 127) {
        /* Overflow */
        e = 255;
        m = 0;
    }
    else if (p < -126) {
        /* int i; */
        e = 0;
        /* printf("p=%ld x=%g %ld\n",p,x,p+126+23); */
        m = x * grib_power(p + 126 + 23, 2);
    }
    else {
        e = p + 127;
        m = x * (1 << 23);
        m &= 0x007fffff;
    }

    m = (s << 31) | (e << 23) | m;

    return m;
}

#endif

#ifdef IEEE


/* 
 * To make these two routines consistent to grib_ieee_to_long and grib_long_to_ieee,
 * we should not do any byte swapping but rather perform a raw copy.
 * Byte swapping is actually implemented in grib_decode_unsigned_long and
 * grib_encode_unsigned_long.
 */
unsigned long grib_ieee64_to_long(double x)
{
    unsigned long lval;
    DebugAssert(sizeof(double) == sizeof(long));
    memcpy(&lval, &x, sizeof(long));
    return lval;
}

double grib_long_to_ieee64(unsigned long x)
{
    double dval;
    DebugAssert(sizeof(double) == sizeof(long));
    memcpy(&dval, &x, sizeof(long));
    return dval;
}

int grib_ieee_decode_array(grib_context* c, unsigned char* buf, size_t nvals, int bytes, double* val)
{
    int err = 0, i = 0, j = 0;
    unsigned char s[8] = {0,};
    float fval;
    double* pval = val;

    switch (bytes) {
        case 4:
            for (i = 0; i < nvals; i++) {
#if IEEE_LE
                for (j = 3; j >= 0; j--)
                    s[j] = *(buf++);
                memcpy(&fval, s, 4);
                val[i] = (double)fval;
#elif IEEE_BE
                memcpy(&fval, buf, 4);
                val[i] = (double)fval;
                buf += 4;
#endif
            }
            break;
        case 8:
            for (i = 0; i < nvals; i++) {
#if IEEE_LE
                for (j = 7; j >= 0; j--)
                    s[j] = *(buf++);
                memcpy(pval++, s, 8);
#elif IEEE_BE
                memcpy(pval++, buf, 8);
                buf += 8;
#endif
            }
            break;
        default:
            grib_context_log(c, GRIB_LOG_ERROR,
                             "grib_ieee_decode_array: %d bits not implemented", bytes * 8);
            return GRIB_NOT_IMPLEMENTED;
    }

    return err;
}

#else

int grib_ieee_decode_array(grib_context* c, unsigned char* buf, size_t nvals, int bytes, double* val)
{
    int err = 0, i = 0;
    long bitr = 0;

    for (i = 0; i < nvals; i++)
        val[i] = grib_long_to_ieee(grib_decode_unsigned_long(buf, &bitr, bytes * 8));

    return err;
}

#endif

#ifdef IEEE

int grib_ieee_encode_array(grib_context* c, double* val, size_t nvals, int bytes,
                           unsigned char* buf)
{
    int err = 0, i = 0, j = 0;
#if IEEE_LE
    unsigned char s4[4];
    unsigned char s8[8];
#endif
    float fval   = 0;
    double* pval = val;

    switch (bytes) {
        case 4:
            for (i = 0; i < nvals; i++) {
                fval = (float)val[i];

#if IEEE_LE
                memcpy(s4, &(fval), 4);
                for (j = 3; j >= 0; j--)
                    *(buf++) = s4[j];
#elif IEEE_BE
                memcpy(buf, &(fval), 4);
                buf += 4;
#endif
            }
            break;
        case 8:
            for (i = 0; i < nvals; i++) {
#if IEEE_LE
                memcpy(s8, pval++, 8);
                for (j = 7; j >= 0; j--)
                    *(buf++) = s8[j];
#elif IEEE_BE
                memcpy(buf, pval++, 8);
                buf += 8;
#endif
            }
            break;
        default:
            grib_context_log(c, GRIB_LOG_ERROR,
                             "grib_ieee_encode_array: %d bits not implemented", bytes * 8);
            return GRIB_NOT_IMPLEMENTED;
    }

    return err;
}

#else

int grib_ieee_encode_array(grib_context* c, double* val, size_t nvals, int bytes, unsigned char* buf)
{
    int err = 0, i = 0;
    long bitr = 0;

    for (i = 0; i < nvals; i++)
        grib_encode_unsigned_long(buf, grib_ieee_to_long(val[i]), &bitr, bytes * 8);

    return err;
}

#endif
