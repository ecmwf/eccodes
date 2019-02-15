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

#define flong int

#define NUMBER(a) (sizeof(a)/sizeof(a[0]))

#define BUFF_SIZE 10000000

#ifdef HAVE_LIBEMOS
#if GRIB_TIMER
#define TEST_OK
#endif
#endif

#ifdef TEST_OK

extern void grsrnd_(flong*);
extern void gribex_(flong*, flong*, flong*, double*, flong*, double*, flong*, double*,
        flong*, char *, flong*, flong*, const char*, flong*,flong );

static int cgribex(
        flong miss,

        flong ksec0[],flong   ksec1[],
        flong ksec2[],double rsec2[],
        flong ksec3[],double rsec3[],
        flong ksec4[],double rsec4[],
        flong sec4len,

        char *buffer,
        flong *buflen,
        char *op)
{
    flong    ret = 1;
    flong    in_bufsize  = (*buflen) / sizeof(flong);
    flong    out_bufsize = in_bufsize;
    flong    round = 0;

    rsec3[1] = miss,
    ksec3[1] = miss,

    grsrnd_(&round); /* no rounding !!! */

    gribex_(ksec0, ksec1, ksec2, rsec2, ksec3, rsec3, ksec4, rsec4,
            &sec4len, buffer, &in_bufsize, &out_bufsize, op, &ret,1 );

    *buflen = out_bufsize * sizeof(flong);

    return ret;
}

