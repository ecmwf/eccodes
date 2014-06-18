/*
 * Copyright 2005-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api.h"

int main(int argc, char* argv[])
{
	FILE* f;
	int err=0;
	char infile[]="../../data/reduced_gaussian_model_level.grib1";
	grib_handle *h=NULL;
	grib_box* box;
	grib_points* points;
	grib_context* c=grib_context_get_default();
	double north,west,south,east;
	double* val;
	int i;

	north=60;
	south=10;
	west=-10;
	east=30;

	f=fopen(infile,"r");
	if (!f) {
		perror(infile);
		exit(1);
	}

	h=grib_handle_new_from_file(c,f,&err);
	if (!h) {
		printf("unable to create handle from file %s\n",infile);
		exit(err);
	}

	box=grib_box_new(h,&err);

	if (!box) {
		printf("unable to create box\n");
		exit(err);
	}

	points=grib_box_get_points(box,north,west,south,east,&err);

	val=malloc(sizeof(double)*points->n);
	grib_points_get_values(h,points,val);

	for (i=0;i<points->n;i++) {
		printf("%d -- %.3f %.3f %ld %g\n",i,
			points->latitudes[i], points->longitudes[i], (long)(points->indexes[i]), val[i]);
	}

	free(val);

	fclose(f);
	return 0;

}
