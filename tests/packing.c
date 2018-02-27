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
#include <time.h>

#define NUMBER(a) (sizeof(a)/sizeof(a[0]))

#ifdef HAVE_LIBEMOS
#define SWAP(a,b) temp=(a);(a)=(b);(b)=temp;

void grib_sort(double* arr,int* index, int beg, int end,int buildindex) {
   double temp;
   if (buildindex) {
     int i;
     for (i=beg;i<=end;i++) index[i]=i;
   }
   if (end > beg) {
      double piv = arr[beg];
      int l = beg + 1, r = end;
      while (l < r) {
         if (arr[l] <= piv) l++;
         else if(arr[r] >= piv) r--;
         else {
          SWAP(arr[l],arr[r])
          SWAP(index[l],index[r])
         }
      }
      if(arr[l] < piv) {
        SWAP(arr[l],arr[beg])
        SWAP(index[l],index[beg])
        l--;
      }
      else {
        l--;
        SWAP(arr[l],arr[beg])
        SWAP(index[l],index[beg])
      }

      grib_sort(arr,index, beg, l,0);
      grib_sort(arr,index, r, end,0);
   }
}

extern void grsrnd_(long*);
extern void gribex_(long*, long*, long*, double*, long*, double*, long*, double*,
        long*, char *, long*, long*, const char*, long*,long );

