/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "ecCodes_internal.h"

void usage(char* prog) {
  printf("usage: %s infile outfile \n",prog);
  exit(1);
}


int main(int argc,char* argv[]) {

	FILE* infh;
	FILE* outfh;
	char* filename;
	char* ofilename;
	void* mesg=NULL;
	void* mesgo=NULL;
	size_t size=0;
	size_t osize=0;
	off_t offset=0;
	int write;
	int err=0;
	int error=0;
	int count=0;
	int edition=0;
	grib_context* c=grib_context_get_default();

	if (argc !=3) usage(argv[0]);

	n=0;
	filename=argv[1];
	infh=fopen(filename,"r");
	if (!infh) {
		perror(filename);
		exit(1);
	}

	ofilename=argv[1];
	outfh=fopen(ofilename,"w");
	if (!outfh) {
		perror(ofilename);
		exit(1);
	}


	count=0;
	while ( (mesg=wmo_read_bufr_from_file_malloc ( infh,0,  &size,&offset,&err))!=NULL ) {
		if (err && err!=GRIB_END_OF_FILE) {
			grib_context_log(c,GRIB_LOG_ERROR,
				"unable to decode message: %s  \n",grib_get_error_message(err));
			error=err;
		}
		edition=mesg[7];
		write=1;
		switch (edition ) {
			case 3:
				omesg=grib_context_malloc_clear(c,size+4);
				memcpy(omesg,mesg,7);
				omesg[7]=4;
				osize=8;
				break;
			case 4:
				omesg=mesg;
				break;
			default :
				error=1;
				write=0;
				grib_context_log(c,GRIB_LOG_ERROR,
					"wrong edition %d, skipping message",edition);
		}
		if (write) {
			if(fwrite(omesg,1,osize,outfh) != osize) {
				perror(ofilename);
				exit(1);
			}

		}
		if (mesg) {grib_context_free(c,mesg);mesg=NULL;}
		if (omesg) {grib_context_free(c,omesg);omesg=NULL;}
		count++;
	}


	fclose(infh);
	fclose(outfh);

  return error;
}
