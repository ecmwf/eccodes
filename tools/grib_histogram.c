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

#include "grib_api.h"

static void usage(const char *prog)
{
	fprintf(stderr,"%s:  in1 [in2 ... ]  out\n",prog);
	exit(1);
}

grib_values values[10240];

int main(int argc, const char *argv[])
{
	int i,j;
	FILE *in;
	int e;
	grib_handle *h;
	double* values = NULL;
	size_t size;
	size_t last_size = 0;
	double min,max;
	long count = 10;
	long intervals[10];
	const char *names[1024];
	int name_count = 0;

	if(argc < 2) usage(argv[0]);


	for(i = 1; i < argc; i++)
	{

		if(argv[i][0] == '-') {
			names[name_count++] = argv[i]+1;	
			continue;
		}

		in = fopen(argv[i],"r");
		if(!in) {
			perror(argv[i]);
			exit(1);
		}

		while( (h = grib_handle_new_from_file(NULL,in,&e)) != NULL )
		{
			long bitmap;
			double delta;
			GRIB_CHECK(grib_get_long(h,"bitMapIndicator",&bitmap),argv[i]);


			GRIB_CHECK(grib_get_size(h,"values",&size),argv[i]);

			if(size > last_size)
			{
				if(values)
					free(values);
				values = (double*)malloc(size*sizeof(double));
				last_size = size;
				assert(values);
			}

			GRIB_CHECK(grib_get_double_array(h,"values",values,&size),argv[i]);

			for(j = 0; j < count; j++)
				intervals[j] = 0;


			if(bitmap != 255)
			{

				double missing;
				GRIB_CHECK(grib_get_double(h,"missingValue",&missing),argv[i]);

				min = max = missing;

				for(j = 0; j < size ; j++)
				{
					if((min == missing) || ((values[j] != missing) && (min>values[j]))) min = values[j];
					if((max == missing) || ((values[j] != missing) && (max<values[j]))) max = values[j];
				}	

				delta = max- min; if(delta == 0) delta = 1;

				for(j = 0; j < size; j++)
				{
					if(values[j] != missing)
					{
						int x = (values[j] - min)/delta * count;
					        if(x==count) x=x-1; /*handle the absolute  maximum*/
						intervals[x]++;
					}
				}

			}
			else
			{
				min = max = values[0];
				for(j = 1; j < size ; j++)
				{
					if(min>values[j]) min = values[j];
					if(max<values[j]) max = values[j];
				}

				delta = max- min; if(delta == 0) delta = 1;

				for(j = 0; j < size; j++)
				{
					int x = (values[j] - min)/delta * count;
					if(x==count) x=x-1; /*handle the absolute  maximum*/
					intervals[x]++;
				}
			}


			for(j = 0; j < name_count; j++)
			{
				char v[1024]; size_t s = sizeof(v);
				GRIB_CHECK(grib_get_string(h,names[j],v,&s),names[j]);
				printf(" %s",v);
			}

			printf("\nmin=%g max=%g size=%ld\n",min,max,(long)size);
			for(j = 0; j < count; j++)
				printf(" %g:%g",
						j*(max-min)/count + min
						,
						(j+1)*(max-min)/count + min
					  );
			printf("\n");

			for(j = 0; j < count; j++)
				printf(" %ld",intervals[j] );
			printf("\n");

		}

		GRIB_CHECK(e,argv[i]);
		fclose(in);
	}

	return 0;
}
