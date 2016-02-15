/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api.h"

void usage(char* prog) {
  printf("usage: %s infile\n",prog);
  exit(1);
}

int main(int argc,char* argv[]) {
  
  grib_index* index=NULL;
  grib_handle* h=NULL;
  char* infile=NULL;
  int i,j,k,l;
  long ostep,olevel,onumber;
  char oshortName[200];
  size_t lenshortName=200;
  int ret=0,count=0;

  if (argc != 2) usage(argv[0]);
  infile=argv[1];
  outfile=argv[2];

  printf("indexing...\n");
  /* Create an index from a grib file with a given set of keys*/
  index=grib_index_new_from_file(0,infile,"shortName,level,number,step",&ret);
  if (ret) {printf("error: %s\n",grib_get_error_message(ret)); exit(ret);}
  printf("end indexing...\n");

  /* ask the index to order the fields thorough some keys*/
  ret=grib_index_orderby(index,"level,shortName,number:desc,step:asc");
  if (ret!=GRIB_SUCCESS) exit(1);

  count=0;
  /* loop through all the fields in the asked order*/
  while ((h=grib_handle_new_from_index(index,&ret))!=NULL){
	count++;
	if (ret) {printf("error: %d\n",ret); exit(ret);}
	lenshortName=200;
	grib_get_string(h,"shortName",oshortName,&lenshortName);
	grib_get_long(h,"level",&olevel);
	grib_get_long(h,"number",&onumber);
	grib_get_long(h,"step",&ostep);
	printf("shortName=%s ",oshortName);
	printf("level=%ld ",olevel);
	printf("number=%ld ",onumber);
	printf("step=%ld \n",ostep);
	grib_handle_delete(h);
  }
  if (ret!=GRIB_END_OF_INDEX) {
   printf("error: %s\n",grib_get_error_message(ret)); 
   exit(ret);
  }
  printf("  %d messages selected\n",count);

  return 0;
}

