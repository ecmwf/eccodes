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
 * C Implementation: grib_get
 *
 * Description:
 *
 */

#include <stdio.h>
#include <unistd.h>
#include "grib_api_internal.h"


void usage( char*);

int main(int argc, char *argv[])
{
  FILE *f=NULL;
  char* infname=NULL;
  int ret=0;
  grib_context* gc;
  grib_handle* h;
  FILE* of=NULL;
  const void* buffer=NULL;
  size_t size=0;
  char* ofname=NULL;

  if ( argc < 3 ) usage(argv[0]);

  infname=argv[1];
  ofname=argv[2];

  f = fopen(infname,"r");
  if(!f) {
    perror(infname);
    exit(1);
  }

  gc = grib_context_get_default();
  h=grib_handle_new(gc);
  if (!h) {
    printf("Error: unable to create handle\n");
    exit(1);
  }

  GRIB_CHECK(grib_load_from_text_file(h,f),infname);

  if(fclose(f)) {
    perror(infname);
    exit(1);
  }

  of = fopen(ofname,"w");
  if(!of) {
     perror(ofname);
     exit(1);
  }

  grib_get_message(h,&buffer,&size);

  if(fwrite(buffer,1,size,of) != size) {
    perror(ofname);
    exit(1);
  }

  if(fclose(of)) {
    perror(ofname);
    exit(1);
  }

  return ret;

}

void usage(char* progname) {
  printf("\nUsage: %s dump_file outfile.grib\n",progname);
  exit(1);
}
