/*
 * Copyright 2005-2018 ECMWF.
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
static pthread_once_t once  = PTHREAD_ONCE_INIT;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void init() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex,&attr);
    pthread_mutexattr_destroy(&attr);
}
#elif GRIB_OMP_THREADS
static int once = 0;
static omp_nest_lock_t mutex;

static void init()
{
    GRIB_OMP_CRITICAL(lock_grib_ieeefloat_c)
    {
        if (once == 0)
        {
            omp_init_nest_lock(&mutex);
            once = 1;
        }
    }
}
#endif

#if 1

typedef struct ieee_table_t ieee_table_t;

struct ieee_table_t {
    int    inited;
    double e[255];
    double v[255];
    double vmin;
    double vmax;
};

static ieee_table_t ieee_table={ 0,{0,},{0,}, 0, 0 };

static void init_ieee_table()
{
    if (!ieee_table.inited) {
        unsigned long i;
        unsigned long mmin=0x800000;
        unsigned long mmax=0xffffff;
        double e=1;
        for (i=1; i<=104;i++) {
            e*=2;
            ieee_table.e[i+150]=e;
            ieee_table.v[i+150]=e*mmin;
        }
        ieee_table.e[150]=1;
        ieee_table.v[150]=mmin;
        e=1;
        for (i=1; i<150;i++) {
            e/=2;
            ieee_table.e[150-i]=e;
            ieee_table.v[150-i]=e*mmin;
        }
        ieee_table.vmin=ieee_table.v[1];
        ieee_table.vmax=ieee_table.e[254]*mmax;
        ieee_table.inited=1;
        /*for (i=0;i<128;i++) printf("++++ ieee_table.v[%d]=%g\n",i,ieee_table.v[i]);*/
    }
}

static void init_table_if_needed()
{
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&mutex)

    if (!ieee_table.inited) init_ieee_table();

    GRIB_MUTEX_UNLOCK(&mutex)
}

static void binary_search(double xx[], const unsigned long n, double x, unsigned long *j)
{
    /*These routine works only on ascending ordered arrays*/
    unsigned long ju,jm,jl;
    jl=0;
    ju=n;
    while (ju-jl > 1) {
        jm=(ju+jl) >> 1;
        /* printf("jl=%lu jm=%lu ju=%lu\n",jl,jm,ju); */
        /* printf("xx[jl]=%.10e xx[jm]=%.10e xx[ju]=%.10e\n",xx[jl],xx[jm],xx[ju]); */
        if (x >= xx[jm]) jl=jm;
        else ju=jm;
    }
    *j=jl;
}

double grib_ieee_table_e(unsigned long e)
{
    init_table_if_needed();
    return ieee_table.e[e];
}

double grib_ieee_table_v(unsigned long e)
{
    init_table_if_needed();
    return ieee_table.v[e];
}

unsigned long grib_ieee_to_long(double x)
{
    unsigned long s = 0;
    unsigned long mmax = 0xffffff;
    unsigned long mmin = 0x800000;
    unsigned long m = mmax;
    unsigned long e=0;
    double rmmax=mmax+0.5;

    init_table_if_needed();

    /* printf("\ngrib_ieee_to_long: x=%.20e\n",x); */
    if (x < 0)  {  s  = 1; x = -x; }

    /* Underflow */
    if (x < ieee_table.vmin) {
        /*printf("grib_ieee_to_long: (x < ieee_table.vmin) x=%.20e vmin=%.20e v=0x%lX\n",x,ieee_table.vmin,(s<<31));*/
        return (s << 31);
    }

    /* Overflow */
    if (x > ieee_table.vmax) {
        fprintf(stderr, "grib_ieee_to_long: Number is too large: x=%.20e > xmax=%.20e\n", x, ieee_table.vmax);
        Assert(0);
        return 0;
    }

    binary_search(ieee_table.v, 254, x, &e);

    /* printf("grib_ieee_to_long: e=%ld\n",e); */

    x/=ieee_table.e[e];

    /* printf("grib_ieee_to_long: x=%.20e\n",x); */

    while(x < mmin ) { x *= 2; e--;
    /* printf("grib_ieee_to_long (e--): x=%.20e e=%ld \n",x,e); */
    }

    while(x > rmmax ) { x /= 2; e++;
    /* printf("grib_ieee_to_long (e++): x=%.20e e=%ld \n",x,e); */
    }

    m=x+0.5;
    /* printf("grib_ieee_to_long: m=0x%lX (%lu) x=%.10e \n",m,m,x ); */
    if ( m > mmax ) { e++; m=0x800000;
    /* printf("grib_ieee_to_long: ( m > mmax ) m=0x%lX (%lu) x=%.10e \n",m,m,x ); */
    }

    /* printf("grib_ieee_to_long: s=%lu c=%lu (0x%lX) m=%lu (0x%lX)\n",s,e,e,m,m ); */

    return (s << 31) | ( e << 23 ) | ( m & 0x7fffff );
}

