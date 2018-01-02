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
   CLASS      = nearest
   SUPER      = grib_nearest_class_gen
   IMPLEMENTS = init
   IMPLEMENTS = find;destroy
   MEMBERS = const char* J
   MEMBERS = const char* K
   MEMBERS = const char* M
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "nearest.class" and rerun ./make_class.pl

*/


static void init_class              (grib_nearest_class*);

static int init               (grib_nearest* nearest,grib_handle* h,grib_arguments* args);
static int find(grib_nearest* nearest, grib_handle* h,double inlat, double inlon, unsigned long flags, double* outlats,double* outlons, double *values,double *distances, int *indexes,size_t *len);
static int destroy            (grib_nearest* nearest);

typedef struct grib_nearest_sh{
  grib_nearest nearest;
/* Members defined in gen */
	const char* values_key;
	const char* radius;
	int cargs;
/* Members defined in sh */
	const char* J;
	const char* K;
	const char* M;
} grib_nearest_sh;

extern grib_nearest_class* grib_nearest_class_gen;

static grib_nearest_class _grib_nearest_class_sh = {
    &grib_nearest_class_gen,                         /* super                     */
    "sh",                         /* name                      */
    sizeof(grib_nearest_sh),      /* size of instance          */
    0,                              /* inited */
    &init_class,                    /* init_class */
    &init,                          /* constructor               */
    &destroy,                       /* destructor                */
    &find,                          /* find nearest              */
};

grib_nearest_class* grib_nearest_class_sh = &_grib_nearest_class_sh;


static void init_class(grib_nearest_class* c)
{
}
/* END_CLASS_IMP */

#define LEGENDRE_SIZE(L) (L+1)*(L+2)/2

static void grib_trigs(int m,double lambda,double* c,double* s);
static int grib_invtrans_legendre(int L,double x, double* RI,double* TR,double* TI);
static double grib_invtrans_trig(int L,double* TR,double* TI,
        double *c,double *s);
static int grib_invtrans(grib_context *c,int L,double latdeg,double londeg,double* values, double* result);

static int init(grib_nearest* nearest,grib_handle* h,grib_arguments* args)
{
    grib_nearest_sh* self = (grib_nearest_sh*) nearest;
    self->J  = grib_arguments_get_name(h,args,self->cargs++);
    self->K  = grib_arguments_get_name(h,args,self->cargs++);
    self->M  = grib_arguments_get_name(h,args,self->cargs++);
    return 0;
}

static int find(grib_nearest* nearest, grib_handle* h,
        double inlat, double inlon,unsigned long flags,
        double* outlats,double* outlons,
        double *outvalues,double *distances,int* indexes, size_t *len)
{
    grib_nearest_sh* self = (grib_nearest_sh*) nearest;
    long J,K,M;
    double *values;
    int size,i,ret;
    size_t vsize=0;
    double val;

    if( (ret =  grib_get_long(h,self->J,&J))!= GRIB_SUCCESS)
        return ret;
    if( (ret =  grib_get_long(h,self->K,&K))!= GRIB_SUCCESS)
        return ret;
    if( (ret =  grib_get_long(h,self->M,&M))!= GRIB_SUCCESS)
        return ret;

    size=2*LEGENDRE_SIZE(J);
    vsize=size;
    values=(double*)grib_context_malloc_clear(h->context,sizeof(double)*size);
    if (!values) {
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "nearest_sh: unable to allocate %d bytes",
                sizeof(double)*size);
        return GRIB_OUT_OF_MEMORY;
    }

    if( (ret =  grib_get_double_array(h,self->values_key,values,&vsize))
            != GRIB_SUCCESS)
        return ret;

    Assert(vsize==size);

    ret=grib_invtrans(h->context,J,inlat,inlon,values, &val);
    if (ret != GRIB_SUCCESS) return ret;

    grib_context_free(h->context,values);

    for (i=0;i<4;i++) {
        outlats[i]=inlat;
        outlons[i]=inlon;
        outvalues[i]=val;
        indexes[i]=-1;
    }

    return GRIB_SUCCESS;
}

static int destroy(grib_nearest* nearest)
{
    return GRIB_SUCCESS;
}

static void grib_trigs(int m,double lambda,double* c,double* s)
{
    int i;
    double a,b;

    b=sin(lambda);
    a=1-2*sin(lambda/2.0)*sin(lambda/2.0);
    c[0]=1.0; s[0]=0.0;

    for (i=1;i<=m;i++) {
        c[i]=a*c[i-1]-b*s[i-1];
        s[i]=a*s[i-1]+b*c[i-1];
    }

}

