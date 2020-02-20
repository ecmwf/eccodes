#include <assert.h>
#include <stdio.h>
#include <unistd.h>
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
  char* filename = "../data/sample1.grib";
  grib_handle *h = NULL;

  in = fopen(filename,"r");
  if(!in) {
    printf("ERROR: unable to open file %s\n",filename);
    return 1;
  }

  GRIB_CHECK(h = grib_handle_new_from_file(0,in,&err),0);

  GRIB_CHECK(grib_get_long(h,"numberOfPointsAlongAParallel",&numberOfPointsAlongAParallel),0);
  printf("numberOfPointsAlongAParallel=%ld\n",numberOfPointsAlongAParallel);

  GRIB_CHECK(grib_get_long(h,"numberOfPointsAlongAMeridian",&numberOfPointsAlongAMeridian),0);
  printf("numberOfPointsAlongAMeridian=%ld\n",numberOfPointsAlongAMeridian);

  GRIB_CHECK(grib_get_double(h,"latitudeOfFirstGridPointInDegrees",&latitudeOfFirstGridPointInDegrees),0);
  printf("latitudeOfFirstGridPointInDegrees=%g\n",latitudeOfFirstGridPointInDegrees);

  GRIB_CHECK(grib_get_double(h,"longitudeOfFirstGridPointInDegrees",&longitudeOfFirstGridPointInDegrees),0);
  printf("longitudeOfFirstGridPointInDegrees=%g\n",longitudeOfFirstGridPointInDegrees);

  GRIB_CHECK(grib_get_double(h,"latitudeOfLastGridPointInDegrees",&latitudeOfLastGridPointInDegrees),0);
  printf("latitudeOfLastGridPointInDegrees=%g\n",latitudeOfLastGridPointInDegrees);

  GRIB_CHECK(grib_get_double(h,"longitudeOfLastGridPointInDegrees",&longitudeOfLastGridPointInDegrees),0);
  printf("longitudeOfLastGridPointInDegrees=%g\n",longitudeOfLastGridPointInDegrees);

  GRIB_CHECK(grib_get_double(h,"jDirectionIncrementInDegrees",&jDirectionIncrementInDegrees),0);
  printf("jDirectionIncrementInDegrees=%g\n",jDirectionIncrementInDegrees);

  GRIB_CHECK(grib_get_double(h,"iDirectionIncrementInDegrees",&iDirectionIncrementInDegrees),0);
  printf("iDirectionIncrementInDegrees=%g\n",iDirectionIncrementInDegrees);

  GRIB_CHECK(grib_get_size(h,"values",&values_len),0);

  values = malloc(values_len*sizeof(double));

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
