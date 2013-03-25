#include "grib_api.h"

void usage(char* prog) {
	printf("usage: %s gribfile\n",prog);
	exit(1);
}

int main(int argc,char* argv[]) {
	FILE* fh;
	int err=0;
	grib_handle* h;
	long binaryScaleFactor=0;
	double max=0,min=0;
	long paramId=0;
	int count=0;

	if (argc!=2) usage(argv[0]);

	fh=fopen(argv[1],"r");
	if (!fh) {
		perror(argv[1]);
		exit(1);
	}

	while ((h=grib_handle_new_from_file(0,fh,&err))!=NULL) {

		count++;
		grib_get_long(h,"paramId",&paramId);	
		if (paramId!=246 && paramId!=247) {grib_handle_delete(h); continue;}

		grib_get_long(h,"binaryScaleFactor",&binaryScaleFactor);	
		if (binaryScaleFactor!=0) { grib_handle_delete(h); continue;}

		grib_get_double(h,"max",&max);	
		grib_get_double(h,"min",&min);	

		if (max!=min) {
			printf("grib_corruption_check: field #%d corrupted: max=%g min=%g\n",count,max,min);
			grib_handle_delete(h);
			return 1;
		}
		grib_handle_delete(h);
	}

	fclose(fh);

	return 0;
}
