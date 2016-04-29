/*
 * Copyright 2005-2016 ECMWF.
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

/* Return n to the power of s */
double grib_power(long s,long n)
{
    double divisor = 1.0;
    while(s < 0)
    {
        divisor /= n;
        s++;
    }
    while(s > 0)
    {
        divisor *= n;
        s--;
    }
    return divisor;
}

long grib_get_binary_scale_fact(double max, double min, long bpval,int *ret)
{
    double range   = max - min;
    double  zs     = 1;
    long    scale  = 0;
    const long last = 127; /* Depends on edition, should be parameter */
    unsigned long maxint = 0;

    /* See ECC-246
      unsigned long maxint = grib_power(bpval,2) - 1;
      double dmaxint=(double)maxint;
    */
    const double dmaxint = grib_power(bpval,2) - 1;
    if (dmaxint >= ULONG_MAX) {
        *ret = GRIB_OUT_OF_RANGE; /*overflow*/
        return 0;
    }
    maxint = (unsigned long)dmaxint; /* Now it's safe to cast */

    *ret=0;
    if (bpval < 1) {
        *ret = GRIB_ENCODING_ERROR; /* constant field */
        return 0;
    }

    Assert (bpval >= 1);
    /*   printf("---- Maxint %ld range=%g\n",maxint,range);    */
    if(range == 0)
        return 0;

    /* range -= 1e-10; */
    while( (range * zs)  <= dmaxint) {
        scale--;
        zs *= 2;
    }

    while( (range * zs ) > dmaxint) {
        scale++;
        zs /= 2;
    }

    while( (unsigned long)(range * zs + 0.5)  <= maxint) {
        scale--;
        zs *= 2;
    }

    while( (unsigned long)(range * zs + 0.5) > maxint) {
        scale++;
        zs /= 2;
    }

    if (scale < -last) {
        *ret=GRIB_UNDERFLOW;
        /*printf("grib_get_binary_scale_fact: max=%g min=%g\n",max,min);*/
        scale = -last;
    }
    Assert(scale <= last);
    return scale;
}

long grib_get_bits_per_value(double max, double min, long binary_scale_factor)
{
    double range         = max - min;
    double  zs           = 1;
    long          scale  = 0;
    const long last = 127; /* Depends on edition, should be parameter */

    unsigned long maxint = grib_power(binary_scale_factor,2) - 1;
    double dmaxint=(double)maxint;

    if (maxint==0) maxint=1;

    /*  printf("---- Maxint %ld range=%g\n",maxint,range);     */
    if(range == 0)
        return 0;

    /* range -= 1e-10; */
    while( (range * zs)  <= dmaxint) { scale--;zs *= 2;}

    while( (range * zs ) > dmaxint) {scale++;zs /= 2;}

    while( (unsigned long)(range * zs + 0.5)  <= maxint) {
        scale--;
        zs *= 2;
    }

    while( (unsigned long)(range * zs + 0.5) > maxint) {
        scale++;
        zs /= 2;
    }

    Assert(scale >= -last && scale <= last);
    /* printf("---- scale=%ld\n",scale);*/

    return scale;
}

long grib_get_decimal_scale_fact(double max, double min, long bpval,long binary_scale)
{
    double range         = max - min;
    double  zs           = 1;
    long          scale  = 0;
    const long last = 127; /* Depends on edition, should be parameter */

    unsigned long maxint = grib_power(bpval,2) - 1;
    double dmaxint=(double)maxint;

    range*=grib_power(-binary_scale,2);

    Assert (bpval >= 1);
    if(range == 0)
        return 0;

    while( (range * zs ) > dmaxint) {
        scale--;
        zs /= 10;
    }
    while( (range * zs)  <= dmaxint) {
        scale++;
        zs *= 10;
    }

    while( (unsigned long)(range * zs + 0.5) > maxint) {
        scale--;
        zs /= 10;
    }
    while( (unsigned long)(range * zs + 0.5)  <= maxint) {
        scale++;
        zs *= 10;
    }

    /* printf("grib_api: decimal_scale_fact=%ld max=%g min=%g bits_per_value=%ld binary_scale=%ld\n",scale,max,min,bpval,binary_scale); */

    Assert(scale >= -last && scale <= last);

    return scale;
}
