/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "grib_api_internal.h"

#define NO_PRINT   1
#define PRINT_ALL (1<<1)
#define PRINT_ROUND_ERRORS (1<<2)
#define PRINT_TRUNCATION (1<<3)
#define NO_NEAREST_SMALLER_IBM_FLOAT (1<<4)

void test_bits(double x,unsigned long mode) {
  double y=0,z=0,w=0,t=0;
  double dy=0,dz=0,dw=0,dt=0;
  unsigned long i=0,j=0,k=0,l=0;

  i=grib_ibm_to_long(x);
  y=grib_long_to_ibm(i);
  dy=y-x;

  z=grib_long_to_ibm(j);
  dz=z-x;

  k=grib_ibm_nearest_smaller_to_long(x);
  w=grib_long_to_ibm(k);
  dw=w-x;

  if ( !(mode & NO_NEAREST_SMALLER_IBM_FLOAT) ) {
    t=grib_nearest_smaller_ibm_float(x);
    l=grib_ibm_to_long(y);
    dt=t-x;
  }

  if (mode & PRINT_ALL) {
    printf("x=%.20e \n",x);
    printf("grib_ibm_to_long                  %lu (0x%lx) -> %.20e diff=%.20e\n",i,i,y,dy);
    printf("grib_nearest_smaller_ibm_float    %lu (0x%lx) -> %.20e diff=%.20e\n",l,l,t,dt);
    printf("grib_ibm_nearest_smaller_to_long  %lu (0x%lx) -> %.20e diff=%.20e\n",k,k,w,dw);
  }

  if (fabs(dz) < fabs(dy) && ( (mode & PRINT_ROUND_ERRORS )| (mode & PRINT_ALL ) )) {
    printf("!!!!!!!!!!--------------  Rounding error \n");
    printf("x=%.20e \n",x);
    printf("grib_ibm_to_long                  %lu (0x%lx) -> %.20e diff=%.20e\n",i,i,y,dy);
    printf("grib_ibm_to_long_trunc            %lu (0x%lx) -> %.20e diff=%.20e\n",j,j,z,dz);
    printf("grib_nearest_smaller_ibm_float    %lu (0x%lx) -> %.20e diff=%.20e\n",l,l,t,dt);
    printf("grib_ibm_nearest_smaller_to_long  %lu (0x%lx) -> %.20e diff=%.20e\n",k,k,w,dw);
  }

  if (( w != t) && ( (mode & PRINT_TRUNCATION ) | ( mode & PRINT_ALL ) ) )  {
    printf("!!!!!!!!!!---------------  Different nearest smaller \n");
    printf("x=%.20e \n",x);
    printf("grib_ibm_to_long                  %lu (0x%lx) -> %.20e diff=%.20e\n",i,i,y,dy);
    printf("grib_ibm_to_long_trunc            %lu (0x%lx) -> %.20e diff=%.20e\n",j,j,z,dz);
    printf("grib_nearest_smaller_ibm_float    %lu (0x%lx) -> %.20e diff=%.20e\n",l,l,t,dt);
    printf("grib_ibm_nearest_smaller_to_long  %lu (0x%lx) -> %.20e diff=%.20e\n",k,k,w,dw);
  }
}

void print_machine_parameters() {
  int ibeta=0, it=0, irnd=0, ngrd=0, machep=0, negep=0, iexp=0, minexp=0, maxexp=0;
  float eps=0, epsneg=0, xmin=0, xmax=0;

  printf("-- Machine parameters ---\n");
  printf("ibeta=%d\n",ibeta);
  printf("it=%d\n",it);
  printf("irnd=%d\n",irnd);
  printf("ngrd=%d\n",ngrd);
  printf("machep=%d\n",machep);
  printf("negep=%d\n",negep);
  printf("iexp=%d\n",iexp);
  printf("minexp=%d\n",minexp);
  printf("maxexp=%d\n",maxexp);
  printf("eps=%g\n",eps);
  printf("epsneg=%g\n",epsneg);
  printf("xmin=%.20e\n",xmin);
  printf("xmax=%.20e\n",xmax);
  printf("-------------------------\n\n");

}