double grib_ieeefloat_error(double x)
{
    unsigned long e=0;

    init_table_if_needed();

    if (x < 0)  x = -x;

    /* Underflow */
    if (x < ieee_table.vmin) return ieee_table.vmin;

    /* Overflow */
    if (x > ieee_table.vmax) {
        fprintf(stderr, "grib_ieeefloat_error: Number is too large: x=%.20e > xmax=%.20e\n", x, ieee_table.vmax);
        Assert(0);
        return 0;
    }

    binary_search(ieee_table.v, 254, x, &e);

    return ieee_table.e[e] ;
}

double grib_long_to_ieee(unsigned long x)
{
    unsigned long s = x  & 0x80000000;
    unsigned long c = (x & 0x7f800000) >> 23;
    unsigned long m = (x & 0x007fffff);

    double val;

#ifdef DEBUG
    if ( x > 0 && x < 0x800000 ) {
        fprintf(stderr, "grib_long_to_ieee: Invalid input %lu\n", x);
        Assert(0);
    }
#endif
    init_table_if_needed();

    if (c == 0 && m==0) return 0;

    if (c == 0)  {
        m |= 0x800000;
        c=1;
    } else  m |= 0x800000;

    val=m*ieee_table.e[c];
    if(s) val = -val;

    return val;
}

unsigned long grib_ieee_nearest_smaller_to_long(double x)
{
    unsigned long l;
    unsigned long e;
    unsigned long m ;
    unsigned long s;
    unsigned long mmin = 0x800000;
    double y,eps;

    if(x == 0) return 0;

    init_table_if_needed();

    l=grib_ieee_to_long(x);
    y=grib_long_to_ieee(l);

    if ( x < y ) {
        if ( x < 0 && -x < ieee_table.vmin ) {
            l=0x80800000;
        } else {
            e = (l & 0x7f800000) >> 23;
            m = (l & 0x007fffff) | 0x800000;
            s  = l  & 0x80000000;

            if ( m == mmin ) {
                /* printf("grib_ieee_nearest_smaller_to_long: m == mmin (0x%lX) e=%lu\n",m,e);  */
                e = s ? e : e-1;
                if (e<1) e=1;
                if (e>254) e=254;
                /* printf("grib_ieee_nearest_smaller_to_long: e=%lu \n",e);  */
            }

            eps=ieee_table.e[e];

            /* printf("grib_ieee_nearest_smaller_to_long: x<y\n"); */
            l=grib_ieee_to_long(y-eps);
            /* printf("grib_ieee_nearest_smaller_to_long: grib_ieee_to_long(y-eps)=0x%lX y=%.10e eps=%.10e x=%.10e\n",l,y,eps,x); */
        }
    } else return l;

    if (x<grib_long_to_ieee(l)) {
        printf("grib_ieee_nearest_smaller_to_long: x=%.20e grib_long_to_ieee(0x%lX)=%.20e\n",x,l,grib_long_to_ieee(l));
        Assert(x >= grib_long_to_ieee(l));
    }

    return l;
}

int grib_nearest_smaller_ieee_float(double a,double* ret)
{
    unsigned long l=0;

    init_table_if_needed();

    if (a>ieee_table.vmax) return GRIB_INTERNAL_ERROR;

    l=grib_ieee_nearest_smaller_to_long(a);
    *ret=grib_long_to_ieee(l);
    return GRIB_SUCCESS;
}

#else
/* old code to be deleted */

double grib_ieeefloat_error(double x) {return 0;}

double grib_long_to_ieee(unsigned long x)
{
    unsigned long s = x  & 0x80000000;
    unsigned long c = (x & 0x7f800000) >> 23;
    unsigned long m;
    double val;
    long e;

    if(x == 0) return 0;
    Assert(c != 255);

    if(c == 0)  {   m = x & 0x007fffff;  e = -126 - 23; }
    else { m = (x & 0x007fffff) | (1<<23); e = c - 127 - 23; }

    val = m;

    while(e < 0)  { val /= 2.0; e++; }
    while(e > 0)  { val *= 2.0; e--; }

    if(s) val = -val;

    return val;
}

