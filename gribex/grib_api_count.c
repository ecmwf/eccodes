#include "grib_api.h"

void usage(char* prog) {
	printf("usage: %s [edition] in.grib\n",prog);
	exit(1);
}
int main(int argc, char* argv[]) {
	FILE* fh;
	grib_handle* h;
	int count;
	int err;
	long edition,editionToSearch;
	char* file;
	char identifier[10]={0,};
	size_t slen=10;

	if (argc < 2 || argc > 3 ) usage(argv[0]);

	if (argc==3) {
		editionToSearch=atoi(argv[1]);
		file=argv[2];	
	} else {
		editionToSearch=0;
		file=argv[1];	
	}

	fh=fopen(file,"r");
	if (!fh) {
		perror(file);
		exit(1);
	}

	count=0;
	while( h=grib_handle_new_from_file(0,fh,&err)) {
		grib_get_long(h,"edition",&edition);
		slen=10;
		grib_get_string(h,"identifier",identifier,&slen);
		if (!strcmp(identifier,"GRIB") && (editionToSearch==0 || edition==editionToSearch)) count++;
		grib_handle_delete(h);
	}
	printf("%d\n",count);

	fclose(fh);

}
