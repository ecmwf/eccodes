/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include "grib_api_internal.h"

static void usage(const char* prog) {
  printf("usage: %s infile\n",prog);
  exit(1);
}

static unsigned char buffer[50000000];

int main(int argc,char* argv[]) {

  char* filename;
  FILE* f;
  grib_handle* h=NULL;
  grib_context* c;
  size_t size=0;
  int ret=0;
  size_t bufsize=sizeof(buffer);
  long count,step,edition,totalLength;
  char gridType[50],levelType[50],level[50],shortName[50];
  size_t gridTypelen=sizeof(gridType);
  size_t levelTypelen=sizeof(levelType);
  size_t levellen=sizeof(level);
  size_t shortNamelen=sizeof(shortName);
  size_t len;

  if (argc!=2) usage(argv[0]);
  filename=argv[1];

  f=fopen(filename,"r");
  if (!f) {
    perror(filename);
	exit(1);
  }
  c=grib_context_get_default();

  size=bufsize;
  count=1;
  while ((ret=grib_read_any_from_file(c,f,buffer,&size))==GRIB_SUCCESS) {
	if ((1)) {
		h=grib_handle_new_from_message_copy(c,buffer,size);
		if (!h) {
		  printf("unable to new from message\n");
		  exit(1);
		} else {
		  grib_get_long(h,"edition",&edition);
		  grib_get_long(h,"step",&step);
		  grib_get_long(h,"totalLength",&totalLength);
		  len=gridTypelen;
		  grib_get_string(h,"gridType",gridType,&len);
		  len=levelTypelen;
		  GRIB_CHECK(grib_get_string(h,"levelType",levelType,&len),0);
		  len=levellen;
		  grib_get_string(h,"level",level,&len);
		  len=shortNamelen;
		  grib_get_string(h,"shortName",shortName,&len);
		  printf("- %3ld -\t ed=%ld\t size=%8ld totalLength=%8ld \t %s\t %s\t %s\t level=%s\t step=%ld\n",
		  	count, edition, (long)size, totalLength, shortName, gridType, levelType, level, step);
		  grib_handle_delete(h);
		}
	} else {
		printf("MESSAGE #%ld\n",count);
	}
    size=bufsize;
	count++;
  }

  return 0;

}
