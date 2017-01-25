/*
 * Copyright 2005-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: get
 *
 * Description: how to get values using keys.
 *
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "grib_api.h"

int main(int argc, char** argv) {
  int err = 0;
  double *values = NULL;
  size_t values_len= 0;

  size_t i = 0;

  double latitudeOfFirstGridPointInDegrees;
  double longitudeOfFirstGridPointInDegrees;
  double latitudeOfLastGridPointInDegrees;
  double longitudeOfLastGridPointInDegrees;

  double jDirectionIncrementInDegrees;
  double iDirectionIncrementInDegrees;

  long numberOfPointsAlongAParallel;
  long numberOfPointsAlongAMeridian;

  double average = 0;

  FILE* in = NULL;
  char* filename = "../data/regular_latlon_surface.grib1";
  grib_handle *h = NULL;

  in = fopen(filename,"r");
  if(!in) {
    printf("ERROR: unable to open file %s\n",filename);
    return 1;
  }

  /* create new handle from a message in a file*/
  h = grib_handle_new_from_file(0,in,&err);
  if (h == NULL) {
    printf("Error: unable to create handle from file %s\n",filename);
  }

  /* get as a long*/
  GRIB_CHECK(grib_get_long(h,"numberOfPointsAlongAParallel",&numberOfPointsAlongAParallel),0);
  printf("numberOfPointsAlongAParallel=%ld\n",numberOfPointsAlongAParallel);

  /* get as a long*/
  GRIB_CHECK(grib_get_long(h,"numberOfPointsAlongAMeridian",&numberOfPointsAlongAMeridian),0);
  printf("numberOfPointsAlongAMeridian=%ld\n",numberOfPointsAlongAMeridian);

  /* get as a double*/
  GRIB_CHECK(grib_get_double(h,"latitudeOfFirstGridPointInDegrees",&latitudeOfFirstGridPointInDegrees),0);
  printf("latitudeOfFirstGridPointInDegrees=%g\n",latitudeOfFirstGridPointInDegrees);

  /* get as a double*/
  GRIB_CHECK(grib_get_double(h,"longitudeOfFirstGridPointInDegrees",&longitudeOfFirstGridPointInDegrees),0);
  printf("longitudeOfFirstGridPointInDegrees=%g\n",longitudeOfFirstGridPointInDegrees);

  /* get as a double*/
  GRIB_CHECK(grib_get_double(h,"latitudeOfLastGridPointInDegrees",&latitudeOfLastGridPointInDegrees),0);
  printf("latitudeOfLastGridPointInDegrees=%g\n",latitudeOfLastGridPointInDegrees);

  /* get as a double*/
  GRIB_CHECK(grib_get_double(h,"longitudeOfLastGridPointInDegrees",&longitudeOfLastGridPointInDegrees),0);
  printf("longitudeOfLastGridPointInDegrees=%g\n",longitudeOfLastGridPointInDegrees);

  /* get as a double*/
  GRIB_CHECK(grib_get_double(h,"jDirectionIncrementInDegrees",&jDirectionIncrementInDegrees),0);
  printf("jDirectionIncrementInDegrees=%g\n",jDirectionIncrementInDegrees);

  /* get as a double*/
  GRIB_CHECK(grib_get_double(h,"iDirectionIncrementInDegrees",&iDirectionIncrementInDegrees),0);
  printf("iDirectionIncrementInDegrees=%g\n",iDirectionIncrementInDegrees);

  /* get the size of the values array*/
  GRIB_CHECK(grib_get_size(h,"values",&values_len),0);

  values = malloc(values_len*sizeof(double));

  /* get data values*/
  GRIB_CHECK(grib_get_double_array(h,"values",values,&values_len),0);

  average = 0;
  for(i = 0; i < values_len; i++)
    average += values[i];

  average /=(double)values_len;

  free(values);

  printf("There are %d values, average is %g\n",(int)values_len,average);

  grib_handle_delete(h);

  fclose(in);
  return 0;
}
