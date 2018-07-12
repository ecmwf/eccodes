/*
 * C Implementation: eps_wind_rose
 *
 * Description:

 *
 * Author: Enrico Fucile <enrico.fucile@ecmwf.int>, (C) 2006
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "grib_api.h"

void usage(char* prog) {
  printf("\nUsage: %s direction_grib_file intensity_grib_file",prog);
  exit(1);
}

int get_wind_rose_index(double v);
int get_sea_state_index(double v);

int main(int argc, char** argv) {
  FILE* fdir = NULL;
  FILE* fint = NULL;
  int i,j,k;
  grib_context* c=NULL;
  char* fintname = NULL;
  char* fdirname = NULL;
  grib_handle *hdir = NULL;
  grib_handle *hint = NULL;
  grib_handle *hold = NULL;
  const void *buffer=0; size_t size=0;
  double* values_dir=0;
  double* values_int=0;
  size_t size=0;
  char* ofile[100]={0,};
  size_t values_count=0;
  char* wind_rose[8]={"nord","nord_east",
                      "east","south_east",
                      "south","south_west",
                      "west","nord_west"};
  size_t wind_rose_count=sizeof(wind_rose);
  char* sea_state[9]={"calm","smooth","slight",
                      "moderate","rough","very_rough",
                      "high","very_high","phenomenal"};
  double* values[72]={0,};
  size_t sea_state_count=sizeof(sea_state);
  int eps_members=0,steps=0;
  long member=0,step=0,ltmp=0;
  int new_step=0;
  int handle_count=0;
  char short_name_dir[10]={0,};
  char short_name_int[10]={0,};
  char tmp[10]={0,};
  size_t len=0;
  int err1=0,err2=0;

  if (argc != 3) usage(argv[0]);

  /* direction file */
  fdirname=argv[1];
  fdir = fopen(fdirname,"r");
  if (!fdir) {
    printf("ERROR: unable to open file %s\n",fdirname);
    return 1;
  }

  /* intensity file */
  fintname=argv[2];
  fint = fopen(fintname,"r");
  if(!fint) {
    printf("ERROR: unable to open file %s\n",fintname);
    fclose(fdir);
    return 1;
  }

  k=0;
  for (i=0;i<wind_rose_count;i++) {
    for (j=0;j<sea_state_count;j++) {
      sprintf(ofile,"%s_%s.grib",wind_rose[k],sea_state[k]);
      unlink(ofile);
      k++;
    }
  }

  handle_count=0;
  step=-1;
  new_step=0;
  values_count=0;
  while( (hdir = grib_handle_new_from_file(0,fdir,&err1)) &&
         (hint = grib_handle_new_from_file(0,fint,&err2))) {

    if (!hdir) GRIB_CHECK(err1,0);
    if (!hint) GRIB_CHECK(err2,0);

    GRIB_CHECK(grib_get_long(hdir,"step",&ltmp),0);
    new_step=0;
    if (step < 0) step=ltmp;
    else if (step != ltmp) {
      new_step=1;
      step=ltmp;
    }

    GRIB_CHECK(grib_get_long(hint,"step",&ltmp),0);
    if (step != ltmp) {
       printf("ERROR  %s: wrong step order in files %s %s\n",argv[0],argv[1],argv[2]);
       exit(1);
    }

    if (new_step) {
      k=0;
      for (i=0;i<wind_rose_count;i++) {
        for (j=0;j<sea_state_count;j++) {
          GRIB_CHECK(grib_set_double_array (hold,"values", values[k],*values_count),0);
          GRIB_CHECK(grib_get_message(hold,&buffer,&size),0);
          sprintf(ofile,"%s_%s.grib",wind_rose[k],sea_state[k]);
          f=fopen(ofile,"w+");
          if(fwrite(buffer,1,size,f) != size)
          {
            perror(ofile);
            exit(1);
          }
          fclose(f);
          k++;
        }
      }
    }

    len=sizeof(tmp);
    GRIB_CHECK(grib_get_string(h,"short_name",&tmp,&len),0);
    if (short_name_int[0] != '\0') {
      if (strcmp(short_name_int,tmp)) {
        printf("%s two different parameters (%s and %s) found in file %s",
            argv[0],short_name_int,tmp,fintname);
      }
    } else sprintf(short_name_int,"%s",tmp);

    len=sizeof(tmp);
    GRIB_CHECK(grib_get_string(h,"short_name",&tmp,&len),0);
    if (short_name_dir[0] != '\0') {
      if (strcmp(short_name_dir,tmp)) {
        printf("%s two different parameters (%s and %s) found in file %s",
            argv[0],short_name_dir,tmp,fintname);
      }
    } else sprintf(short_name_dir,"%s",tmp);

    GRIB_CHECK(grib_get_size(hdir,"values",&size),0);
    if (values_count==0) values_count=size;
    else if (values_count != size) {
      printf("ERROR  %s: wrong values count %d %d\n",argv[0],values_count,size);
      exit(1);
    }

    GRIB_CHECK(grib_get_size(hint,"values",&size),0);
    if (values_count != size) {
      printf("ERROR  %s: wrong values count %d %d\n",argv[0],values_count,size);
      exit(1);
    }

    if (!values_dir) values_dir=grib_context_malloc(c,sizeof(double)*values_count);
    if (!values_dir) {
      printf("ERROR: memory allocation problem\n");
      exit(1);
    }
    size=values_count;
    GRIB_CHECK(grib_get_double_array(hdir,"values",values_dir,&size),0);
    if (values_count != size) {
      printf("ERROR  %s: wrong values count %d %d\n",argv[0],values_count,size);
      exit(1);
    }

    if (!values_int) values_int=grib_context_malloc(c,sizeof(double)*values_count);
    if (!values_int) {
      printf("ERROR: memory allocation problem\n");
      exit(1);
    }
    GRIB_CHECK(grib_get_double_array(hint,"values",values_int,&size),0);
    if (values_count != size) {
      printf("ERROR %s: wrong values count %d %d\n",argv[0],values_count,size);
      exit(1);
    }

    k=0;
    for (i=0;i<wind_rose_count;i++) {
      for (j=0;j<sea_state_count;j++) {
        if (!values[k]) values[k]=grib_context_malloc(c,sizeof(double)*values_count);
        if (!values[k]) {
          printf("ERROR: memory allocation problem\n");
          exit(1);
        }
        for (n=0;n<values_count;n++) values[k]=0;
        k++;
      }
    }

    for (n=0;n<values_count;n++) {
       i=get_wind_rose_index(values_dir[n]);
       j=get_sea_state_index(values_int[n]);
       k=i*sea_state_count+j;
       values[k]+=1;
    }

    handle_count++;

    grib_handle_delete(hold);
    grib_handle_delete(hint);
    hold=hdir;
  }

  grib_handle_delete(hdir);

  return 0;
}

int get_wind_rose_index(double v) {
  int ret=0;
  ret=round(v/45.0);
  ret = ret == 8 ? 0 : ret;
  return ret;
}

int get_sea_state_index(int v) {
  int ret=0;
  return ret;
}
