#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "grib_api.h"

int main(int argc, char *argv[])
{
	int ret=0;

	grib_handle* h;
	double missing=99999;
	double* values;
	long numberOfPointsAlongAParallel,numberOfPointsAlongAMeridian,numberOfPoints,i,j,k;
	int option_flags;

  h = grib_handle_new_from_samples(0,"bitmap");
  if(!h) {
		printf("ERROR: Unable to create grib handle\n");
		exit(1);
	}
	
	grib_set_double(h,"latitudeOfFirstGridPointInDegrees",10);
	grib_set_double(h,"longitudeOfFirstGridPointInDegrees",-10);

	grib_set_double(h,"latitudeOfLastGridPointInDegrees",-10);
	grib_set_double(h,"longitudeOfLastGridPointInDegrees",10);

	grib_set_double(h,"jDirectionIncrementInDegrees",0.5);
	grib_set_double(h,"iDirectionIncrementInDegrees",0.5);

	grib_get_long( h,"numberOfPointsAlongAParallel",&numberOfPointsAlongAParallel);
	grib_get_long( h,"numberOfPointsAlongAMeridian",&numberOfPointsAlongAMeridian);
	
	numberOfPoints=numberOfPointsAlongAMeridian*numberOfPointsAlongAMeridian;
	values=(double*)malloc(numberOfPoints*sizeof(double));
	
	grib_set_double( h,"missingValue",missing);

	for (j=0;j<numberOfPointsAlongAMeridian;j++) {
		for (i=0;i<numberOfPointsAlongAParallel;i++) {
			k=i+numberOfPointsAlongAParallel*j;
			if (i==j) values[k]=missing;
			else values[k]=50;
		}
	}
	
	grib_set_double_array( h,"values",values,numberOfPoints);
	
  if (h) { 
		grib_dump_content(h,stdout,"serialize",option_flags,NULL);
	} else  {
		printf("Error: unable to create grib_handle\n");
	}
  return ret;
	
}
