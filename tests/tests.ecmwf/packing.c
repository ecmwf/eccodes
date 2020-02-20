#include "test.h"


char buffer[1024*1024*80];


int main(int argc,char** argv)
{
	long length = 0;
	FILE *f;
	grib_handle  *g =  0;
	grib_handle  *h =  0;
	int err;
	size_t count;
	size_t ncount;
	size_t message_count = 0;
	int cnt = 0;
	double *values;
	int sample = getenv("SAMPLE") != NULL;
	int bad = 0;

	extern const char* grib_get_accessor_class_name(grib_handle* , const char* );

	/* grib_context_set_log_mode(0,GRIB_LOG_DEBUG); */

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
		const char* result_class_name = NULL;

		if(strncmp(buffer,"GRIB",4) != 0)
		{
			cnt++;
			length = sizeof(buffer);
			continue;
		}


		g = grib_handle_new_from_message(0,buffer,length);

		if(!g)
		{
			fprintf(stderr,"Cannot get handle : %ld - messages %d\n",length, message_count);
			exit(11);
		}
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
			message_count++;
			continue;
		}
#endif

		if(sample) save_message(g,"packing.orig");

		h = grib_handle_clone(g);
		if(!h)
		{
			fprintf(stderr,"Cannot clone handle : %ld - messages %d\n",length, message_count);
			exit(11);
		}

		if((err = grib_get_size(g,"values",&count)))
		{
			fprintf(stderr,"Cannot get number of values  %d - messages %d\n",err, message_count);
			exit(11);
		}


		values = (double*)malloc(sizeof(double)*(count));
		ncount = count;

		if((err =  grib_get_double_array(g,"values",values,&ncount)))
		{
			fprintf(stderr,"Cannot get original values %d (%s) count=%d - messages %d\n",err,grib_get_error_message(err),count, message_count);
			exit(15);
		}

		if(count != ncount)
		{
			fprintf(stderr,"count changed %d %d - messages %d\n",count,ncount, message_count);
			exit(15);
		}

		if((err =  grib_set_double_array(h,"values",values,count)))
		{
			fprintf(stderr,"Cannot set values %d (%s) count=%d- messages %d\n",err,grib_get_error_message(err),count, message_count);
			exit(15);
		}
		free(values);

		if(sample) save_message(h,"packing.grib");
		/* grib_dump_content(h,stdout,NULL,0,NULL); */

		if(compare_with_gribex(h)){
			fprintf(stderr,"messages %d\n",message_count);
			compare_two_gribex(g);
			compare_with_gribex(g);
			/* save_message(g,problem(get_current_dir_name(),"packing_orig",message_count)); */
			/* save_message(h,problem(get_current_dir_name(),"packing_api",message_count)); */
			exit(15);
		}

		if(compare_with_handle(g,h)){
			fprintf(stderr,"messages %d\n",message_count);
			compare_two_gribex(g);
			compare_with_gribex(g);
			compare_with_gribex(h);
			/* save_message(g,problem(get_current_dir_name(),"packing_orig",message_count)); */
			/* save_message(h,problem(get_current_dir_name(),"packing_api",message_count)); */
			exit(15);
		}

		grib_handle_delete(g);
		grib_handle_delete(h);


		length = sizeof(buffer);
		message_count++;
	}


	if(err != -1)
	{
		fprintf(stderr,"readany error %d\n",err);
		exit(17);
	}

	if(message_count == 0 && cnt == 0)
	{
		fprintf(stderr,"No GRIB processed\n");
		exit(18);
	}


	exit(bad?1:0);
}