struct { int code; char *message; } gribex_errors[] = {
    {201 ," An invalid coding/decoding option was requested.",},
    {202 ,"The number of bits per data value exceeds a word length.",},
    {203 ,"Missing data indicated and data field contains non-zero values.",},
    {204 ,"Number of bits per data value is invalid.",},
    {205 ,"Number of bits per data value is same as the number of bits per computer word.",},
    {301 ,"Error in encoding or decoding the letters GRIB.",},
    {302 ,"Error decoding the length of the GRIB message.",},
    {303 ,"Error encoding or decoding the GRIB edition number.",},
    {304 ,"Error decoding octets 22 and 23, experimental edition check.",},
    {305 ,"Input data is not GRIB or pseudo-GRIB.",},
    {401 ,"Error encoding or decoding the length of section 1.",},
    {402 ,"Error encoding or decoding the parameter version number.",},
    {403 ,"Error encoding or decoding six header fields in section 1, from 'identification of centre' to 'indicator of type of level'.",},
    {404 ,"Error encoding or decoding height, pressure, etc of levels.",},
    {405 ,"Error encoding or decoding six fields in section 1, from 'year of century' to 'indicator of unit of time range'.",},
    {406 ,"Error encoding or decoding period of time.",},
    {407 ,"Error encoding or decoding time range indicator.",},
    {408 ,"Error encoding or decoding number averaged.",},
    {409 ,"Error encoding or decoding number missing from averages etc.",},
    {410 ,"Error encoding or decoding century of data or reserved field.",},
    {411 ,"Error encoding or decoding units decimal scale factor.",},
    {412 ,"Error encoding or decoding ECMWF local data.",},
    {413 ,"Grib edition not handled.",},
    {499 ,"Error found when checking values for section 1 against expected valid GRIB values.",},
    {501 ,"Error encoding or decoding length of section 2.",},
    {502 ,"Error encoding or decoding number of vertical coordinate parameters.",},
    {503 ,"Error encoding or decoding location of list of vertical coordinate parameters or list of numbers of points.",},
    {504 ,"Error encoding or decoding data representation type.",},
    {505 ,"Error encoding or decoding number of points aflong a parallel or meridian.",},
    {505 ,"Error encoding or decoding number of points aflong a parallel or meridian.",},
    {506 ,"Error encoding or decoding latitude or flongitude of the first grid point.",},
    {507 ,"Error encoding or decoding the components flag.",},
    {508 ,"Error encoding or decoding latitude or flongitude of the last grid point.",},
    {509 ,"Error encoding or decoding the i direction increment.",},
    {510 ,"Error encoding or decoding the number of parallels between the pole and the Equator.",},
    {511 ,"Error encoding or decoding scanning mode flags.",},
    {513 ,"Error encoding or decoding the j direction increment.",},
    {514 ,"Error encoding or decoding J,K,M pentagonal resolution parameters.",},
    {515 ,"Error encoding or decoding representation type or mode.",},
    {517 ,"Error encoding or decoding latitude or flongitude of southern pole.",},
    {518 ,"Error encoding or decoding angle of rotation.",},
    {519 ,"Error encoding or decoding latitude or flongitude of pole of stretching.",},
    {520 ,"Error encoding or decoding stretching factor.",},
    {521 ,"Error encoding or decoding vertical coordinate parameters.",},
    {522 ,"Error encoding or decoding list of numbers of points.",},
    {523 ,"Error encoding or decoding number of points aflong X or Y axis.",},
    {524 ,"Error encoding or decoding X or Y axis grid lengths.",},
    {525 ,"Error encoding or decoding the projection centre flag.",},
    {526 ,"Error encoding or decoding the latitude or flongitude of sub-satellite point.",},
    {527 ,"Error encoding or decoding the diameter of the Earth in X or Y direction.",},
    {528 ,"Error encoding or decoding the X or Y coordinate of the sub-satellite point.",},
    {529 ,"Error encoding or decoding the orientation of the grid or camera angle.",},
    {530 ,"Error encoding or decoding the X or Y coordinate of the origin of sector.",},
    {531 ,"Error inserting/extracting Latin1 or Latin2 of secants points.",},
    {532 ,"Error inserting/extracting latitude/flongitude of southern pole.",},
    {540 ,"Error encoding gaussian grid section 2.",},
    {541 ,"Error decoding gaussian grid section 2.",},
    {542 ,"Error encoding lat/flong grid section 2.",},
    {543 ,"Error decoding lat/flong grid section .",},
    {544 ,"Error encoding spectral section 2.",},
    {545 ,"Error decoding spectral section 2.",},
    {546 ,"Error encoding space view section 2, or error encoding ECMWF ocean section 2.",},
    {547 ,"Error decoding space view section 2, or error decoding ECMWF ocean section 2.",},
    {598 ,"Representation type not handled.",},
    {598 ,"Representation type not handled.",},
    {599 ,"Error found when checking values for section 2 against expected valid GRIB values.",},
    {601 ,"Error encoding or decoding length of section 3.",},
    {602 ,"Error encoding or decoding the number of unused bits at the end of section 3.",},
    {603 ,"Error encoding or decoding the bitmap reference table.",},
    {604 ,"Error encoding or decoding the primary bitmap.",},
    {605 ,"Unable to convert quasi-regular (reduced) gaussian grid with a bitmap.",},
    {699 ,"Error found when checking values for section 3 against expected valid GRIB values.",},
    {701 ,"Error encoding or decoding the length of section 4.",},
    {703 ,"Second-order packing implies additional flags.",},
    {704 ,"Function A or B invalid for second-order packed field (grid-point).",},
    {705 ,"Only simple packing handled for grid data.",},
    {706 ,"Error in decoding the section 4 flag field.",},
    {707 ,"Error encoding or decoding the field scale factor.",},
    {708 ,"Error encoding or decoding the field reference value.",},
    {709 ,"Error encoding or decoding the number of bits per data value.",},
    {710 ,"Output array too small.",},
    {711 ,"Error encoding or decoding a REAL coefficient.",},
    {712 ,"Error encoding or decoding data values.",},
    {713 ,"Error encoding or decoding the 'flag and unused bits' field.",},
    {714 ,"Decoding option is 'X' and number of values is illegal.",},
    {715 ,"Decoding option is 'X' and scanning mode is not north to south and west to east.",},
    {716 ,"Decoding option is 'X' and field is not a gaussian or latitude/flongitude grid.",},
    {717 ,"Decoding option is 'X' and a bitmap is encountered.",},
    {720 ,"Error encoding or decoding the octet number at which packed data begins.",},
    {721 ,"Error encoding or decoding the 'extended flag' field.",},
    {722 ,"Error encoding or decoding the first or second dimension of a matrix.",},
    {723 ,"Error encoding or decoding six fields, from first dimension coordinate value onwards.",},
    {724 ,"Error encoding or decoding first or second dimension coefficients.",},
    {725 ,"Error encoding a secondary bitmap.",},
    {726 ,"Output array is not big enough to allow expansion using bitmap(s), or constant (0-bit) field without section 2 not supported.",},
    {727 ,"Invalid predefined bitmap number given, or constant (0-bit) spectral field not supported.",},
    {728 ,"Open of predefined bitmap failed, or constant (0-bit) field function G or B not supported.",},
    {729 ,"Problem reading a predefined bitmap size, or output array is not big enough to allow expansion using bitmaps.",},
    {730 ,"Problem allocating memory for a predefined bitmap.",},
    {731 ,"Problem reading the number of non-missing points in a predefined bitmap.",},
    {732 ,"Problem reading a predefined bitmap.",},
    {733 ,"Problem closing a predefined bitmap file.",},
    {734 ,"Error inserting padding zeroes at end of section 4.",},
    {796 ,"Error extracting pointer to packed data.",},
    {797 ,"Error extracting sub-set truncation or constant (0-bit) field function G or B not supported.",},
    {798 ,"Function is X but no section 2 included.",},
    {799 ,"Error found when checking values for section 4 against expected valid GRIB values.",},
    {800 ,"Error encoding or decoding the 7777 group.",},
    {801 ,"Error inserting or extracting the 7777 group.",},
    {802 ,"Error encoding or decoding the length of the GRIB message.",},
    {805 ,"End of message 7777 group not found.",},
    {806 ,"Error in decoding a primary or secondary bitmap.",},
    {807 ,"Inconsistent values specified for complex packing in KSEC2(6) and KSEC4(4).",},
    {808 ,"Error converting quasi-regular (reduced) gaussian grid to regular.",},
    {809 ,"Error padding unused part of GRIB to zero.",},
    {810 ,"Error inserting dummy zero, or error padding GRIB to multiple of 120.",},
    {811 ,"Unable to handle secondary bitmaps for J option.",},
};
void gribex_check(int err) {
  int i=0;
  if(err > 0) {
    char *e = "unknown";
    for(i = 0; i < NUMBER(gribex_errors); i++)
      if(gribex_errors[i].code == err)
        e = gribex_errors[i].message;

    fprintf(stderr,"GRIBEX error %d: %s\n",err,e);
    exit(err);
  }
}

