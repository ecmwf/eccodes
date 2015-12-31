/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "config.h"
/* #include <sys/types.h> */
/* #include <inttypes.h> */
/* #include <stdio.h> */
/* #include <stdlib.h> */
/* #include <errno.h> */
/* #include <string.h> */
#include "grib_api_internal.h"
#include <unistd.h>

void usage(char* prog) {
  printf("usage: %s filename\n",prog);
  exit(1);
}

int main(int argc,char* argv[]) {
  off_t offsetin=0,offsetout=0;
  off_t size=100000000;
  size_t rsizein=10,rsizeout=0;
  double doff=0;
  FILE* f=0;
  char str[10];
  int i;

  if (argc != 2) usage(argv[0]);

  printf("sizeof(off_t)=%d sizeof(long)=%d\n",sizeof(off_t),sizeof(long));

  f=fopen(argv[1],"r");
  if (!f) {
    printf("%s\n",strerror(errno));
	exit(errno);
  }

  offsetin=size;
  for (i=0;i<100000;i++) {
    printf("i=%d  ",i);
    if (fseeko(f,offsetin,SEEK_SET) ) {
	  printf("fseeko error\n");
      printf("- %d - %s\n",errno,strerror(errno));
	  exit(1);
	}
	rsizeout=fread(str,1,rsizein,f);
	if (rsizeout != rsizein ) {
	  printf("rsizein=%d rsizeout=%d\n",rsizein,rsizeout);
	  printf("end of file\n");
	  exit(1);
	}
	offsetout=ftello(f);
	doff=(double)offsetout;
	printf("%20.0f \n",doff);
	if (offsetout != offsetin+rsizeout) {
	  printf("offset error: offsetout=%jd offsetin=%jd\n",offsetout,offsetin+rsizeout);
	  exit(1);
	}
	offsetin=offsetout+size;
  }

  fclose(f);
  return 0;
}
