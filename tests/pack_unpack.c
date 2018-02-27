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

#ifdef HAVE_LIBEMOS

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

void usage(char* prog) {
  printf("usage: %s grib_file\n",prog);
  exit(1);
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

int main(int argc, char* argv[]) {
  flong ksec0[ISECTION_0];
  flong ksec1[ISECTION_1];
  flong ksec2[ISECTION_2];
  flong ksec3[ISECTION_3];
  flong ksec4[ISECTION_4];

  double rsec2[RSECTION_2];
  double rsec3[RSECTION_3];
  flong sec4len;
  flong miss=0;
  const void *msg;
  flong gribex_msg_len=0;
  grib_handle *h=NULL;
  grib_context* c=NULL;
  FILE* fin=NULL;
  char* finname;
  double *values=NULL;
  double *values_gribex=NULL;
  size_t nvalues=0;
  int maxnvalues;
  unsigned char buffer[5000000];
  size_t length = sizeof(buffer);
  int i=338;

  if (argc != 2) usage(argv[0]);
  finname=argv[1];

  fin = fopen(finname,"r");
  if(!fin) {perror(finname);exit(1);}

  c=grib_context_get_default();
  GRIB_CHECK(grib_read_any_from_file(c,fin,buffer,&length),0);
  fclose(fin);

  maxnvalues=1000000;
  h=grib_handle_new_from_message_copy(c,buffer,length);

  /* decode with gribex */
  msg=(char*)buffer;
  sec4len=maxnvalues+100000;
  values_gribex  = (double*)grib_context_malloc(c,sizeof(double)*(sec4len));

  printf("decoding with gribex\n");
  gribex_msg_len=length;
  gribex_check(cgribex(  miss, ksec0,ksec1,ksec2,rsec2,ksec3,rsec3,
                      ksec4,values_gribex,sec4len, (char*)msg,&gribex_msg_len,"D"));

  printf("gribex v[%d]=%.20e\n",i,values_gribex[i]);
  printf("ksec1[1]=%d\n",ksec1[1]);

  /* decode values with grib_api*/
  GRIB_CHECK(grib_get_size(h,"values",&nvalues),0);
  values=(double*)grib_context_malloc(c,sizeof(double)*nvalues);
  if (!values) { printf("%s: memory allocation error\n",argv[0]); exit(1); }
  if (maxnvalues<nvalues) maxnvalues=nvalues;

  printf("decoding with grib_api\n");
  GRIB_CHECK(grib_get_double_array(h,"values",values,&nvalues),0);
  printf("grib_api v[%d]=%.20e\n",i,values[i]);

  grib_context_free(c,values_gribex);
  return 0;
}

#else

int main(int argc, char* argv[]) {
  return 0;
}
#endif