#define     ISECTION_0  2
#define     ISECTION_1  1024     /* beware of  for ocean data */
#define     ISECTION_2  3000
#define     ISECTION_3  2
#define     ISECTION_4  512

#define     RSECTION_2  512
#define     RSECTION_3  2
#define     RSECTION_4  1

#define STRMAXLEN  300




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
  int i;
  grib_handle *h=NULL;
  grib_handle *hso=NULL;
  grib_context* c=NULL;
  FILE* fin=NULL;
  FILE* fout=NULL;
  char* finname;
  char* ofilename;
  char defofilename[]="so_perf.out";
  double *values=NULL;
  double gvalues[1000000]={0,};
  double sovalues[1000000]={0,};
  int append=0;
  size_t nvalues=0;
  int count,e=0;
  int repeatso=1;
  int repeatsimple=1;
  grib_timer *tes,*tds,*teso,*tdso;
  char grid_simple[]="grid_simple";
  size_t grid_simple_l=strlen(grid_simple);
  char packingType[50]={0,};
  size_t len=50;
  char param[50]={0,};
  char gridType[50]={0,};
  char outfilename[255]={0,};
  size_t filesize_so=0;
  size_t filesize_simple=0;
  double perc=0;
  long bitsPerValue=0;
  int iarg=1;
  char grid[20]={0,};
  char shortName[20]={0,};
  long level;
  char levelType[20]={0,};
  char buffer[BUFF_SIZE]={0,};
  char buf[BUFF_SIZE]={0,};
  size_t length;
  int sec4len;
  flong ksec0[ISECTION_0];
  flong ksec1[ISECTION_1];
  flong ksec2[ISECTION_2];
  flong ksec3[ISECTION_3];
  flong ksec4[ISECTION_4];
  flong miss=0;
  const void *msg;
  flong gribex_msg_len=0;
  double packingError=0;


  double rsec2[RSECTION_2];
  double rsec3[RSECTION_3];

  tes=grib_get_timer(0,"encoding simple", 0, 0);
  tds=grib_get_timer(0,"decoding simple", 0, 0);
  teso=grib_get_timer(0,"encoding so", 0, 0);
  tdso=grib_get_timer(0,"decoding so", 0, 0);

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

  c=grib_context_get_default();
  length=BUFF_SIZE;
  GRIB_CHECK(grib_read_any_from_file(c,fin,buffer,&length),0);
  fclose(fin);


  if (append) 
	  fout = fopen(ofilename,"a");
  else
	  fout = fopen(ofilename,"w");

  if(!fout) {perror(ofilename);exit(1);}

  c=grib_context_get_default();
  e=0;
  h=grib_handle_new_from_message_copy(c,buffer,length);

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

  /* grib_set_long(h,"editionNumber",2); */
  GRIB_CHECK(grib_get_size(h,"values",&nvalues),0);
  values=(double*)grib_context_malloc_clear(c,sizeof(double)*nvalues);
  if (!values) { printf("%s: memory allocation error\n",argv[0]); exit(1); }

  len=50;
  grib_get_string(h,"gridType",gridType,&len);

  len=50;
  grib_get_string(h,"packingType",packingType,&len);

  GRIB_CHECK(grib_get_double_array(h,"values",values,&nvalues),0);
  grib_set_long(h,"bitsPerValue",bitsPerValue);
  GRIB_CHECK(grib_set_double_array(h,"values",values,nvalues),0);
  GRIB_CHECK(grib_get_double(h,"packingError",&packingError),0);

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
	"shortName gridType numberOfValues bitsPerValue grid sizeSimple sizeso encodeso encodeSimple decodeso decodeSimple\n");

  sec4len=nvalues+100000;

  /* decode values grid_simple */
  if (strcmp(packingType,grid_simple))
    grib_set_string(h,"packingType",grid_simple,&grid_simple_l);
  grib_timer_start(tds);
  for (count=0;count<repeatsimple;count++) 
    GRIB_CHECK(grib_get_double_array(h,"values",values,&nvalues),0);
  grib_timer_stop(tds,0);

  *outfilename='\0';
  sprintf(outfilename,"%s_%s_%ld_%ld_simple.grib1",param,gridType,bitsPerValue,(long)nvalues);
  filesize_simple=grib_handle_write(h,outfilename);
  printf("file size simple = %ld\n",(long)filesize_simple);

  /* encode values grid_simple*/
  grib_timer_start(tes);
  for (count=0;count<repeatsimple;count++) 
    GRIB_CHECK(grib_set_double_array(h,"values",values,nvalues),0);
  grib_timer_stop(tes,0);

  /* decode with gribex*/
  msg=(char*)buffer;
  gribex_msg_len=BUFF_SIZE;
  sec4len=nvalues+100000;
  gribex_check(cgribex(  miss, ksec0,ksec1,ksec2,rsec2,ksec3,rsec3,
                      ksec4,gvalues,sec4len, (char*)msg,&gribex_msg_len,"D")); 

  /* encode values second order with gribex*/
  ksec4[1] = bitsPerValue;
  /* to indicate complex packing. */
  ksec4[3] = 64;

  /* to indicate extended flags are present. */
  ksec4[5] = 16;

  ksec4[9] = 16; 
  ksec4[11] = 8; 
  ksec4[12] = 4; 
  ksec4[13] = 0; 
  ksec4[14] = -1; 

  gribex_msg_len=BUFF_SIZE;
  grib_timer_start(teso);
  gribex_check(cgribex(  miss, ksec0,ksec1,ksec2,rsec2,ksec3,rsec3,
                      ksec4,gvalues,nvalues, buf,&gribex_msg_len,"K"));
  grib_timer_stop(teso,0);

  hso=grib_handle_new_from_message_copy(c,buf,gribex_msg_len);

  GRIB_CHECK(grib_get_double_array(h,"values",sovalues,&nvalues),0);

  *outfilename='\0';
  sprintf(outfilename,"%s_%s_%ld_%ld_so.grib1",param,gridType,bitsPerValue,(long)nvalues);
  filesize_so=grib_handle_write(hso,outfilename);
  printf("file size so   = %ld\n",(long)filesize_so);

  perc=(double)filesize_simple/(double)filesize_so; 

  printf("compression ratio = %g \n",perc);
  printf("space savings = %g \n",(1.0-1.0/perc)*100);

  grib_handle_delete(h);
  /* decode values second order */
  /* decode with gribex*/
  msg=(char*)buf;
  gribex_msg_len=BUFF_SIZE;
  sec4len=nvalues+100000;
  grib_timer_start(tdso);
  gribex_check(cgribex(  miss, ksec0,ksec1,ksec2,rsec2,ksec3,rsec3,
                      ksec4,gvalues,sec4len, (char*)msg,&gribex_msg_len,"D")); 
  grib_timer_stop(tdso,0);

  for (i=0;i<nvalues;i++) {
	if (fabs(gvalues[i]-values[i])>packingError) {
	  printf("++++++++ Wrong coding\n");
	  printf("packingError=%g gvalues[%d]=%.20e values[%d]=%.20e abs. err=%g \n",packingError,i,
	  gvalues[i],i,values[i],gvalues[i]-values[i]);
	}
  }

  for (i=0;i<nvalues;i++) {
	if (fabs(gvalues[i]-sovalues[i])>packingError) {
	  printf("++++++++ Wrong coding\n");
	  printf("packingError=%g gvalues[%d]=%.20e sovalues[%d]=%.20e abs. err=%g \n",packingError,i,
	  gvalues[i],i,sovalues[i],gvalues[i]-sovalues[i]);
	}
  }

  grib_handle_delete(hso);
  grib_context_free(c,values);

  print_timer(teso,repeatso);
  print_timer(tdso,repeatso);
  print_timer(tes,repeatsimple);
  print_timer(tds,repeatsimple);

  fprintf(fout,"%s %s %ld %ld %s %ld %ld %g %g %g %g\n",
	shortName,gridType,(long)nvalues,bitsPerValue,
  	grid,(long)filesize_simple,(long)filesize_so,teso->timer_/repeatso,tes->timer_/repeatsimple,tdso->timer_/repeatso,tds->timer_/repeatsimple);

  fclose(fout);
  return 0;
}
#else

int main(int argc,char* argv[]) {
return 0;
}

#endif

