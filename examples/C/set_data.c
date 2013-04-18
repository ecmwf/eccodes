/*
 * Copyright 2005-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: set_data
 *
 * Description: set the data contained in a grib file
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "grib_api.h"

void usage(char* prog) {
	printf("usage: %s out.grib\n",prog);
	exit(1);
}

int main(int argc, char** argv) {
  int i;
  double *values = NULL;
  size_t values_len= 0;

  const char* filename = "regular_ll_pl_grib1";
  grib_handle *h = NULL;
  double d,e;
  long count;

  if (argc!=2) usage(argv[0]);
  /* create new handle from a message in a file*/
  h = grib_handle_new_from_template(0,"regular_ll_pl_grib1");
  if (h == NULL) {
    printf("Error: unable to create handle from file %s\n",filename);
	exit(1);
  }

  values_len=10000;
  values = malloc(values_len*sizeof(double));
  d=10e-8;
  e=d;
  count=1;
  for (i=0;i<values_len;i++) {
 	if (count>1000) {e*=10; count=1;}
	values[i]=d;
	printf("%g \n",values[i]);
	d+=e;
	count++;
  }

  GRIB_CHECK(grib_set_long(h,"bitsPerValue",16),0);

  /* set data values*/
  GRIB_CHECK(grib_set_double_array(h,"values",values,values_len),0);

  grib_write_message(h,argv[1],"w");

  free(values);
  grib_handle_delete(h);

  return 0;
}
