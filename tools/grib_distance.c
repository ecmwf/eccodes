/*
 * Copyright 2005-2016 ECMWF.
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
#include <math.h>
#include <stdlib.h>


#include "grib_api.h"

void usage(const char *prog)
{
	fprintf(stderr,"%s:  in1 [in2 ... ]  out\n",prog);
	exit(1);
}

int compare(const void *a, const void *b)
{
	double da = *(double*)a;
	double db = *(double*)b;
	return da == db ? 0 : ( da > db ? 1 : -1 ); 
}

int main(int argc, const char *argv[])
{
	int i;
	FILE *in1,*in2;
	int e;
	grib_handle *h1,*h2;
	double* v1,*v2 = NULL;
	size_t s1,s2;
	double d = 0;
#if 0
	double d1 = 0;
	double d2 = 0;
#endif

	if(argc < 3) usage(argv[0]);

	in1 = fopen(argv[1],"r");
	if(!in1) {
		perror(argv[1]);
		exit(1);
	}

	in2 = fopen(argv[2],"r");
	if(!in2) {
		perror(argv[2]);
		exit(2);
	}

	if((h1 = grib_handle_new_from_file(NULL,in1,&e)) == NULL )
	{
		if(e == 0) e = -1; 
		GRIB_CHECK(e,argv[1]);
		exit(1);
	}

	if((h2 = grib_handle_new_from_file(NULL,in2,&e)) == NULL )
	{
		if(e == 0) e = -1; 
		GRIB_CHECK(e,argv[2]);
		exit(1);
	}

	GRIB_CHECK(grib_get_size(h1,"values",&s1),argv[1]);
	GRIB_CHECK(grib_get_size(h2,"values",&s2),argv[2]);

	/*printf("s1=%ld s2=%ld\n",(long)s1,(long)s2);*/
	assert(s1 == s2);

	v1 = (double*)calloc(s1,sizeof(double));
	assert(v1);
	v2 = (double*)calloc(s2,sizeof(double));
	assert(v2);

	GRIB_CHECK(grib_get_double_array(h1,"values",v1,&s1),argv[1]);
	GRIB_CHECK(grib_get_double_array(h2,"values",v2,&s2),argv[2]);

	if(0)
	{
		qsort(v1,sizeof(double),s1,compare);
		qsort(v2,sizeof(double),s2,compare);

		/* http://people.revoledu.com/kardi/tutorial/Similarity/index.html */

		/* http://people.revoledu.com/kardi/tutorial/Similarity/BrayCurtisDistance.html */
		/*  Bray Curtis Distance, or Sorensen */

		for(i = 0; i < s1; i++)
		{
			double s = v1[i]-v2[i];
			d += s * s;
		}
		printf("%g\n",sqrt(d/s1));
	}

	if(0) {
		double d1 = 0, d2 = 0;
		for(i = 0; i < s1; i++)
		{
			if(i) assert(v1[i] >= v1[i-1]);
			if(i) assert(v2[i] >= v2[i-1]);

			d  += fabs(v1[i]-v2[i]);
			d1 += v1[i];
			d2 += v2[i];
		}
		printf("%g\n",d/(d1+d2));
	}

	if(1) 
	{
		double d1 = 0;
		double d2 = 0;
		for(i = 0; i < s1; i++)
		{
			d1 += v1[i] * v1[i];
			d2 += v2[i] * v2[i];
		}

		d1 = d1*d1;
		d2 = d2*d2;

		if(d1 == 0) d1 = 1;
		if(d2 == 0) d2 = 1;

		d  = 0;
		for(i = 0; i < s1; i++)
		{
			double x = v1[i]/d1 - v2[i]/d2;
			d += x * x;
		}

		printf("%g\n",sqrt(d));

	}

	if(0)
	{
		/* http://people.revoledu.com/kardi/tutorial/Similarity/CanberraDistance.html */
		d = 0;
		for(i = 0; i < s1; i++)
		{
			if(v1[i] == 0 && v2[i] == 0) 
				d += 0.0;
			else
				d += fabs(v1[i]-v2[i])/(fabs(v1[i]) + fabs(v2[i]));
		}

		/*printf("%g\n",d/s1);*/
		printf("%g\n",d);
	}

	return 0;
}

