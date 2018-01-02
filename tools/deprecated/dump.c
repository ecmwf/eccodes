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
#include <unistd.h>
#include <grib_api_internal.h>
#include <load.h>



int dump_values(FILE* out,grib_handle* h,const char *name,int missingOK)
{
	size_t len = 0;
	int err;
	int type;

	char *sval = NULL;
	unsigned char *uval = NULL; 
	double *dval = NULL;
	long *lval = NULL;


	int i;

	if((err = grib_get_type(h,name,&type)) != GRIB_SUCCESS)
	{
		printf("# Oops... cannot get type of [%s]: %s\n",name,
				grib_get_error_message(err));
		return err;
	}

	if((err = grib_get_size(h,name,&len)) != GRIB_SUCCESS)
	{
		printf("# Oops... cannot get size of [%s]: %s\n",name,
				grib_get_error_message(err));
		return err;
	}

	switch(type)
	{
		case GRIB_TYPE_STRING:

			sval = grib_context_malloc(h->context,len*sizeof(char));

			if((err = grib_get_string(h,name,sval,&len)) != GRIB_SUCCESS)
			{
				printf("# Oops... cannot get string value of [%s]: %s\n",
						name,grib_get_error_message(err));
			}
			else
			{
				fprintf(out,"%s='%s';\n",name,sval);
			}

			grib_context_free(h->context,sval);

			if(err) return err;

			break;

		case GRIB_TYPE_LONG:
			lval = grib_context_malloc(h->context,len*sizeof(long));

			if((err = grib_get_long_array(h,name,lval,&len)) != GRIB_SUCCESS)
			{
				printf("# Oops... cannot get long value of [%s]: %s\n",
						name,grib_get_error_message(err));
			}
			else
			{
				if(len == 1)
				{
					if(missingOK && lval[0] == GRIB_MISSING_LONG)
						fprintf(out,"%s = missing;\n",name);
					else
						fprintf(out,"%s = %ld;\n",name,lval[0]);
				}
				else {
					fprintf(out,"%s = {", name);
					if(len >= 10) fprintf(out," # %ld\n",(long)len);
					for(i = 0; i < len; i++){
						fprintf(out,"%ld, ",lval[i]);
						if((i+1)%10 == 0)
							fprintf(out,"\n");
					}
					fprintf(out,"};\n");
				}
			}

			grib_context_free(h->context,lval);

			if(err) return err;
			break;

		case GRIB_TYPE_DOUBLE:
			dval = grib_context_malloc(h->context,len*sizeof(double));

			if((err = grib_get_double_array(h,name,dval,&len)) != GRIB_SUCCESS)
			{
				printf("# Oops... cannot get double value of [%s]: %s\n",
						name,grib_get_error_message(err));
			}
			else
			{
				if(len == 1)
				{
					if(missingOK && dval[0] == GRIB_MISSING_DOUBLE)
						fprintf(out,"%s = missing;\n",name);
					else {
						fprintf(out,"%s = %g;\n",name,dval[0]);
					}
				}
				else {
					fprintf(out,"%s = { \n",name);
					if(len >= 10) fprintf(out," # %ld\n",(long)len);
					for(i = 0; i < len; i++)
					{
						fprintf(out,"%f, ",dval[i]);
						if((i+1)%10 == 0)
							fprintf(out,"\n");
					}
					fprintf(out,"};\n");
				}
			}

			grib_context_free(h->context,dval);

			if(err) return err;
			break;

		case GRIB_TYPE_BYTES:
			uval = grib_context_malloc(h->context,len*sizeof(unsigned char));

			if((err = grib_get_bytes(h,name,uval,&len)) != GRIB_SUCCESS)
			{
				printf("# Oops... cannot get bytes value of [%s]: %s\n",
						name,grib_get_error_message(err));
			}
			else
			{
				fprintf(out,"%s = { \n",name);
					if(len >= 10) fprintf(out," # %ld\n",(long)len);
					for(i = 0; i < len; i++)
					{
						fprintf(out,"%02x, ",uval[i]);
						if((i+1)%10 == 0)
							fprintf(out,"\n");
					}
					fprintf(out,"};\n");

			}

			grib_context_free(h->context,uval);

			if(err) return err;

			break;

		case GRIB_TYPE_LABEL:
			break;

		case GRIB_TYPE_SECTION:
			break;

		default:
			printf("Cannot compare [%s], unsupported type %d\n",name,type);
			return GRIB_UNABLE_TO_COMPARE_ACCESSORS;
			break;
	}

	return GRIB_SUCCESS;

}

static int dump_handle(FILE* out,grib_handle* h)
{
	grib_keys_iterator* iter  = grib_keys_iterator_new(h,
		GRIB_KEYS_ITERATOR_SKIP_READ_ONLY|GRIB_KEYS_ITERATOR_SKIP_DUPLICATES,NULL);

	while(grib_keys_iterator_next(iter))
	{
		grib_accessor* a = grib_keys_iterator_get_accessor(iter);
		dump_values(out,h,grib_keys_iterator_get_name(iter),
			a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING);
	}

	grib_keys_iterator_delete(iter);

	return 0;
}

int dump_file(const char* file,const char* out)
{
	FILE *f = fopen(file,"r");
	FILE *o = fopen(out,"w");
	int e = 0;
	grib_handle *h;
	const void* b;
	size_t s;
	int count = 0;
	char identifier[10];
	size_t size = sizeof(identifier);

	if(!f) {
		perror(file);
		exit(1);
	}

	h = grib_handle_new_from_file(NULL,f,&e);

	while(h)
	{
		GRIB_CHECK(grib_get_message(h,&b,&s),file);
		GRIB_CHECK(grib_get_string(h,"identifier",identifier,&size),file);

		++count;



		fprintf(o,"%s { # %d\n",identifier,count);
		dump_handle(o,h);
		fprintf(o,"};\n");

		grib_handle_delete(h);

		e = 0;

		h = grib_handle_new_from_file(NULL,f,&e);
	}


	GRIB_CHECK(e,file);

	return 0;

}

