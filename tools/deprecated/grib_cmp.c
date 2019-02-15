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
 * C Implementation: grib_cmp
 *
 * Description: Binary comparison between two files
 *
 */
#include "grib_api_internal.h"

void usage(char* progname) {
  printf("\nUsage: %s file file\n",progname);
  exit(1);
}

/*extern int errno;*/

int main(int argc, char *argv[]) {
  int err=0,count=0,ret1=0,ret2=0;
  char *file1=NULL,*file2=NULL;
  FILE *f1 = NULL;
  FILE *f2 = NULL;
  unsigned char buff1[1000000];
  unsigned char buff2[1000000];
  grib_context* c;
  size_t size1,size2;

  fprintf(stderr, "\nWARNING: The tool %s is deprecated. Please use grib_compare instead.\n\n", argv[0]);

  if (argc != 3) usage(argv[0]);
  file1=argv[1];
  file2=argv[2];

  f1 = fopen(file1,"r");
  if(!f1) {perror(file1); exit(1);}
  
  f2 = fopen(file2,"r");
  if(!f2) {perror(file2);exit(1);}

  count=1;
  c=grib_context_get_default();
  size1=sizeof(buff1);
  size2=sizeof(buff2);
  while (((ret1=grib_read_any_from_file(c,f1,buff1,&size1))==GRIB_SUCCESS) &
         ((ret2=grib_read_any_from_file(c,f2,buff2,&size2))==GRIB_SUCCESS)    ) {
    if (size1!=size2 || memcmp(buff1,buff2,size1)) {
      printf("message %d are different\n",count);
      break;
    }
    size1=sizeof(buff1);
    size2=sizeof(buff2);
    count++; 
  }

/* old cmp
  count=1;
  while (!feof(f1) && !feof(f2)) {
     if (fread(&a,1,1,f1) != 1 && !feof(f1)) {
       printf("%s error: %s\n",file1,strerror(errno));
       exit(1);
     }
     if (fread(&b,1,1,f2)!=1 && !feof(f2)) {
       printf("%s error: %s\n",file2,strerror(errno));
       exit(1);
     }
     if (a != b) {
       printf("%s differs from %s at byte %d (0x%.2X != 0x%.2X) \n",file1,file2,count,a,b);
       return 1;
     }
     count++;
  }
*/

  fclose(f1);
  fclose(f2);
  
  if (!ret1 && ret2==GRIB_END_OF_FILE) {
    printf("%s bigger than %s\n",file1,file2);
    return 1;
  }
  if (!ret2 && ret1==GRIB_END_OF_FILE) {
    printf("%s bigger than %s\n",file2,file1);
    return 1;
  }
  if (ret2!=GRIB_END_OF_FILE && ret2 ) {
    printf("%s message %d: %s\n",file1,count,grib_get_error_message(ret2));
    err++;
  }

  if (ret1!=GRIB_END_OF_FILE && ret1) {
    printf("%s message %d: %s\n",file1,count,grib_get_error_message(ret1));
    err++;
  }

  return err;
}

