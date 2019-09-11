/*
 * C Implementation: create_grib
 *
 * Description:
 *
 *
 * Author: Enrico Fucile <enrico.fucile@ecmwf.int>, (C) 2006
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include <stdio.h>

#include "grib_api_internal.h"

int main(int argc, char *argv[])
{

  FILE* of=NULL;
  const void* buffer=NULL;
  int ret=0;
  size_t size;
  double* values=NULL;
  long numberOfPointsAlongAParallel=0;
  long numberOfPointsAlongAMeridian=0;
  long numberOfPoints = 0,i = 0,j = 0,k = 0;
  grib_context* c=NULL;
  grib_handle* h=NULL;

  char* outfilename="out.grib";
  long missing=9999;
#if 0
  FILE *f=NULL;
  char* infilename="../data/sample_grib.txt";
  f = fopen(infilename,"r");
  if(!f) {
    perror(argv[i]);
    exit(1);
  }

  h=grib_handle_new_from_text_file(c,f);

  fclose(f);
#endif

#if 0
  FILE *f=NULL;
  char* infilename="../data/sample_grib.txt";
  f = fopen(infilename,"r");
  if(!f) {
    perror(argv[i]);
    exit(1);
  }

    h=grib_handle_new(c);
  GRIB_CHECK(grib_load_from_text_file(h,f),0);

  fclose(f);
#endif

#if 1
  h=grib_handle_new(c);
  GRIB_CHECK(grib_set_type(h,"grib1"),0);
  GRIB_CHECK(grib_set_type(h,"ecmwf"),0);
  GRIB_CHECK(grib_set_type(h,"geopotential"),0);
#endif

  GRIB_CHECK(grib_set_long(h,"generatingProcessIdentifier",127),0);

  GRIB_CHECK(grib_set_double(h,"latitudeOfFirstGridPointInDegrees",10),0);
  GRIB_CHECK(grib_set_double(h,"longitudeOfFirstGridPointInDegrees",-10),0);

  GRIB_CHECK(grib_set_double(h,"latitudeOfLastGridPointInDegrees",-10),0);
  GRIB_CHECK(grib_set_double(h,"longitudeOfLastGridPointInDegrees",10),0);

  GRIB_CHECK(grib_set_double(h,"jDirectionIncrementInDegrees",0.5),0);
  GRIB_CHECK(grib_set_double(h,"iDirectionIncrementInDegrees",0.5),0);

  GRIB_CHECK(grib_set_long(h,"section3Present",1),0);
  GRIB_CHECK(grib_set_long(h,"missingValue",missing),0);

  GRIB_CHECK(grib_get_long( h,"numberOfPointsAlongAParallel",&numberOfPointsAlongAParallel),0);
  GRIB_CHECK(grib_get_long( h,"numberOfPointsAlongAMeridian",&numberOfPointsAlongAMeridian),0);

  numberOfPoints=numberOfPointsAlongAMeridian*numberOfPointsAlongAParallel;
  values=(double*)malloc(numberOfPoints*sizeof(double));


  for (j=0;j<numberOfPointsAlongAMeridian;j++) {
    for (i=0;i<numberOfPointsAlongAParallel;i++) {
      k=i+numberOfPointsAlongAParallel*j;
      if (i==j) values[k]=missing;
      else values[k]=50;
    }
  }

  grib_set_data_array( h,values,numberOfPoints);

  if (h) {
    grib_dump_content(h,stdout,"debug",0,NULL);
  } else  {
    printf("Error: unable to create grib_handle\n");
  }

  of = fopen(outfilename,"w");
  if(!of) {
    perror(outfilename);
    exit(1);
  }

  grib_get_message(h,&buffer,&size);

  if(fwrite(buffer,1,size,of) != size) {
    perror(outfilename);
    exit(1);
  }

  fclose(of);

  return ret;

}
