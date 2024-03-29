/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: fieldset
 *
 * Description: how to use a fieldset.
 *
 *
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grib_api.h"

void usage(char* prog) {
  printf("Usage: %s order_by grib_file grib_file ...\n",prog);
  exit(1);
}

int main(int argc, char** argv) {
  int err = 0;
  long step,levelType;
  char** filenames;
  size_t nkeys,nfiles;
  int i=0;
  char* keys[]={"step","date","param","levelType"};
  grib_fieldset* set;
  grib_handle* h;
  char param[20]={0,};
  char date[10]={0,};
  size_t datelen=10;
  size_t len=20;
  char* order_by=0;

  if (argc != 3) usage(argv[0]);

  nkeys=sizeof(keys)/sizeof(*keys);
  order_by=strdup(argv[1]);

  nfiles=argc-2;
  filenames=(char**)malloc(sizeof(char*)*nfiles);
  for (i=0;i<nfiles;i++)
    filenames[i]=(char*)strdup(argv[i+2]);

  set=grib_fieldset_new_from_files(0,filenames,nfiles,keys,nkeys,0,0,&err);
  GRIB_CHECK(err,0);

  /* not yet implemented */
  /* err=grib_fieldset_apply_where(set,"(centre=='ecmf') && number==1 || step==6 "); */
  /* GRIB_CHECK(err,0); */

  grib_fieldset_apply_order_by(set,order_by);
  GRIB_CHECK(err,0);

  printf("\nordering by %s\n",order_by);
  printf("\n%d fields in the fieldset\n",grib_fieldset_count(set));
  printf("step,date,levelType,levelType\n");
  while ((h=grib_fieldset_next_handle(set,&err))!=NULL) {
    GRIB_CHECK(grib_get_long(h,"step",&step),0);
    GRIB_CHECK(grib_get_string(h,"date",date,&datelen),0);
    GRIB_CHECK(grib_get_string(h,"param",param,&len),0);
    GRIB_CHECK(grib_get_long(h,"levelType",&levelType),0);

    printf("%ld %s %ld %s\n",step,date,levelType,param);
    grib_handle_delete(h);
  }

  grib_fieldset_delete(set);
  grib_handle_delete(h);

  return 0;
}
