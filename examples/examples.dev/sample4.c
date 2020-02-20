#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "grib_api.h"



int main(int argc, char *argv[])
{
	int ret=0;
	FILE* f;
	size_t size;
	grib_handle* h=NULL;
	double* values=NULL;
	long numberOfPointsAlongAParallel,numberOfPointsAlongAMeridian,numberOfPoints,i,j,k;
	char* filename="out.grib";
	const void* buffer=NULL;

	/* Create a grib handle from template*/
	h = grib_handle_new_from_template(0,"GRIB1");
	if(!h) {
		printf("ERROR: Unable to create grib handle\n");
		exit(1);
	}
	
	/* set values as appropriate in the grib handle header */
	CHECK(grib_set_long(h,"gribTablesVersionNo", 210));
	CHECK(grib_set_long(h,"indicatorOfParameter",80 ));
	
	CHECK(grib_set_long(h,"indicatorOfTypeOfLevel",1 ));
	CHECK(grib_set_long(h,"topLevel",0));
  CHECK(grib_set_long(h,"bottomLevel",0));
	
	CHECK(grib_set_long(h,"dataDate",20021201 ));
	CHECK(grib_set_long(h,"dataTime",1200 ));

	CHECK(grib_set_double(h,"latitudeOfFirstGridPointInDegrees",10));
	CHECK(grib_set_double(h,"longitudeOfFirstGridPointInDegrees",-10));

	CHECK(grib_set_double(h,"latitudeOfLastGridPointInDegrees",-10));
	CHECK(grib_set_double(h,"longitudeOfLastGridPointInDegrees",10));

	CHECK(grib_set_double(h,"jDirectionIncrementInDegrees",0.5));
	CHECK(grib_set_double(h,"iDirectionIncrementInDegrees",0.5));

	
	/* get number of points calculated automatically by the grib_api */
	CHECK(grib_get_long( h,"numberOfPointsAlongAParallel",&numberOfPointsAlongAParallel));
	CHECK(grib_get_long( h,"numberOfPointsAlongAMeridian",&numberOfPointsAlongAMeridian));

	numberOfPoints=numberOfPointsAlongAMeridian*numberOfPointsAlongAMeridian;
	
	/* allocate the values array */
	values=(double*)malloc(numberOfPoints*sizeof(double));

	/* load the values array */	
	for (j=0;j<numberOfPointsAlongAMeridian;j++) {
		for (i=0;i<numberOfPointsAlongAParallel;i++) {
			k=i+numberOfPointsAlongAParallel*j;
			values[k]=50;
		}
	}

	/* set values in the grib */
	grib_set_double_array( h,"values",values,numberOfPoints);

	/* write grib to file */
	f = fopen(filename,"w");
  if(!f) {
      perror(filename);
      exit(1);
  }

  grib_get_message(h,&buffer,&size);
  
	if(fwrite(buffer,1,size,f) != size) {
      perror(filename);
      exit(1);
  }

  if(fclose(f)) {
        perror(filename);
        exit(1);
	}
	
	grib_handle_delete( h);
	
	return ret;

}
