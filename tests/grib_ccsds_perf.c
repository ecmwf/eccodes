/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

#if GRIB_TIMER

void usage(char* prog) {
  printf("usage: %s [-a outfile | -w outfile ] grib_file repetitions bitsPerValue\n",prog);
  exit(1);
}

size_t grib_handle_write(grib_handle* h,char* filename) {
  FILE* of=NULL;
  const void *buffer; size_t size;

  of = fopen(filename,"w");
  if(!of) {
      perror(filename);
      exit(1);
  }
  GRIB_CHECK(grib_get_message(h,&buffer,&size),0);
  if(fwrite(buffer,1,size,of) != size) {
    perror(filename);
    exit(1);
  }
  fclose(of);

  return size;

}

static void print_timer(grib_timer* t,int repeat) {
   printf("%s : %g cpu\n",t->name_,t->timer_/repeat);
}

int main(int argc, char* argv[]) {
  grib_handle *h=NULL;
  grib_context* c=NULL;
  FILE* fin=NULL;
  FILE* fout=NULL;
  char* finname;
  char* ofilename;
  char defofilename[]="grib_ccsds_perf.out";
  double *values=NULL;
  int append=0;
  size_t nvalues=0;
  int count,e=0;
  int repeatccsds=1;
  int repeatsimple=1;
  grib_timer *tes,*tds,*tej,*tdj;
  char grid_ccsds[]="grid_ccsds";
  size_t grid_ccsds_l=strlen(grid_ccsds);
  char grid_simple[]="grid_simple";
  size_t grid_simple_l=strlen(grid_simple);
  char packingType[50]={0,};
  size_t len=50;
  char param[50]={0,};
  char gridType[50]={0,};
  char outfilename[255]={0,};
  size_t filesize_ccsds=0;
  size_t filesize_simple=0;
  double perc=0;
  long bitsPerValue=0;
  int iarg=1;
  char grid[20]={0,};
  char shortName[20]={0,};
  long level;
  char levelType[20]={0,};

  tes=grib_get_timer(0,"encoding simple", 0, 0);
  tds=grib_get_timer(0,"decoding simple", 0, 0);
  tej=grib_get_timer(0,"encoding ccsds", 0, 0);
  tdj=grib_get_timer(0,"decoding ccsds", 0, 0);

  if (argc != 4 && argc != 6 ) usage(argv[0]);
  if (!strcmp(argv[iarg],"-w")) {
    append=0;
    iarg++;
    ofilename=argv[iarg];
    iarg++;
  } else if (!strcmp(argv[iarg],"-a")) {
    append=1;
    iarg++;
    ofilename=argv[iarg];
    iarg++;
  } else {
    append=0;
    ofilename=defofilename;
  }
  finname=argv[iarg++];
  repeatsimple=atoi(argv[iarg++]);
  bitsPerValue=atoi(argv[iarg++]);

  fin = fopen(finname,"r");
  if(!fin) {perror(finname);exit(1);}

  if (append)
      fout = fopen(ofilename,"a");
  else
      fout = fopen(ofilename,"w");

  if(!fout) {perror(ofilename);exit(1);}

  c=grib_context_get_default();
  e=0;
  h=grib_handle_new_from_file(c,fin,&e);
  fclose(fin);

  GRIB_CHECK(e,0);

  len=50;
  grib_get_string(h,"shortName",param,&len);

  len=20;
  grib_get_string(h,"levelType",levelType,&len);

  if (!strcmp(levelType,"pl")) {
    GRIB_CHECK(grib_get_long(h,"level",&level),0);
    sprintf(shortName,"%s%ld",param,level);
  } else {
    sprintf(shortName,"%s",param);
  }


  grib_set_long(h,"editionNumber",2);
  GRIB_CHECK(grib_get_size(h,"values",&nvalues),0);
  values=(double*)grib_context_malloc(c,sizeof(double)*nvalues);
  if (!values) { printf("%s: memory allocation error\n",argv[0]); exit(1); }

  len=50;
  grib_get_string(h,"gridType",gridType,&len);

  len=50;
  grib_get_string(h,"packingType",packingType,&len);

  GRIB_CHECK(grib_get_double_array(h,"values",values,&nvalues),0);
  grib_set_long(h,"bitsPerValue",bitsPerValue);
  GRIB_CHECK(grib_set_double_array(h,"values",values,nvalues),0);

  printf("--------------------------------\n");
  printf("- %s - gridType=%s packingType=%s numberOfValues=%ld bitsPerValue=%ld\n",
    param,gridType,packingType,(long)nvalues,bitsPerValue);

  if (!strcmp(packingType,"spectral_complex") || !strcmp(packingType,"spectral_simple")) {
     printf("unable to process spectral data\n");
     exit(1);
  }

  if (!strcmp(gridType,"reduced_gg") || !strcmp(gridType,"regular_gg")) {
     long N;
     grib_get_long(h,"N",&N);
     printf("    N=%ld\n",N);
     sprintf(grid,"%ld",N);
  }
  if (!strcmp(gridType,"regular_ll")) {
     double Di,Dj;
     grib_get_double(h,"DiInDegrees",&Di);
     grib_get_double(h,"DjInDegrees",&Dj);
     printf("    Di=%g Dj=%g\n",Di,Dj);
     sprintf(grid,"%g/%g",Di,Dj);
  }


  if (!append)
    fprintf(fout,
    "shortName gridType numberOfValues bitsPerValue grid sizeSimple sizeccsds encodeccsds encodeSimple decodeccsds decodeSimple\n");

  /* decode values grid_simple */
  if (strcmp(packingType,grid_simple))
    grib_set_string(h,"packingType",grid_simple,&grid_simple_l);
  /* printf("decoding simple\n"); */
  grib_timer_start(tds);
  for (count=0;count<repeatsimple;count++)
    GRIB_CHECK(grib_get_double_array(h,"values",values,&nvalues),0);
  grib_timer_stop(tds,0);
  /* printf("%d messages decoded\n\n",count); */

  *outfilename='\0';
  sprintf(outfilename,"%s_%s_%ld_simple.grib2",param,gridType,bitsPerValue);
  filesize_simple=grib_handle_write(h,outfilename);
  printf("file size simple = %ld\n",(long)filesize_simple);

  /* encode values grid_simple*/
  /* printf("encoding simple\n"); */
  grib_timer_start(tes);
  for (count=0;count<repeatsimple;count++)
    GRIB_CHECK(grib_set_double_array(h,"values",values,nvalues),0);
  grib_timer_stop(tes,0);
  /* printf("%d messages encoded \n\n",count); */

  /* decode values grid_ccsds */
  grib_set_string(h,"packingType",grid_ccsds,&grid_ccsds_l);
  /* printf("decoding ccsds\n"); */
  grib_timer_start(tdj);
  for (count=0;count<repeatccsds;count++)
    GRIB_CHECK(grib_get_double_array(h,"values",values,&nvalues),0);
  grib_timer_stop(tdj,0);
  /* printf("%d messages decoded\n\n",count); */

  *outfilename='\0';
  sprintf(outfilename,"%s_%s_%ld_ccsds.grib2",param,gridType,bitsPerValue);
  filesize_ccsds=grib_handle_write(h,outfilename);
  printf("file size ccsds   = %ld\n",(long)filesize_ccsds);

  perc=(double)filesize_simple/(double)filesize_ccsds;

  printf("compression ratio = %g \n",perc);
  printf("space savings = %g \n",(1.0-1.0/perc)*100);

  /* encode values grid_ccsds*/
  /* printf("encoding ccsds\n"); */
  grib_timer_start(tej);
  for (count=0;count<repeatccsds;count++)
    GRIB_CHECK(grib_set_double_array(h,"values",values,nvalues),0);
  grib_timer_stop(tej,0);
  /* printf("%d messages encoded \n\n",count); */

  grib_handle_delete(h);
  grib_context_free(c,values);

  print_timer(tej,repeatccsds);
  print_timer(tdj,repeatccsds);
  print_timer(tes,repeatsimple);
  print_timer(tds,repeatsimple);
  printf("--------------------------------\n\n");
  fprintf(fout,"%s %s %ld %ld %s %ld %ld %g %g %g %g\n",
    shortName,gridType,(long)nvalues,bitsPerValue,
    grid,(long)filesize_simple,(long)filesize_ccsds,tej->timer_/repeatccsds,tes->timer_/repeatsimple,tdj->timer_/repeatccsds,tds->timer_/repeatsimple);

  return 0;
}
#else

int main(int argc,char* argv[]) {
return 0;
}

#endif
