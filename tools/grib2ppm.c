/*
 * Copyright 2005-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stdio.h>
#include <stdlib.h>

#include "grib_api.h"

int main(int argc, char *argv[])
{
  grib_handle* h = NULL;
  FILE* f = NULL;
  int i = 0;
  int err = 0;
  char *mode = "file";

  for(i = 1; i < argc; i++)
  {

    if(argv[i][0] == '-')
    {
      mode = argv[i]+1;
      continue;
    }

    f = fopen(argv[i],"r");
    if(!f) {
      perror(argv[i]);
      exit(1);
    }

    while((h = grib_handle_new_from_file(0,f,&err)) != NULL)
    {
      long width;
      long height;
      double max, min;
      double *values = NULL;
      size_t count;
      int i;

      GRIB_CHECK(grib_get_size(h,"values",&count),0);
      values = (double *)malloc(sizeof(double)*count);

      GRIB_CHECK(grib_get_long(h,"Ni",&width),0);
      GRIB_CHECK(grib_get_long(h,"Nj",&height),0);
      GRIB_CHECK(grib_get_double_array(h,"values",values,&count),0);

      max=values[0];
      min=values[0];
      for (i=1; i<count;++i)
      {
        if(values[i]>max)
          max = values[i];
        if(values[i]<min)
          min = values[i];
      }

      /* PPM header */
      printf("P5\n%ld %ld\n255\n",width,height);
      for (i=0; i<count;++i)
      {
        char c = (values[i] - min)*255 / (max - min);
        printf("%c",c);
      }

      grib_handle_delete(h);
    }
    fclose(f);
    if(err)
    {
      fprintf(stderr,"%s\n",grib_get_error_message(err));
      exit(1);
    }
  }
  return 0;
}
