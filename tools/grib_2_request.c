/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#include "grib_api_internal.h"


int main(int argc, char *argv[])
{
	grib_handle* h = NULL;
	FILE* f = NULL;
	int i = 0;
	int err = 0;
	char *mode = "file";
	char name[80];
	char value[80];


	for(i = 1; i < argc; i++)
	{

		if(argv[i][0] == '-')
		{
			mode = argv[i]+1;
			continue;
		}

		f = fopen(argv[i],"r");
		if(!f) {
			perror(argv[i]);
			exit(1);
		}

		while((h = grib_handle_new_from_file(0,f,&err)) != NULL)
		{

			grib_keys_iterator* ks  = grib_keys_iterator_new(h,GRIB_KEYS_ITERATOR_ALL_KEYS,"mars");
			while(grib_keys_iterator_next(ks))
			{
				size_t len = sizeof(value);
				char tmp[100];

				strcpy(name,grib_keys_iterator_get_name(ks));

				sprintf(tmp,"mars.%s",name);

				if((err = grib_keys_iterator_get_string(ks,value,&len)) != GRIB_SUCCESS)
				/* if(err = grib_get_string(h,tmp,value,&len)) */
				{
					fprintf(stderr,"Cannot get %s as string %d (%s)\n",name,err,
							grib_get_error_message(err));
					exit(err);
				}

				printf("%s=%s\n",name,value);
			}
			grib_keys_iterator_delete(ks);
			printf("\n");



			grib_handle_delete(h);
		}
		fclose(f);
		if(err)
		{
			fprintf(stderr,"%s\n",grib_get_error_message(err));
			exit(1);
		}
	}
	return 0;
}
