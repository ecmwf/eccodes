#include "test.h"

char buffer[1024*1024*10];
double values[1024*1024*10];

long cnt = 0;
long ff = 0;
long m = 0;

void* foo[1000000] = {0,};

static void  default_free(const grib_context* c, void* p)   {

#if 0
	int i, found = 0;

	for(i = 0; i < m; i++)
		if(foo[i] == p)
		{
			foo[i] = 0;
			found = 1;
		}

	if(!found) marslog(LOG_EXIT,"Wrong free called %p",p);
#endif

	ff++;
	cnt--;
	free(p);
}

static void* default_malloc(const grib_context* c, size_t size) { 
	cnt++;
#if 0
	if(m == NUMBER(foo)) marslog(LOG_EXIT,"Too many malloc");
	return foo[m++] = malloc(size);
#else
	return malloc(size);
#endif
}


int main(int argc,char** argv)
{
	long length = 0;
	FILE *f;
	grib_handle  *g =  0;
	int e;
	grib_iterator* i;
	double lat,lon,val;
	int n;
	size_t count;
	size_t fields1 = 0;
	size_t fields2 = 0;
	grib_handle* p;
	FILE* dev_null = fopen("/dev/null","w");

	grib_context *c = grib_context_new(0);


	f = fopen(argc > 1 ? argv[1]: "data" ,"r");
	if(!f)
	{
		perror(argc > 1 ? argv[1]: "data");
		exit(1);
	}

	length = sizeof(buffer);

	grib_context_set_memory_proc(c,default_malloc,default_free);
	grib_context_set_persistent_memory_proc(c,default_malloc,default_free);

	while( (e = readany(f,buffer,&length)) == 0)
	{

		/* alarm(60); */
		fields1++;

		if(fields1 != 1)
		{
			length = sizeof(buffer);
			alarm(0);
			continue;
		}

		g = grib_handle_new_from_message(c,buffer,length);
		p = grib_handle_new_from_message_copy(c,buffer,length);

		grib_dump_content(g,dev_null,NULL,0,NULL);

		i = grib_iterator_new(g);

		if(i != NULL)
		{

			n = 0;
			while(grib_iterator_next(i,&lat,&lon,&val))
			{
				n++;
			}


			grib_iterator_delete(i);

			/* printf("n = %d\n",n); if(n != size) exit(1); */

		}

		count = 0;
		if(grib_get_size(g,"values",&count) == 0 && count != 0)
		{
			double *values = (double*)malloc(sizeof(double)*count);
			int err;

			if((err =  grib_get_double_array(g,"values",values,&count)))
			{
				fprintf(stderr,"Cannot get values %d (%s) count=%d\n",err,grib_get_error_message(err),count);
			}
			free(values);
		}


		{
				grib_handle *h  = grib_handle_clone(g); 
				grib_keyset* ks = grib_new_keyset(h,"mars.*");
				char name[80];
				size_t len = sizeof(name);
				int err;

				while(grib_keyset_next(ks,name,&len))
				{
					double d; long l; char c[1024]; unsigned char b[1024];
					printf("%s\n",name);

					if((err = grib_get_double(h,name,&d)))
						fprintf(stderr,"Cannot get %s as double %d (%s)\n",name,err,
							grib_get_error_message(err));

					if((err = grib_get_long(h,name,&l)))
						fprintf(stderr,"Cannot get %s as long %d (%s)\n",name,err,
							grib_get_error_message(err));

					len = sizeof(c);
					if((err = grib_get_string(h,name,c,&len)))
						fprintf(stderr,"Cannot get %s as string %d (%s)\n",name,err,
							grib_get_error_message(err));

					len = sizeof(b);
					if((err = grib_get_bytes(h,name,b,&len)))
						fprintf(stderr,"Cannot get %s as bytes %d (%s)\n",name,err,
							grib_get_error_message(err));


					len = sizeof(name);
				}

				grib_keyset_delete(ks);
				grib_handle_delete(h);

		}


		grib_handle_delete(g);
		grib_handle_delete(p);


		length = sizeof(buffer);
	}
	
	

	rewind(f);
	while((g = grib_handle_new_from_file(c,f,&e)))
	{

int i;

		fields2++;


		for(i=1;i<10;i++)
			grib_set_long(g,"localDefinitionNumber",i);

		grib_handle_delete(g);
	}
	
	if(e) {
	fprintf(stderr,"grib_handle_new_from_file error %s\n",grib_get_error_message(e));
		exit(1);}

	if(fields1 != fields2)
	{
		fprintf(stderr,"readany/grib_handle_new_from_file mismatch %d %d\n",fields1,fields2);
		exit(1);
	}

	grib_handle_delete(grib_handle_new_from_samples(c,"latlon"));

	rewind(f);



	grib_context_delete(c);

	if(cnt != 0)
	{
		fprintf(stderr,"Malloc/free mismatch %ld\n",cnt);
		exit(1);
	}

	fclose(f);

	if(e != -1)
	{
		fprintf(stderr,"readany error %d\n",e);
		exit(1);
	}

	fclose(dev_null);

	exit(cnt);

}
