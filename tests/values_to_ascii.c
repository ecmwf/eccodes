/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "grib_api.h"

int main(int argc, char** argv) {
  int err = 0;
  double *values = NULL;
  char format[1024];
  size_t values_len= 0;

  size_t i = 0;
 
  long ni;
  double a;
  long d;
  FILE* in = NULL;

  grib_handle *h = NULL;

  
  if(argc > 1) 
    in = fopen(argv[1],"r");
  else{
    printf("usage : gribvals filename [format]\n");
    exit(1);
   }
  if(!in){
       printf("error file not found");
      return 1;
  }

  if(argc > 2) 
    strcpy(format,argv[2]);
  else
    strcpy(format,"%g ");
   

  h = grib_handle_new_from_file(0,in);
  while(h){

  if((err = grib_get_long(h,"ni",&ni)) != GRIB_SUCCESS)
  printf("error %d : %s",err,grib_get_error_message(err));

  if((err = grib_get_size(h,"values",&values_len)) != GRIB_SUCCESS){
    printf("error %d : %s",err,grib_get_error_message(err));
    return 1;
  }

  values = malloc(values_len*sizeof(double));

  if((err = grib_get_double_array(h,"values",values,&values_len)) != GRIB_SUCCESS)
  printf("error %d : %s",err,grib_get_error_message(err));

  for(i = 0; i < values_len; i++){
      if(i!=0 && (i%ni == 0))printf("\n");
      printf(format,values[i]);
    }
    printf("\n");
    fprintf(stderr,"%d values\n",i);
    free(values);

    grib_handle_delete(h);

    h = grib_handle_new_from_file(0,in);

   if(h)
    printf("************************************************************\n");
   
  }
  fclose(in);
  return 0;
}
