/*
 * C Implementation: gaussian
 *
 * Description: example to get the gaussian latitudes given the
 *              order of the grid
 *
 *
 * Author: Enrico Fucile <enrico.fucile@ecmwf.int>, (C) 2006
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "grib_api.h"
#include "gaussian.h"

void usage(char* progname) {
  printf("\nUsage: %s [-r] [-f lat/lon] order\n",progname);
  exit(1);
}

int grib_gaussian_grid(int reduced,int order,int** npoints,double* latitude) {
  int i=0;
  int rg_idx=-1;
  while (i < GRIB_MAX_RG_GRIDS) {
    if (grib_rg_idx[i] == order) {
      rg_idx=i;
      break;
    }
    i++;
  }

  if ( rg_idx == -1 ) return 1;
  *npoints=grib_rg[rg_idx];

  GRIB_CHECK(grib_get_gaussian_latitudes(order, latitude),0);

  return 0;
}

static int find_nearest_in_array(double val,double* vals,int ilast,int* i1,int* i2) {
  int nstep=0;
  int count=1;

  if (vals[0] < val || vals[ilast] > val ) {
    return GRIB_GEOCALCULUS_PROBLEM;
  }

  if (*i1 < 0 || *i1 > ilast || vals[*i1] > val) *i1=0; 
  if (*i2 < 0 || *i2 > ilast || vals[*i2] < val) *i2=ilast; 

  nstep=(*i2-*i1)/2;
  while (nstep >= 1) {
    if ( vals[*i1+nstep] > val ) {
	  *i1+=nstep;
	} else {
	  *i2-=nstep;
	}
    nstep=(*i2-*i1)/2;
    printf("----- iteration %d -- val=%g -- val[%d]=%g  val[%d]=%g\n",count,val,*i1,vals[*i1],*i2,vals[*i2]);
	count++;
  }
   
  return 0;
}

int grib_gaussian_find_nearest(int reduced,int order,int* npoints,double* lats,double lat,double lon,int* n,int* nearest_idx) {
  int ilat1=-1;
  int ilat2=-1;
  int ilatlast;
  double lonstep1,lonstep2;
  int ilon1,ilon2;
  int ret;
  int npoints1,npoints2;

  ilatlast=2*order-1;

  if ((ret=find_nearest_in_array(lat,lats,ilatlast,&ilat1,&ilat2)) != GRIB_SUCCESS ) 
    return ret;
  
  printf("------------ nearest of lat=%g -- lat[%d]=%g  lat[%d]=%g \n",lat,ilat1,lats[ilat1],ilat2,lats[ilat2]);

  if (ilat1 <= order) npoints1=npoints[ilat1];
  else npoints1=npoints[2*order-ilat1-1];

  lonstep1=360.0/npoints1;
  ilon1=floor(lon/lonstep1);

  if (ilat2 <= order) npoints2=npoints[ilat2];
  else npoints2=npoints[2*order-ilat2-1];

  lonstep2=360.0/npoints2;
  ilon2=floor(lon/lonstep2);
  
  printf("------------ nearest of lon=%g -- (%d,%d) lon=%g  (step=%g npoints=%d) - (%d,%d) lon=%g  (step=%g npoints=%d)\n",
		 lon,
         ilat1,ilon1,ilon1*lonstep1,lonstep1,npoints1,
         ilat2,ilon2,ilon2*lonstep2,lonstep2,npoints2);

  return 0;
}


int main(int argc, char *argv[])
{
  double* lats;
  long order=0;
  int i;
  int c;
  int reduced=0;
  int findnearest=0;
  int* npoints=0;
  char* pc=0;
  double lat=0,lon=0;
  int debug=0;
  int n=0;
  int* nearest_idx;
  int tot_npoints=0;


  while ((c = getopt (argc, argv, "drf:")) != -1) {
    switch (c) {
	   case 'r':
	      reduced = 1;
	      break;
	   case 'd':
	      debug=1;
	      break;
	   case 'f':
	      findnearest = 1;
		  pc=optarg;
          while (*pc != '\0' && *pc != '/') pc++;
		  if (*pc == '\0' || *(pc+1) == '\0' ) usage(argv[0]);
		  *pc='\0';
		  pc++;
		  lat=atof(optarg);
		  lon=atof(pc);
	      break;
	   default:
	      usage (argv[0]);
    }
  }

  if (argc-optind != 1 ) usage(argv[0]);

  order=atoi(argv[optind]);

  lats   = (double*)malloc(order*2*sizeof(double));

  if (grib_gaussian_grid(reduced,order,&npoints,lats) != 0) {
     if (reduced) printf("Error: unable to build the reduced gaussian grid %d\n",(int)order);
     else printf("Error: unable to build the gaussian grid %d\n",(int)order);
  }
  
  n=4;
  nearest_idx=(int*)malloc(n*sizeof(int));
  if ( grib_gaussian_find_nearest(reduced,order,npoints,lats,lat,lon,&n,nearest_idx) != GRIB_SUCCESS  ) {
     printf("Error: unable to find the %d nearest points of lat=%g lon=%g\n",n,lat,lon);
  }

  if (reduced) printf("--- Reduced Gaussian grid %d ---\n",(int)order);
  else printf("--- Gaussian grid %d ---\n",(int)order);

  if (reduced) {
    printf("Latitude  Number     Latitude\n"); 
    printf("number    of points\n"); 
	tot_npoints=0;
    for (i=0; i< order; i++) { 
       printf("%-*d %-*d %2.5f\n",9,i,10,npoints[i],lats[i]);  
	   tot_npoints+=npoints[i];
	}
    for (i=order; i< 2*order; i++) {
       printf("%-*d %-*d %2.5f\n",9,i,10,npoints[2*order-i-1],lats[i]);  
	   tot_npoints+=npoints[2*order-i-1];
	}
	printf("Total Number Of Points = %d\n",tot_npoints);
  } else {
    printf("Latitude  Latitude\n");
    printf("number   \n"); 
    for ( i=0; i < 2*order; i++) 
      printf("%-*d %2.5f\n",9,i,lats[i]);
  }

  printf("\n");

  return 0;

}