static int cgribex(
        long miss,

        long ksec0[],long   ksec1[],
        long ksec2[],double rsec2[],
        long ksec3[],double rsec3[],
        long ksec4[],double rsec4[],
        long sec4len,

        char *buffer,
        long *buflen,
        char *op)
{
    long    ret = 1;
    long    in_bufsize  = (*buflen) / sizeof(long);
    long    out_bufsize = in_bufsize;
    long    round = 0;

    rsec3[1] = miss,
    ksec3[1] = miss,

    grsrnd_(&round); /* no rounding !!! */

    gribex_(ksec0, ksec1, ksec2, rsec2, ksec3, rsec3, ksec4, rsec4,
            &sec4len, buffer, &in_bufsize, &out_bufsize, op, &ret,1 );

    *buflen = out_bufsize * sizeof(long);

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
    {505 ,"Error encoding or decoding number of points along a parallel or meridian.",},
    {505 ,"Error encoding or decoding number of points along a parallel or meridian.",},
    {506 ,"Error encoding or decoding latitude or longitude of the first grid point.",},
    {507 ,"Error encoding or decoding the components flag.",},
    {508 ,"Error encoding or decoding latitude or longitude of the last grid point.",},
    {509 ,"Error encoding or decoding the i direction increment.",},
    {510 ,"Error encoding or decoding the number of parallels between the pole and the Equator.",},
    {511 ,"Error encoding or decoding scanning mode flags.",},
    {513 ,"Error encoding or decoding the j direction increment.",},
    {514 ,"Error encoding or decoding J,K,M pentagonal resolution parameters.",},
    {515 ,"Error encoding or decoding representation type or mode.",},
    {517 ,"Error encoding or decoding latitude or longitude of southern pole.",},
    {518 ,"Error encoding or decoding angle of rotation.",},
    {519 ,"Error encoding or decoding latitude or longitude of pole of stretching.",},
    {520 ,"Error encoding or decoding stretching factor.",},
    {521 ,"Error encoding or decoding vertical coordinate parameters.",},
    {522 ,"Error encoding or decoding list of numbers of points.",},
    {523 ,"Error encoding or decoding number of points along X or Y axis.",},
    {524 ,"Error encoding or decoding X or Y axis grid lengths.",},
    {525 ,"Error encoding or decoding the projection centre flag.",},
    {526 ,"Error encoding or decoding the latitude or longitude of sub-satellite point.",},
    {527 ,"Error encoding or decoding the diameter of the Earth in X or Y direction.",},
    {528 ,"Error encoding or decoding the X or Y coordinate of the sub-satellite point.",},
    {529 ,"Error encoding or decoding the orientation of the grid or camera angle.",},
    {530 ,"Error encoding or decoding the X or Y coordinate of the origin of sector.",},
    {531 ,"Error inserting/extracting Latin1 or Latin2 of secants points.",},
    {532 ,"Error inserting/extracting latitude/longitude of southern pole.",},
    {540 ,"Error encoding gaussian grid section 2.",},
    {541 ,"Error decoding gaussian grid section 2.",},
    {542 ,"Error encoding lat/long grid section 2.",},
    {543 ,"Error decoding lat/long grid section .",},
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
    {716 ,"Decoding option is 'X' and field is not a gaussian or latitude/longitude grid.",},
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

int statistics(grib_context* c, double err_threshold,double* values,
               double* oldvalues,double* grib_values,double* gribex_values,
			   int nvalues,int count,int dosort,int gribex_grib_compare) {
  double vmax,vmin, grib_vmax, grib_vmin, gribex_vmax, gribex_vmin;
  double vabsmin,vabs,grib_errmax,gribex_errmax,gribex_grib_errmax,ovmax,ovmin;
  double grib_vabsmin,gribex_vabsmin,grib_vabs,gribex_vabs;
  double *grib_err,*gribex_err,*gribex_grib_err;
  int i,imax,imin,grib_ierrmax,gribex_ierrmax,gribex_grib_ierrmax;
  int ret=0;
  double error=0;

  vmax=values[0]; vmin=values[0]; vabsmin=values[0];
  imax=0; imin=0;
  grib_vmax=grib_values[0]; grib_vmin=grib_values[0];
  gribex_vmax=gribex_values[0]; gribex_vmin=gribex_values[0];
  ovmax=oldvalues[0]; ovmin=oldvalues[0];
  gribex_vabsmin=gribex_values[0];
  grib_vabsmin=grib_values[0];
  vabsmin=values[0];

  grib_err=(double*)grib_context_malloc(c,sizeof(double)*nvalues);
  gribex_err=(double*)grib_context_malloc(c,sizeof(double)*nvalues);
  gribex_grib_err=(double*)grib_context_malloc(c,sizeof(double)*nvalues);

  grib_errmax=fabs((grib_values[0]-values[0])/values[0]);
  gribex_errmax=fabs((gribex_values[0]-values[0])/values[0]);
  gribex_grib_errmax=fabs((gribex_values[0]-grib_values[0])/grib_values[0]);
  grib_ierrmax=0;
  gribex_ierrmax=0;
  gribex_grib_ierrmax=0;

  for (i=0; i<nvalues; i++) {
    if (values[i]!=0) {
	  grib_err[i]=fabs((grib_values[i]-values[i])/values[i]);
      gribex_err[i]=fabs((gribex_values[i]-values[i])/values[i]);
      gribex_grib_err[i]=fabs((gribex_values[i]-grib_values[i])/grib_values[i]);
	}
    vabs=fabs(values[i]);
    grib_vabs=fabs(grib_values[i]);
    gribex_vabs=fabs(gribex_values[i]);

    if (grib_errmax < grib_err[i]) {grib_errmax=grib_err[i]; grib_ierrmax=i;}
    if (gribex_errmax < gribex_err[i]) {gribex_errmax=gribex_err[i]; gribex_ierrmax=i;}
    if (gribex_grib_errmax < gribex_grib_err[i]) {gribex_grib_errmax=gribex_grib_err[i]; gribex_grib_ierrmax=i;}

    if (vmax < values[i]) {vmax=values[i];imax=i;}
    if (vmin > values[i]) {vmin=values[i];imin=i;}
    if (ovmax < oldvalues[i]) ovmax=oldvalues[i];
    if (ovmin > oldvalues[i]) ovmin=oldvalues[i];
    if (vabs != 0 && vabsmin > vabs) vabsmin=vabs;
    if (grib_vabs != 0 && grib_vabsmin > grib_vabs) grib_vabsmin=grib_vabs;
    if (gribex_vabs != 0 && gribex_vabsmin > gribex_vabs) gribex_vabsmin=gribex_vabs;
    if (grib_vmax < grib_values[i]) grib_vmax=grib_values[i];
    if (grib_vmin > grib_values[i]) grib_vmin=grib_values[i];
    if (gribex_vmax < gribex_values[i]) gribex_vmax=gribex_values[i];
    if (gribex_vmin > gribex_values[i]) gribex_vmin=gribex_values[i];
  }
  if ( (!gribex_grib_compare && (gribex_errmax > err_threshold || grib_errmax > err_threshold)) || gribex_grib_errmax > err_threshold ) {
    int* index;
    ret=1;
    index=(int*)grib_context_malloc(c,sizeof(int)*nvalues);
    if (!index) {printf("memory allocation problems"); exit(1);}

    printf("--------- %d ---------\n",count);
    if (gribex_errmax > err_threshold) {
      printf("         gribex_errmax=%.5e  gribex_ierrmax=%d\n",gribex_errmax,gribex_ierrmax);
      printf("v[%d]=%.15e (gribex)\n",gribex_ierrmax,gribex_values[gribex_ierrmax]);
      printf("v[%d]=%.15e (grib_api)\n",gribex_ierrmax,grib_values[gribex_ierrmax]);
      printf("v[%d]=%.15e \n",gribex_ierrmax,values[gribex_ierrmax]);
    }
    if (grib_errmax > err_threshold) {
      printf("         grib_errmax=%.5e  grib_ierrmax=%d\n",grib_errmax,grib_ierrmax);
      printf("v[%d]=%.15e (grib_api)\n",grib_ierrmax,grib_values[grib_ierrmax]);
      printf("v[%d]=%.15e (gribex)\n",grib_ierrmax,gribex_values[grib_ierrmax]);
      printf("v[%d]=%.15e \n",grib_ierrmax,values[grib_ierrmax]);
    }
    if (gribex_grib_errmax > err_threshold) {
      printf("         gribex_grib_errmax=%.5e  gribex_grib_ierrmax=%d\n",gribex_grib_errmax,gribex_grib_ierrmax);
	  if (values[gribex_grib_ierrmax]!=0) 
	   error=(gribex_values[gribex_grib_ierrmax]-values[gribex_grib_ierrmax])/values[gribex_grib_ierrmax];
      printf("v[%d]=%.15e err=%.15e  (gribex)\n",gribex_grib_ierrmax,gribex_values[gribex_grib_ierrmax],error);
	  if (values[gribex_grib_ierrmax]!=0) 
	   error=(grib_values[gribex_grib_ierrmax]-values[gribex_grib_ierrmax])/values[gribex_grib_ierrmax];
      printf("v[%d]=%.15e err=%.15e  (grib_api)\n",gribex_grib_ierrmax,grib_values[gribex_grib_ierrmax],error);
      printf("v[%d]=%.15e \n",gribex_grib_ierrmax,values[gribex_grib_ierrmax]);
    }
    printf("  ovmax=%.15e   ovmin=%.15e\n",ovmax,ovmin);
    printf("   vmax=%.15e    vmin=%.15e  vabsmin=%.15e imax=%d imin=%d\n",
              vmax,vmin,vabsmin,imax,imin);
    printf("   vmax=%.15e    vmin=%.15e  vabsmin=%.15e (grib_api)\n",
              grib_vmax,grib_vmin,grib_vabsmin);
    printf("   vmax=%.15e    vmin=%.15e  vabsmin=%.15e (gribex)\n",
              gribex_vmax,gribex_vmin,gribex_vabsmin);
    printf("v[imax]=%.15e v[imin]=%.15e  (grib_api)\n",grib_values[imax],grib_values[imin]);
    printf("v[imax]=%.15e v[imin]=%.15e  (gribex)\n",gribex_values[imax],gribex_values[imin]);
    printf("i index oldv v gribex grib_api gribex_err grib_err\n");

    if(dosort) {
      grib_sort(values,index, 0, nvalues-1,1);
      for (i=0; i<nvalues; i++) {
        printf("%6d %6d %18.10e %18.10e %18.10e %18.10e %18.10e %18.10e \n",
             i,index[i],oldvalues[index[i]],values[i],gribex_values[index[i]],grib_values[index[i]],
             gribex_err[index[i]],grib_err[index[i]]);
      }
    } else {
      for (i=0; i<nvalues; i++) {
        printf("%6d %18.10e %18.10e %18.10e %18.10e %18.10e %18.10e \n",
             i,oldvalues[i],values[i],gribex_values[i],grib_values[i],
             gribex_err[i],grib_err[i]);
      }
  }

    grib_context_free(c,index);
  }
  grib_context_free(c,grib_err);
  grib_context_free(c,gribex_err);
  grib_context_free(c,gribex_grib_err);
  return ret;
}

void usage(char* prog) {
  printf("usage: %s write_grib{1=gribex, 2=grib_api} calculateP compare_gribex_vs_grib_api dosort noise_percentage error_threshold grib_file\n",prog);
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
  long ksec0[ISECTION_0];
  long ksec1[ISECTION_1];
  long ksec2[ISECTION_2];
  long ksec3[ISECTION_3];
  long ksec4[ISECTION_4];

  double rsec2[RSECTION_2];
  double rsec3[RSECTION_3];
  long sec4len;
  long miss;
  const void *msg;
  char* gribex_msg;
  size_t msg_len=0;
  long gribex_msg_len=0;
  grib_handle *h=NULL,*hgribex=NULL;
  grib_context* c=NULL;
  FILE* fin=NULL;
  char* finname;
  int err=0;
  double *values=NULL,*grib_values=NULL,*gribex_values=NULL,*cvalues=NULL;
  double *oldvalues=NULL;
  size_t nvalues=0,nvalueso;
  double r,err_threshold;
  int i,count;
  int noiselev,dosort;
  int gribex_grib_compare=0;
  int seed=1234567;
  long lval=0;
  int calculateP,write_grib;
  int argi=0;
  FILE* foutgribex=NULL;
  char foutnamegribex[]="gribex_out.grib";
  FILE* foutgrib=NULL;
  char foutnamegrib[]="grib_api_out.grib";

  long grib_decimalScaleFactor=0,grib_binaryScaleFactor=0,grib_bitsPerValue=0;
  long gribex_decimalScaleFactor=0,gribex_binaryScaleFactor=0,gribex_bitsPerValue=0;
  double grib_referenceValue=0,grib_laplacianOperator=0;
  double gribex_referenceValue=0,gribex_laplacianOperator=0;
  char name[STRMAXLEN];
  size_t namelen=STRMAXLEN;

  argi=0;
  if (argc != 8) usage(argv[argi]);
  write_grib=atoi(argv[++argi]);
  calculateP=atoi(argv[++argi]);
  gribex_grib_compare=atoi(argv[++argi]);
  dosort=atoi(argv[++argi]);
  noiselev=atoi(argv[++argi]);
  err_threshold=atof(argv[++argi]);
  finname=argv[++argi];

  fin = fopen(finname,"r");
  if(!fin) {perror(finname);exit(1);}

  c=grib_context_get_default();
  srand ( seed );

  /* open grib file (grib_api only)*/
  count=0;
  while((h = grib_handle_new_from_file(c,fin,&err)) != NULL
         || err != GRIB_SUCCESS ) {

    count++;
    GRIB_CHECK(grib_get_long(h,"missingValue",&miss),0);
    GRIB_CHECK(grib_get_message(h,&msg,&msg_len),0);

    /* decode values with grib_api*/
    GRIB_CHECK(grib_get_size(h,"values",&nvalues),0);

    values=(double*)grib_context_malloc(c,sizeof(double)*nvalues);
    oldvalues=(double*)grib_context_malloc(c,sizeof(double)*nvalues);
    if (!values) { printf("%s: memory allocation error\n",argv[0]); exit(1); }

    nvalueso=nvalues;
    GRIB_CHECK(grib_get_double_array(h,"values",oldvalues,&nvalueso),0);
    if (nvalues !=nvalueso) {printf("wrong number of values\n");exit(1);}

    /* add random noise */
    if (noiselev !=0 ) {
      for (i=0; i<nvalues; i++) {
        r=(((double)((rand() % (2 * noiselev)) - noiselev/2 +101))/100.);
        values[i] = oldvalues[i]*r;
      }
    } else {
      for (i=0; i<nvalues; i++) {
        values[i] = oldvalues[i];
      }
    }

    cvalues  = (double*)grib_context_malloc(c,sizeof(double)*(nvalues));
    for (i=0; i<nvalues; i++) cvalues[i] = values[i];

	/* calculate P factor */
	lval=-32767;
	if (calculateP) 
	  GRIB_CHECK(grib_set_long(h,"P",lval),0);

    /* encode/decode with grib_api */
    GRIB_CHECK(grib_set_double_array(h,"values",values,nvalues),0);

    grib_values=(double*)grib_context_malloc(c,sizeof(double)*nvalues);
    if (!grib_values) { printf("%s: memory allocation error\n",argv[0]); exit(1); }

    GRIB_CHECK(grib_get_double_array(h,"values",grib_values,&nvalueso),0);

    /* encode/decode with gribex */
    sec4len=nvalues+100000;
    gribex_values  = (double*)grib_context_malloc(c,sizeof(double)*(sec4len));

    /* just to get the ksec ready */
    gribex_msg_len=msg_len;
    gribex_check(cgribex(  miss, ksec0,ksec1,ksec2,rsec2,ksec3,rsec3,
                      ksec4,gribex_values,sec4len, (char*)msg,&gribex_msg_len,"D"));

    sec4len = nvalues +100000;
    gribex_msg = (char*)grib_context_malloc(c,sizeof(char)*(sec4len));

    gribex_msg_len=sec4len;
    gribex_check(cgribex( miss, ksec0,ksec1,ksec2,rsec2,ksec3,rsec3,
                      ksec4,cvalues,nvalues, (char*)gribex_msg,&gribex_msg_len,"C"));

    gribex_check(cgribex(  miss, ksec0,ksec1,ksec2,rsec2,ksec3,rsec3,
                      ksec4,gribex_values,sec4len, (char*)gribex_msg,&gribex_msg_len,"D"));

    /* writing the output grib files if required */
    if (write_grib == 1) {
	  if (!foutgribex) {
	    foutgribex=fopen(foutnamegribex,"w");
		  if (!foutgribex) {
		     perror(foutnamegribex);
		     exit(1);
		}
	  }
	  if (!foutgribex) {
	    printf("unable to open output file\n");
		exit(1);
      }
	  if(fwrite(gribex_msg,1,gribex_msg_len,foutgribex) != gribex_msg_len) {
	    perror(foutnamegribex);
	    exit(1);
      }
	}

    if (write_grib == 2) {
      GRIB_CHECK(grib_get_message(h,&msg,&msg_len),0);
	  if (!foutgrib) {
	    foutgrib=fopen(foutnamegrib,"w");
		  if (!foutgrib) {
		     perror(foutnamegrib);
		     exit(1);
		}
	  }
	  if (!foutgrib) {
	    printf("unable to open output file\n");
		exit(1);
      }
	  if(fwrite(msg,1,msg_len,foutgrib) != msg_len) {
	    perror(foutnamegrib);
	    exit(1);
      }
	}

    /* statistics on values */
    if (statistics(c,err_threshold,values,oldvalues,
                    grib_values,gribex_values,nvalues,count,dosort,gribex_grib_compare)) {
	  if (grib_get_string(h,"name",name,&namelen) == GRIB_SUCCESS ) {
	    printf("--- %s \n",name);
	  }
      hgribex=grib_handle_new_from_message(c,gribex_msg,gribex_msg_len);
      GRIB_CHECK(grib_get_long(h,"decimalScaleFactor",&grib_decimalScaleFactor),0);
      GRIB_CHECK(grib_get_long(hgribex,"decimalScaleFactor",&gribex_decimalScaleFactor),0);
      GRIB_CHECK(grib_get_long(h,"bitsPerValue",&grib_bitsPerValue),0);
      GRIB_CHECK(grib_get_long(h,"bitsPerValue",&gribex_bitsPerValue),0);
      GRIB_CHECK(grib_get_long(h,"binaryScaleFactor",&grib_binaryScaleFactor),0);
      GRIB_CHECK(grib_get_long(hgribex,"binaryScaleFactor",&gribex_binaryScaleFactor),0);
      GRIB_CHECK(grib_get_double(h,"referenceValue",&grib_referenceValue),0);
      GRIB_CHECK(grib_get_double(hgribex,"referenceValue",&gribex_referenceValue),0);

      printf("decimalScaleFactor=%ld  (grib_api)\n",grib_decimalScaleFactor);
      printf("decimalScaleFactor=%ld  (gribex)\n",gribex_decimalScaleFactor);
      printf("bitsPerValue=%ld  (grib_api)\n",grib_bitsPerValue);
      printf("bitsPerValue=%ld  (gribex)\n",gribex_bitsPerValue);
      printf("binaryScaleFactor=%ld  (grib_api)\n",grib_binaryScaleFactor);
      printf("binaryScaleFactor=%ld  (gribex)\n",gribex_binaryScaleFactor);
      printf("referenceValue=%.20e  (grib_api)\n",grib_referenceValue);
      printf("referenceValue=%.20e  (gribex)\n",gribex_referenceValue);
      if (grib_get_double(h,"laplacianOperator",&grib_laplacianOperator) == GRIB_SUCCESS ) {
        grib_get_double(hgribex,"laplacianOperator",&gribex_laplacianOperator);
        printf("laplacianOperator=%.20e  (grib_api)\n",grib_laplacianOperator);
        printf("laplacianOperator=%.20e  (gribex)\n",gribex_laplacianOperator);
      }
      if (grib_get_long(h,"J",&lval) == GRIB_SUCCESS ) printf("J=%ld  \n",lval);
      if (grib_get_long(h,"K",&lval) == GRIB_SUCCESS ) printf("K=%ld  \n",lval);
      if (grib_get_long(h,"M",&lval) == GRIB_SUCCESS ) printf("M=%ld  \n",lval);
      if (grib_get_long(h,"JS",&lval) == GRIB_SUCCESS ) printf("JS=%ld  \n",lval);
      if (grib_get_long(h,"KS",&lval) == GRIB_SUCCESS ) printf("KS=%ld  \n",lval);
      if (grib_get_long(h,"MS",&lval) == GRIB_SUCCESS ) printf("MS=%ld  \n",lval);
	  namelen=STRMAXLEN;
      printf("----------------------\n");
      grib_handle_delete(hgribex);
    }

    /* free!!!!!! */
    grib_context_free(c,gribex_msg);
    grib_context_free(c,values);
    grib_context_free(c,cvalues);
    grib_context_free(c,grib_values);
    grib_context_free(c,gribex_values);
    grib_handle_delete(h);
	if (foutgrib) fclose(foutgrib);
	if (foutgribex) fclose(foutgribex);

  }

  GRIB_CHECK(err,0);

  return 0;
}

#else

int main(int argc, char* argv[]) {
  return 0;
}
#endif


