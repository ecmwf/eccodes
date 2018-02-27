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
#include <string.h>
#include <assert.h>

#include "grib_api.h"

void usage(const char *prog)
{
	fprintf(stderr,"%s: N\n",prog);
	exit(1);
}
int main(int argc, const char *argv[])
{
	double *latitudes;
	int count = 0;
	int n,i;


	if(argc != 2) usage(argv[0]);

	n = atol(argv[1]);
	count = n*2;

	latitudes = (double*)malloc(count*sizeof(double));
	assert(latitudes);

	assert(grib_get_gaussian_latitudes(n,latitudes) == 0);

	for(i = 0; i < count ; i++)
		printf("%g\n",latitudes[i]);

	return 0;
}
