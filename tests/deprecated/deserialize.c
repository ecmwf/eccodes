#include <stdio.h>

#include "grib_api_internal.h"

int main(int argc, char *argv[])
{
#if 1
	FILE *f;
	int ret=0;
	int i;
	grib_associative_array* ar;
	grib_context* gc;
	grib_handle* gh;
	

	
	gc = grib_context_get_default();
	ar=grib_associative_array_new(gc);
	if (argc > 1) {
		for (i=1;i<argc;i++) {
			f = fopen(argv[i],"r");
  		if(!f) {
    		perror(argv[i]);
    		exit(1);
  		}
			grib_file_to_array(f,ar);
		}
	} else {
		grib_file_to_array(f,ar);
	}
	
	gh=grib_handle_new_from_array(ar);
	
	grib_associative_array_destroy(ar);
	
  if (gh) { 

  	grib_dump_content(gh,stdout,NULL,0,NULL);
		grib_set_long(gh,"editionNumber",2,NULL); 
		grib_dump_content(gh,stdout,NULL,0,NULL);
	} else  {
		printf("Error: unable to create grib_handle\n");
	}
  return ret;
	
#else
	grib_handle* gh = grib_handle_new_from_samples(NULL,"latlon");
	grib_set_long(gh,"editionNumber",2);
	grib_dump_content(gh,stdout,NULL,0,NULL);
	  return 0;
#endif
}
