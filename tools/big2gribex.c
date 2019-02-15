/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: big2gribex
 *
 * Description:
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "grib_api.h"
void usage(char*);

int main(int argc, char *argv[])
{
  grib_handle* h = NULL;
  FILE* inf = NULL;
  FILE* ouf = NULL;
  char* infile=0;
  char* oufile=0;
  int err = 0;
  size_t size;
  const void* buffer=NULL;

  if (argc < 3) usage(argv[0]);
  infile=argv[1];
  oufile=argv[2];

  inf = fopen(infile,"r");
  if(!inf) {
     perror(infile);
     exit(1);
  }

  ouf = fopen(oufile,"w");
  if(!ouf) {
     perror(oufile);
     exit(1);
  }

  while((h = grib_handle_new_from_file(0,inf,&err)) != NULL) {
      grib_update_sections_lengths(h);
	  GRIB_CHECK(grib_get_message(h,&buffer,&size),0);
	  if(fwrite(buffer,1,size,ouf) != size) {
        perror(oufile);
		exit(1);
	  }
  }

  grib_handle_delete(h);

  if(fclose(inf)) {
     perror(infile);
     exit(1);
  }

  if(fclose(ouf)) {
     perror(oufile);
     exit(1);
  }


  return 0;
}

void usage(char* progname) {
  printf("\nUsage: %s grib_in grib_out\n",progname);
  exit(1);
}
