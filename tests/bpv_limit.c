/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * =====================================================================================
 *
 *       Filename:  bpv_limit.c
 *
 *    Description:  test setting the number of bpv before decoding and then encoding;
 *                  setting the wrong number of bpv should result in an exception
 *
 *        Version:  1.0
 *        Created:  30/03/10 16:07:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Cristian Codorean (mow), mow@ecmwf.int
 *        Company:  ECMWF
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>

#include "grib_api.h"

void usage(char* prog) {
  printf("usage: %s filein\n",prog);
  exit(1);
}


int main(int argc, char** argv) {
  int err = 0;
  size_t values_len=0;
  double *values = NULL;
  char error_msg[100] ;
 
  size_t slong = sizeof(long) * 8 ;

  int i;

  FILE* in = NULL;
  char* filename ;

  grib_handle *h = NULL;

  if (argc<2) usage(argv[0]);
  filename=argv[1];

  in = fopen(filename,"r");
  if(!in) {
    printf("ERROR: unable to open file %s\n",filename);
    return 1;
  }

  /* create new handle from a message in a file*/
  h = grib_handle_new_from_file(0,in,&err);
  if (h == NULL) {
    printf("Error: unable to create handle from file %s\n",filename);
  }

  for (i=0;i<255;i++) {

    /* get the size of the values array*/
    GRIB_CHECK(grib_get_size(h,"values",&values_len),0);

    values = (double*)malloc(values_len*sizeof(double));

    err = grib_get_double_array(h,"values",values,&values_len);

    GRIB_CHECK(grib_set_long(h,"bitsPerValue",i),0);

    err = grib_set_double_array(h,"values",values,values_len);

    free(values);
    values = 0;

    /*
     * check if encoding of the data fails when the number of bpv
     * is not supported; bpv allowed on decoding is bpv < size of long
     */

    if (i < slong && err == 0) {
      /* do nothing */
    } else if (i >= slong  && err == GRIB_INVALID_BPV) {
      /* do nothing  */
    } else {
      sprintf(error_msg,"Error decoding when bpv=%d. Error message:%s",i,grib_get_error_message(err));
      perror(error_msg);
      exit(1);
    }

    values = (double*)malloc(values_len*sizeof(double));
    err = grib_get_double_array(h,"values",values,&values_len);

    /*
     * check if decoding of the data fails when the number of bpv
     * is not supported; bpv allowed on decoding is bpv <= size of long
     */

    if (i <= slong && err == 0) {
      /* do nothing */
    } else if (i > slong  && err == GRIB_INVALID_BPV) {
      /* do nothing  */
    } else {
      sprintf(error_msg,"Error decoding when bpv=%d. Error message:%s",i,grib_get_error_message(err));
      perror(error_msg);
      exit(1);
    }

    free(values);
    values = 0;
  }

  grib_handle_delete(h);
  h=0;

  fclose(in);

  return 0;
}