int main(int argc, char *argv[])
{
  double x,y,z,w,d,eps,epst,epsh;
  double dt=0;
  unsigned long i,j,n,nc,c,m;
  unsigned long seed=123;
  unsigned char ibm[4];
  float xf=0;
  int r;
  unsigned long iminp=0x00100000, imaxp=0x7fffffff;
  unsigned long iminn=0x80100000, imaxn=0xffffffff;
  double dminp=0,dmaxp=0,dminn=0,dmaxn=0;
  double dminpn=0,dmaxpp=0,dminnn=0,dmaxnp=0;
  unsigned long A;
  long e;
  int k,cc;

  dminp=grib_long_to_ibm(iminp);
  dminpn=grib_long_to_ibm(iminp+1);
  dmaxp=grib_long_to_ibm(imaxp);
  dmaxpp=grib_long_to_ibm(imaxp-1);
  dminn=grib_long_to_ibm(iminn);
  dminnn=grib_long_to_ibm(iminn+1);
  dmaxn=grib_long_to_ibm(imaxn);
  dmaxnp=grib_long_to_ibm(imaxn-1);

  x=-1.33642013258103330000e-83;
  i=grib_ibm_to_long(x);
  j=grib_ibm_nearest_smaller_to_long(x);
  printf("grib_ibm_to_long(%.20e)=0x%lX grib_long_to_ibm(0x%lX)=%.20e grib_ibm_nearest_smaller_to_long(%.20e)=0x%lX\n",
          x,i,i,grib_long_to_ibm(i),x,j);
  printf("grib_long_to_ibm(grib_ibm_nearest_smaller_to_long(%.20e))=%.20e\n",x,grib_long_to_ibm(j));
  exit(0);

/*
  if (argc > 1 ) {
    x=atof(argv[1]);
    test_bits(x,PRINT_ALL);
    printf("-----------------\n");
    exit(0);
  }
  */

/*
  i=0x49800000;
  x=grib_long_to_ibm(i);
  j=grib_ibm_to_long(x);
  printf(" grib_long_to_ibm(0x%lX)=%.8e  grib_ibm_to_long(grib_long_to_ibm(0x%lX))=0x%lX\n",i,x,i,j);
  exit(0);
*/

  printf("-------------------------------- ibm float -------------------------------\n");
  printf("Min positive 0x%lX   ->  %.8e ",iminp,dminp);
  printf("-- next  0x%lX   ->  %.8e  diff=%.3e\n",iminp+1,dminpn,dminp-dminpn);
  printf("Max positive 0x%lX ->  %.8e ",imaxp,dmaxp);
  printf("-- prev  0x%lX ->  %.8e  diff= %.3e\n",imaxp-1,dmaxpp,dmaxp-dmaxpp);
  printf("Min negative 0x%lX -> %.8e ",iminn,dminn);
  printf("-- next  0x%lX -> %.8e  diff= %.3e\n",iminn+1,dminnn,dminn-dminnn);
  printf("Max negative 0x%lX -> %.8e ",imaxn,dmaxn);
  printf("-- prev  0x%lX -> %.8e  diff=%.3e\n",imaxn-1,dmaxnp,dmaxn-dmaxnp);
  printf("%lu negative values, %lu positive values\n",(imaxn-iminn),(imaxp-iminp));
  printf("--------------------------------------------------------------------------\n");
  fflush(stdout);
#if 0
  if (0) {
  printf("---- Test --------- ibm_to_long(long_to_ibm()) ---------------------------\n");
  nc=((imaxn-iminp))/100;
  c=0;
  for (i=iminp;i<imaxn;i++) {
   if (c==nc) { printf("%lu \n",i-iminp);fflush(stdout); c=0; }
     if ( (i & 0x00800000) != 0 &&  i != grib_ibm_to_long(grib_long_to_ibm(i))) {
     printf("test failed: 0x%lX != ibm_to_long(long_to_ibm(0x%lX)) \n",grib_ibm_to_long(grib_long_to_ibm(i)),i);
     exit(1);
   }
   c++;
  }
  printf("%lu \n",i-iminp);
  printf("---- Test passed ---------------------------------------------------------\n");
  }

  printf("\n--- Negative Underflow test  --------------\n");

  x=1;
  y=fabs(x)/2;

  while ( grib_long_to_ibm(grib_ibm_to_long(x)) != 0 ) {
    /* test_bits(x,PRINT_ALL); */
    test_bits(x,PRINT_TRUNCATION | PRINT_ROUND_ERRORS);
    x*=y;
  }
  test_bits(x,PRINT_ALL);
  printf("----------------------------------\n");


  printf("\n--- Positive Underflow test  --------------\n");

  x=1;
  y=fabs(x)/2;
  w=0;
  while ( grib_long_to_ibm(grib_ibm_to_long(x)) != 0 ) {
    test_bits(x,PRINT_TRUNCATION | PRINT_ROUND_ERRORS);
    /* test_bits(x,PRINT_ALL); */
  w=x;
    x*=y;
  }
  test_bits(x,PRINT_ALL);
  printf("----------------------------------\n");

  printf("\n--- Positive Overflow test  --------------\n");
  x=grib_long_to_ibm(0x7fffffff);
  test_bits(x,PRINT_ALL);
  x=7.237005145973e75;
  test_bits(x,PRINT_ALL);
  x=7.237005145974e75;
  test_bits(x,PRINT_ALL);
  printf("----------------------------------\n");

  printf("\n--- Negative Overflow test  --------------\n");
  x=grib_long_to_ibm(0xffffffff);
  test_bits(x,PRINT_ALL );
  x=-7.237005145974e75;
  test_bits(x,PRINT_ALL | NO_NEAREST_SMALLER_IBM_FLOAT);
  printf("----------------------------------\n");
#endif

  printf("\n--- Rounding & nearest smaller test  --------------\n");
  fflush(stdout);
  nc=((imaxn-iminp))/100;
  printf("imaxn=%lu iminp=%lu nc=%lu\n",imaxn,iminp,nc);
  fflush(stdout);
  c=0;
  cc=1;
  i=0xCC100000;
  /* for (i=0x7F100000 ;i<imaxn;i++)  */
  for (i=iminp;i<imaxn;i++)
  {
    if ( (i & 0x00100000) == 0  &&
         (i & 0x00200000) == 0  &&
         (i & 0x00400000) == 0  &&
         (i & 0x00800000) == 0
         ) {c++;continue;}
    if (c>=nc) { printf("%d - %lu 0x%lX\n",cc,i-iminp,i);fflush(stdout); c=0;cc++; }

    A = (i & 0x7f000000) >> 24;
    e = A-70;
    eps=1;
    m = (i & 0xffffff);
    /* printf("---m=0x%lX\n",m); */

    eps=grib_ibm_table_e(A);

    epsh=eps/2;
    epst=eps/3;

    y=grib_long_to_ibm(i);
    if (grib_ibm_to_long(y) != i) {
        printf("------ grib_long_to_ibm(grib_ibm_to_long()) Failure -----------------\n");
        printf("i=0x%lX\n",i);
        printf("grib_long_to_ibm(0x%lX)=%.20e \n", i,grib_long_to_ibm(i));
        printf("grib_long_to_ibm(0x%lX)=%.20e \n", i-1,grib_long_to_ibm(i-1));
        printf("grib_long_to_ibm(0x%lX)=%.20e \n", i+1,grib_long_to_ibm(i+1));
        printf("eps=%.20e\n",eps);
        printf("y=%.20e grib_ibm_to_long(y)=0x%lX grib_long_to_ibm(grib_ibm_to_long(y))=%.20e\n",y,j,z);
        printf("fabs(grib_long_to_ibm(grib_ibm_to_long(y))-y)=%.20e \n",d);
    printf("--------------------------------------------------------\n");
        fflush(stdout);
    exit(1);
  }

    for (k=1;k<3;k++) {
      if (i == 0x7FFFFFFF) continue;
      if (y>0) y+=epst;
      else y-=epst;

      /* rounding test */
      j=grib_ibm_to_long(y);
      z=grib_long_to_ibm(j);
      d=fabs(z-y);
      if ( d > epsh ) {
    printf("---------------- Rounding Failure ----------------------\n");
        printf("i=0x%lX\n",i);
        printf("grib_long_to_ibm(0x%lX)=%.20e \n", i,grib_long_to_ibm(i));
        printf("grib_long_to_ibm(0x%lX)=%.20e \n", i-1,grib_long_to_ibm(i-1));
        printf("grib_long_to_ibm(0x%lX)=%.20e \n", i+1,grib_long_to_ibm(i+1));
        printf("eps=%.20e\n",eps);
        printf("y=%.20e grib_ibm_to_long(y)=0x%lX grib_long_to_ibm(grib_ibm_to_long(y))=%.20e\n",y,j,z);
        printf("fabs(grib_long_to_ibm(grib_ibm_to_long(y))-y)=%.20e \n",d);
    printf("--------------------------------------------------------\n");
        fflush(stdout);
    exit(1);
      }

      /* nearest smaller test */
    j=grib_ibm_nearest_smaller_to_long(y);
    z=grib_long_to_ibm(j);
    d=z-y;
    if ( d > 0 ) {
    printf("---------------- Nearest smaller Failure ---------------\n");
        printf("i=0x%lX\n",i);
        printf("grib_long_to_ibm(0x%lX)=%.20e \n", i-1,grib_long_to_ibm(i-1));
        printf("grib_long_to_ibm(0x%lX)=%.20e \n", i,grib_long_to_ibm(i));
        printf("grib_long_to_ibm(0x%lX)=%.20e \n", i+1,grib_long_to_ibm(i+1));
        printf("eps=%.20e d=%.20e\n",eps,d);
        printf("                      y=%.20e grib_ibm_nearest_smaller_to_long(y)=0x%lX\n",y,j);
        printf("grib_long_to_ibm(0x%lX)=%.20e\n",j,z);
    printf("--------------------------------------------------------\n");
        fflush(stdout);
    exit(1);
    }
    if ( abs(d) > eps ) {
    printf("---------------- Nearest smaller warning ---------------\n");
        printf("i=0x%lX\n",i);
        printf("grib_long_to_ibm(0x%lX)=%.20e \n", i-1,grib_long_to_ibm(i-1));
        printf("grib_long_to_ibm(0x%lX)=%.20e \n", i,grib_long_to_ibm(i));
        printf("grib_long_to_ibm(0x%lX)=%.20e \n", i+1,grib_long_to_ibm(i+1));
        printf("eps=%.20e d=%.20e\n",eps,d);
        printf("                      y=%.20e grib_ibm_nearest_smaller_to_long(y)=0x%lX\n",y,j);
        printf("grib_long_to_ibm(0x%lX)=%.20e\n",j,z);
    printf("--------------------------------------------------------\n");
        fflush(stdout);
    }
  }

    c++;
  }
  printf("\n--- test finished ---------------------------------\n");

  return 0;
}
