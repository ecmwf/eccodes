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
	
	f = fopen(argc > 1 ? argv[1]: "data" ,"r");
	if(!f)
	{
		perror(argc > 1 ? argv[1]: "data");
		exit(10);
	}

	length = sizeof(buffer);
	while((err = readany(f,buffer,&length)) == 0)
	{

		g = grib_handle_new_from_message(0,buffer,length);
		h = grib_handle_clone(g);

		if(compare_with_gribex(h))
			exit(1);

		if(compare_with_handle(g,h))
			exit(1);


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