int grib_nearest_smaller_ieee_float(double a,double* x)
{
    double e =  grib_long_to_ieee(grib_ieee_to_long(a));
    double b = a;

    /* printf("----> a=%g e=%g e-a=%g\n",a,e,e-a); */

    if( e > b )
    {
        unsigned long ub = grib_ieee_to_long(b);
        unsigned long ue;
        while(e>b)
        {
            /* printf("a=%g e=%g e-a=%g\n",a,e,e-a); */
            a -= (e-a);
            e = grib_long_to_ieee(grib_ieee_to_long(a));
        }
        ue = grib_ieee_to_long(e);
        Assert((ue-ub) == 1);
    }

    Assert(b >= e);
    *x=e;
    return GRIB_SUCCESS;
}

unsigned long grib_ieee_to_long(double x)
{
    /* double y = x; */
    unsigned long s = 0;
    unsigned long m;
    long p = 0;
    unsigned long e = 0;

    if(x == 0) return 0;

    if(x < 0) { s  = 1; x = -x; }
    while(x < 2) { x *= 2; p--; }

    while(x >= 2) { x /= 2; p++; }

    if(p > 127 ) {
        /* Overflow */
        e = 255;
        m = 0;
    }
    else if(p < -126) {
        /* int i; */
        e = 0;
        /* printf("p=%ld x=%g %ld\n",p,x,p+126+23); */
        m = x * grib_power(p+126+23,2);
    }
    else {
        e = p + 127;
        m = x * ( 1 << 23);
        m &= 0x007fffff;
    }

    m =  (s << 31) | ( e << 23 ) | m;

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
    memcpy(&lval,&x,8);
    return lval;
}

double grib_long_to_ieee64 (unsigned long x)
{
    double dval;
    memcpy(&dval,&x,8);
    return dval;
}


int grib_ieee_decode_array(grib_context* c,unsigned char* buf,size_t nvals,int bytes,double* val)
{
    int err=0,i=0,j=0;
    unsigned char s[8]={0,};
    float fval;
    double* pval=val;

    switch (bytes) {
    case 4:
        for (i=0;i<nvals;i++) {
#if IEEE_LE
            for (j=3;j>=0;j--)
                s[j]=*(buf++);
            memcpy(&fval,s,4);
            val[i]=(double)fval;
#elif IEEE_BE
            memcpy(&fval,buf,4);
            val[i]=(double)fval;
            buf+=4;
#endif
        }
        break;
    case 8:
        for (i=0;i<nvals;i++) {
#if IEEE_LE
            for (j=7;j>=0;j--)
                s[j]=*(buf++);
            memcpy(pval++,s,8);
#elif IEEE_BE
            memcpy(pval++,buf,8);
            buf+=8;
#endif
        }
        break;
    default:
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_ieee_decode_array: %d bits not implemented",bytes*8);
        return GRIB_NOT_IMPLEMENTED;
    }

    return err;
}

#else

int grib_ieee_decode_array(grib_context* c,unsigned char* buf,size_t nvals,int bytes,double* val)
{
    int err=0,i=0;
    long bitr=0;

    for(i=0;i< nvals;i++)
        val[i] = grib_long_to_ieee(grib_decode_unsigned_long(buf,&bitr,bytes*8));

    return err;
}

#endif

#ifdef IEEE

int grib_ieee_encode_array(grib_context* c,double* val,size_t nvals,int bytes,
        unsigned char* buf) {
    int err=0,i=0,j=0;
    unsigned char s4[4];
    unsigned char s8[8];
    float fval=0;
    double *pval=val;

    switch (bytes) {
    case 4:
        for (i=0;i<nvals;i++) {
            fval=(float)val[i];

#if IEEE_LE
            memcpy(s4,&(fval),4);
            for (j=3;j>=0;j--)
                *(buf++)=s4[j];
#elif IEEE_BE
            memcpy(buf,&(fval),4);
            buf+=4;
#endif
        }
        break;
    case 8:
        for (i=0;i<nvals;i++) {
#if IEEE_LE
            memcpy(s8,pval++,8);
            for (j=7;j>=0;j--)
                *(buf++)=s8[j];
#elif IEEE_BE
            memcpy(buf,pval++,8);
            buf+=8;
#endif
        }
        break;
    default:
        grib_context_log(c,GRIB_LOG_ERROR,
                "grib_ieee_encode_array: %d bits not implemented",bytes*8);
        return GRIB_NOT_IMPLEMENTED;
    }

    return err;
}

#else

int grib_ieee_encode_array(grib_context* c, double* val, size_t nvals, int bytes, unsigned char* buf)
{
    int err=0,i=0;
    long bitr=0;

    for(i=0;i< nvals;i++)
        grib_encode_unsigned_long(buf, grib_ieee_to_long(val[i]), &bitr, bytes*8);

    return err;
}

#endif