static double grib_invtrans_trig(int L,double* TR,double* TI,double *c,double *s)
{
    double ret=0;
    int m=0;

    for (m=1;m<=L;m++) {
        ret+= TR[m] * c[m];
        printf("++ %d ++ %.20e %g %g\n",m,ret,TR[m],c[m]);
        ret-= TI[m] * s[m];
        printf("+- %d ++ %.20e %g %g\n",m,ret,TI[m],s[m]);
    }
    ret=2*ret+TR[0];

    return ret;
}

static int grib_invtrans_legendre(int L,double x,
        double* RI,double* TR,double* TI)
{
    int l,m;
    double y2;
    double f,of,fx,p0;
    double *pP,*oP,*pRI;

    if (fabs(x) > 1.0) {
        printf("grib_legendreP: invalid x=%g must be abs(x)>0\n",x);
        return GRIB_INVALID_ARGUMENT;
    }
    if (L<0) {
        printf("grib_legendreP: invalid L=%d must be >0\n",L);
        return GRIB_INVALID_ARGUMENT;
    }

    pP=(double*)malloc(sizeof(double)*(L+1));
    if (!pP) {
        printf("unable to allocate %d bytes\n",(int)sizeof(double)*(L+1));
        return GRIB_OUT_OF_MEMORY;
    }

    y2=(1.0-x*x); fx=1; p0=1; oP=pP; pRI=RI;
    for (m=0;m<L;m++) {
        /* printf("\n"); */
        pP[0]=sqrt((2*m+1)*p0);
        if (m & 1) pP[0]=-pP[0];
        TR[m]=pP[0] * *(pRI++);
        TI[m]=pP[0] * *(pRI++);
        printf("-- (%d,%d) %.20e %.20e\n",m,m,TR[m],pP[0]);
        of=sqrt(2*m+3);
        pP[1]=x*of*pP[0];
        TR[m]+=pP[1] * *(pRI++);
        TI[m]+=pP[1] * *(pRI++);
        printf("-- (%d,%d) %.20e %.20e\n",m+1,m,TR[m],pP[1]);
        for (l=m+2;l<=L;l++) {
            f=sqrt((4.0*l*l-1.0)/(l*l-m*m));
            pP[2]=(x*pP[1]-pP[0]/of)*f;
            TR[m]+=pP[2] * *(pRI++);
            TI[m]+=pP[2] * *(pRI++);
            printf("-- (%d,%d) %.20e %.20e\n",l,m,TR[m],pP[2]);
            of=f;
            pP++;
        }
        pP=oP;
        p0*=y2*fx/(fx+1.0);
        fx+=2;
    }
    /* printf("\n"); */
    pP[0]=sqrt((2*L+1)*p0);
    if (L & 1) pP[0]=-pP[0];
    TR[L]=pP[0] * *(pRI++);
    TI[L]=pP[0] * *(pRI++);
    printf("-- (%d,%d) %.20e %.20e\n",L,L,TR[m],pP[0]);
    return GRIB_SUCCESS;
}

static int grib_invtrans(grib_context* context, int L, double latdeg, double londeg, double* values,
        double* result)
{
    double *c,*s,*TR,*TI;
    double sinlat,deg2rad,lonrad;
    int Lp1=L+1;
    int err = 0;

    deg2rad=acos(0.0)/90.0;
    sinlat=sin(latdeg*deg2rad);
    lonrad=londeg*deg2rad;

    c=(double*)grib_context_malloc_clear(context,sizeof(double)*Lp1);
    if (!c) {
        grib_context_log(context,GRIB_LOG_ERROR,
                "nearest_sh: unable to allocate %d bytes",sizeof(double)*Lp1);
        return GRIB_OUT_OF_MEMORY;
    }

    s=(double*)grib_context_malloc_clear(context,sizeof(double)*Lp1);
    if (!s) {
        grib_context_log(context,GRIB_LOG_ERROR,
                "nearest_sh: unable to allocate %d bytes",sizeof(double)*Lp1);
        return GRIB_OUT_OF_MEMORY;
    }

    grib_trigs(L,lonrad,c,s);

    TR=(double*)grib_context_malloc_clear(context,sizeof(double)*Lp1);
    if (!TR) {
        grib_context_log(context,GRIB_LOG_ERROR,
                "nearest_sh: unable to allocate %d bytes",sizeof(double)*Lp1);
        return GRIB_OUT_OF_MEMORY;
    }
    TI=(double*)grib_context_malloc_clear(context,sizeof(double)*Lp1);
    if (!TI) {
        grib_context_log(context,GRIB_LOG_ERROR,
                "nearest_sh: unable to allocate %d bytes",sizeof(double)*Lp1);
        return GRIB_OUT_OF_MEMORY;
    }

    err = grib_invtrans_legendre(L,sinlat,values,TR,TI);
    if(err) return err;

    *result=grib_invtrans_trig(L,TR,TI,c,s);

    grib_context_free(context,c);
    grib_context_free(context,s);
    grib_context_free(context,TR);
    grib_context_free(context,TI);

    return GRIB_SUCCESS;
}
