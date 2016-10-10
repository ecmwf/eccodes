#include "test.h"

char buffer[1024*1024*80];

int main(int argc,char** argv)
{
	long length = 0;
	FILE *f;
	grib_handle  *g =  0;
	int err;
	int cnt = 0;
	int sample = getenv("SAMPLE") != NULL;
	extern const char* grib_get_accessor_class_name(grib_handle* , const char* );

	char *path = argc > 1 ? argv[1]: "data";

	f = fopen(path ,"r");
	if(!f)
	{
		perror(path);
		exit(10);
	}


	length = sizeof(buffer);


	while((err = readany(f,buffer,&length)) == 0)
	{
		const char* result_class_name;
		size_t off = 0;

		if(strncmp(buffer,"GRIB",4) != 0)
		{
			cnt++;
			length = sizeof(buffer);
			continue;
		}

		g = grib_handle_new_from_message(0,buffer,length);

		result_class_name = grib_get_accessor_class_name(g,"codedValues");
		if(!result_class_name)
			result_class_name = grib_get_accessor_class_name(g,"values");

		fprintf(stderr,"Value class is %s\n",result_class_name?result_class_name:"NONE");

		if(result_class_name && (strcmp(result_class_name,"data_2order_packing") == 0 ||
		
			strcmp(result_class_name,"data_g22order_packing") == 0)
		
		)
		{
			cnt++;
			printf("Field %d ignored\n",cnt);
			grib_handle_delete(g);
			length = sizeof(buffer);
			continue;
		}

		if((err = grib_get_offset(g,"endOfHeadersMarker",&off)))
		{
			fprintf(stderr,"Cannot get values endOfHeadersMarker  %d\n",err);
			exit(11);
		}


		if(sample) save_message(g,"values.grib");

		if(compare_with_gribex(g))	
			exit(11);


		cnt++;
		printf("Field %d OK\n",cnt);
		grib_handle_delete(g);

		length = sizeof(buffer);
	}

	if(err != -1)
	{
		fprintf(stderr,"readany error %d\n",err);
		exit(17);
	}

	if(cnt == 0)
	{
		fprintf(stderr,"No GRIB processed\n");
		exit(18);
	}


	exit(0);
}
