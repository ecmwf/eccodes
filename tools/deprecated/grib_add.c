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
	fprintf(stderr,"%s:  in1 [in2 ... ]  out\n",prog);
	exit(1);
}

grib_values values[10240];

int main(int argc, const char *argv[])
{
	int i,j;
	FILE *in,*out;
	int e;
	grib_handle *result = NULL,*h;
	double* values = NULL;
	double *tmp = NULL;
	size_t size,count;

	if(argc < 3) usage(argv[0]);


	out = fopen(argv[argc-1],"w");
	if(!out) {
		perror(argv[argc-1]);
		exit(1);
	}


	for(i = 1; i < argc-1; i++)
	{
		in = fopen(argv[i],"r");
		if(!in) {
			perror(argv[i]);
			exit(1);
		}

		while( (h = grib_handle_new_from_file(NULL,in,&e)) != NULL )
		{

			if(result == NULL)
			{

				GRIB_CHECK(grib_get_size(h,"values",&size),argv[i]);

				values = (double*)calloc(size,sizeof(double));
				assert(values);

				tmp    = (double*)calloc(size,sizeof(double));
				assert(tmp);

				result = h;
			}

			GRIB_CHECK(grib_get_size(h,"values",&count),argv[i]);
			assert(count == size);

			GRIB_CHECK(grib_get_double_array(h,"values",tmp,&count),argv[i]);
			assert(count == size);

			for(j = 0; j < count; j++)
				values[j] += tmp[j];


			if(h != result)
				grib_handle_delete(h);
		}

		GRIB_CHECK(e,argv[argc-2]);
	}

	if(result)
	{
		const void *buffer; 
		GRIB_CHECK(grib_set_double_array(result,"values",values,size),argv[i]);
		GRIB_CHECK(grib_get_message(result,&buffer,&size),argv[0]);

		if(fwrite(buffer,1,size,out) != size)
		{
			perror(argv[argc-1]);
			exit(1);
		}

		grib_handle_delete(result);
	}


	if(fclose(out))
	{
		perror(argv[argc-1]);
		exit(1);
	}


	return 0;
}
