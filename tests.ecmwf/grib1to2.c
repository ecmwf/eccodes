#include "test.h"

char buffer[1024*1024*80];

int main(int argc,char** argv)
{
	long length = 0;
	FILE *f;
	grib_handle  *g =  0;
	grib_handle  *h =  0;
	int err;
	int cnt = 0;
	extern const char* grib_get_accessor_class_name(grib_handle* , const char* );
	int sample = getenv("SAMPLE") != NULL;
	
	f = fopen(argc > 1 ? argv[1]: "data" ,"r");
	if(!f)
	{
		perror(argc > 1 ? argv[1]: "data");
		exit(10);
	}

	length = sizeof(buffer);
	while((err = readany(f,buffer,&length)) == 0)
	{
		const char *result_class_name;
		if(strncmp(buffer,"GRIB",4) != 0)
		{
			cnt++;
			length = sizeof(buffer);
			continue;
		}

		g = grib_handle_new_from_message(0,buffer,length);

		if(sample) save_message(g,"grib1to2.orig");

		h = grib_handle_clone(g);

		result_class_name = grib_get_accessor_class_name(g,"codedValues");
		if(!result_class_name)
			result_class_name = grib_get_accessor_class_name(g,"values");

		fprintf(stderr,"Value class is %s\n",result_class_name?result_class_name:"NONE");

#if 0
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
#endif

		grib_set_long(h,"editionNumber",2);

		if(sample) save_message(h,"grib1to2.data");

		if(compare_with_handle(g,h))
		        exit(99);


#if 0
		grib_set_long(g,"editionNumber",1);

		if(compare_with_gribex(g))
			exit(99);
#endif

		cnt++;
		grib_handle_delete(g);
		grib_handle_delete(h);


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
