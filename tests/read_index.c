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

int main(int argc,char* argv[])
{
  grib_index* index=NULL;
  grib_handle* h=NULL;
  long *step,*level,*number;
  char** shortName=NULL;
  int i,j,k,l;
  size_t stepSize,levelSize,shortNameSize,numberSize;
  long ostep,olevel,onumber;
  char oshortName[200];
  size_t lenshortName=200;
  int ret=0,count=0;

  if (argc != 2) usage(argv[0]);

  printf("indexing...\n");

  index=grib_index_read(0,"out.gribidx",&ret);
  GRIB_CHECK(ret,0);

  printf("end indexing...\n");

  /* get the number of distinct values of "step" in the index */
  GRIB_CHECK(grib_index_get_size(index,"step",&stepSize),0);
  step=(long*)malloc(sizeof(long)*stepSize);
  if (!step) exit(1);
  /* get the list of distinct steps from the index */
  /* the list is in ascending order */
  GRIB_CHECK(grib_index_get_long(index,"step",step,&stepSize),0);
  printf("stepSize=%ld\n",(long)stepSize);
  for (i=0;i<stepSize;i++) printf("%ld ",step[i]);
  printf("\n");

  /*same as for "step"*/
  GRIB_CHECK(grib_index_get_size(index,"level",&levelSize),0);
  level=(long*)malloc(sizeof(long)*levelSize);
  if (!level) exit(1);
  /*same as for "step"*/
  GRIB_CHECK(grib_index_get_long(index,"level",level,&levelSize),0);
  printf("levelSize=%ld\n",(long)levelSize);
  for (i=0;i<levelSize;i++) printf("%ld ",level[i]);
  printf("\n");

  /*same as for "step"*/
  GRIB_CHECK(grib_index_get_size(index,"number",&numberSize),0);
  number=(long*)malloc(sizeof(long)*numberSize);
  if (!number) exit(1);
  /*same as for "step"*/
  GRIB_CHECK(grib_index_get_long(index,"number",number,&numberSize),0);
  printf("numberSize=%ld\n",(long)numberSize);
  for (i=0;i<numberSize;i++) printf("%ld ",number[i]);
  printf("\n");

  /*same as for "step"*/
  GRIB_CHECK(grib_index_get_size(index,"shortName",&shortNameSize),0);
  shortName=(char**)malloc(sizeof(char*)*shortNameSize);
  if (!shortName) exit(1);
  /*same as for "step"*/
  GRIB_CHECK(grib_index_get_string(index,"shortName",shortName,&shortNameSize),0);
  printf("shortNameSize=%ld\n",(long)shortNameSize);
  for (i=0;i<shortNameSize;i++) printf("%s ",shortName[i]);
  printf("\n");

  count=0;
  /* nested loops on the keys values of the index */
  /* different order of the nested loops doesn't affect performance*/
  for (i=0;i<shortNameSize;i++) {
    /* select the grib with shortName=shortName[i] */
    grib_index_select_string(index,"shortName",shortName[i]);

    for (l=0;l<levelSize;l++) {
      /* select the grib with level=level[i] */
      grib_index_select_long(index,"level",level[l]);

      for (j=0;j<numberSize;j++) {
        /* select the grib with number=number[i] */
        grib_index_select_long(index,"number",number[j]);

        for (k=0;k<stepSize;k++) {
          /* select the grib with step=step[i] */
          grib_index_select_long(index,"step",step[k]);

		 /* create a new grib_handle from the index with the constraints 
		    imposed by the select statements. It is a loop because
			in the index there could be more than one grib with those
			constrants */
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
         if (ret!=GRIB_END_OF_INDEX) {printf("error: %s\n",grib_get_error_message(ret)); exit(ret);}
	    } 
	  }
	}
  }
  printf("  %d messages selected\n",count);


  return 0;
}

